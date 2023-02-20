#include <stdlib.h>
#include <string.h>
#include "json_path.h"
#include "json_element.h"


static void JsonElementSetUpType(tJsonElement *Element, tJsonType Type, tJsonElement *Parent)
{
    Element->Type = Type;
    Element->Parent = Parent;
    Element->Next = NULL;
    Element->Child = NULL;
    JsonStringSetUp(&Element->Name);
}


static bool JsonElementCheckChildType(tJsonType ParentType, tJsonType ChildType)
{
    bool ok;

    ok = (ChildType != json_TypeRoot);

    if (ParentType != json_TypeRoot)
    {
        ok = ok && ((ParentType == json_TypeArray) || (ParentType != ChildType));

        ok = ok && (ParentType != json_TypeValueLiteral);
        
        ok = ok && (ParentType != json_TypeValueString);
        
        ok = ok && (ParentType != json_TypeComment);
        
        ok = ok && ((ParentType != json_TypeObject) || (ChildType == json_TypeKey) || (ChildType == json_TypeComment));
        
        ok = ok && ((ParentType != json_TypeArray) || (ChildType != json_TypeKey));
    }

    return ok;
}


static tJsonElement *JsonElementAllocate(tJsonType Type, tJsonElement *Parent)
{
    tJsonElement *Element;

    if ((Type == json_TypeRoot) || (Parent == NULL) || !JsonElementCheckChildType(Parent->Type, Type))
    {
        return NULL;
    }

    Element = (tJsonElement *)malloc(sizeof(tJsonElement));
    if (Element != NULL)
    {
        JsonElementSetUpType(Element, Type, Parent);
    }

    return Element;
}


static void JsonElementFree(tJsonElement **Element)
{
    if (*Element != NULL)
    {
        JsonElementCleanUp(*Element);
        free(*Element);
        *Element = NULL;
    }
}


void JsonElementSetUp(tJsonElement *Element)
{
    JsonElementSetUpType(Element, json_TypeRoot, NULL);
}


void JsonElementCleanUp(tJsonElement *Element)
{
    JsonStringCleanUp(&Element->Name);
    JsonElementFree(&Element->Child);
    JsonElementFree(&Element->Next);
}


bool JsonElementAllocateChild(tJsonElement *Element, tJsonType Type)
{
    JsonElementFree(&Element->Child);

    Element->Child = JsonElementAllocate(Type, Element);

    return Element->Child != NULL;
}


bool JsonElementAllocateNext(tJsonElement *Element, tJsonType Type)
{
    JsonElementFree(&Element->Next);

    if (Element->Parent != NULL)
    {
        if ((Type == json_TypeComment) || ((Element->Parent->Type != json_TypeRoot) && (Element->Parent->Type != json_TypeKey)) || (JsonElementGetChild(Element->Parent, true) == NULL))
        {
            Element->Next = JsonElementAllocate(Type, Element->Parent);
        }
    }

    return Element->Next != NULL;
}


tJsonType JsonElementGetType(tJsonElement *Element)
{
    return Element->Type;
}


tJsonElement *JsonElementGetChild(tJsonElement *Element, bool IgnoreComments)
{
    if (Element != NULL)
    {
        Element = Element->Child;
        while (IgnoreComments && (Element != NULL) && (Element->Type == json_TypeComment))
        {
            Element = Element->Next;
        }
    }

    return Element;
}


tJsonElement *JsonElementGetNext(tJsonElement *Element, bool IgnoreComments)
{
    if (Element != NULL)
    {
        do
        {
            Element = Element->Next;
        }
        while (IgnoreComments && (Element != NULL) && (Element->Type == json_TypeComment));
    }

    return Element;
}


static tJsonElement **JsonElementFindSubPath(tJsonElement **Element, tJsonElement *Parent, const uint8_t *Path, size_t PathLength, bool Create);


static tJsonElement *JsonElementCreatePath(tJsonType Type, tJsonElement *Parent, const uint8_t *Path, size_t PathLength)
{
    tJsonElement *Element;

    Element = JsonElementAllocate(Type, Parent);
    if (Element != NULL)
    {
        if ((Type == json_TypeKey) || (Type == json_TypeValueString) || (Type == json_TypeValueLiteral))
        {
            if (!JsonPathSetString(Path, PathLength, &Element->Name))
            {
                JsonElementFree(&Element);
            }
        }
        else if (Type == json_TypeArray)
        {
            if (JsonElementFindSubPath(&Element->Child, Element, Path, PathLength, true) == NULL)
            {
                JsonElementFree(&Element);
            }
        }
    }

    return Element;
}


static tJsonElement **JsonElementFindSubPath(tJsonElement **Element, tJsonElement *Parent, const uint8_t *Path, size_t PathLength, bool Create)
{
    tJsonType ComponentType;
    const uint8_t *Component;
    size_t ComponentLength;
    size_t PathIndex;
    size_t Length;

    PathIndex = 0;

    for (;;)
    {
        while ((*Element != NULL) && ((*Element)->Type == json_TypeComment))
        {
            Element = &(*Element)->Next;
        }

        Length = JsonPathGetComponent(&Path[PathIndex], PathLength - PathIndex, &ComponentType, &Component, &ComponentLength);
        if (Length == 0)
        {
            return NULL;
        }
        else if (*Element == NULL)
        {
            if (Create)
            {
                *Element = JsonElementCreatePath(ComponentType, Parent, Component, ComponentLength);
                if ((ComponentType == json_TypeArray) && (*Element != NULL))
                {
                    Parent = *Element;
                    Element = &Parent->Child;
                }
            }
        }
        else if (ComponentType != (*Element)->Type)
        {
            return NULL;
        }
        else if (ComponentType == json_TypeArray)
        {
            Parent = *Element;
            Element = &Parent->Child;

            while (*Element != NULL)
            {
                if (JsonElementFindSubPath(Element, Parent, Component, ComponentLength, false) != NULL)
                {
                    break;
                }

                do
                {
                    Element = &(*Element)->Next;
                }
                while ((*Element != NULL) && ((*Element)->Type == json_TypeComment));
            }

            if ((*Element == NULL) && Create)
            {
                if (JsonElementFindSubPath(Element, Parent, Component, ComponentLength, true) == NULL)
                {
                    JsonElementFree(Element);
                }
            }
        }
        else if (ComponentType == json_TypeKey)
        {
            while (*Element != NULL)
            {
                if (JsonPathCompareString(Component, ComponentLength, &(*Element)->Name))
                {
                    break;
                }

                do
                {
                    Element = &(*Element)->Next;
                }
                while ((*Element != NULL) && ((*Element)->Type == json_TypeComment));
            }

            if ((*Element == NULL) && Create)
            {
                *Element = JsonElementCreatePath(json_TypeKey, Parent, Component, ComponentLength);
            }
        }
        else if (ComponentType != json_TypeObject)
        {
            if (!JsonPathCompareString(Component, ComponentLength, &(*Element)->Name))
            {
                return NULL;
            }
        }

        if (*Element == NULL)
        {
            return NULL;
        }

        PathIndex = PathIndex + Length;
        if (PathIndex == PathLength)
        {
            return Element;
        }

        if (ComponentType != json_TypeArray)
        {
            Parent = *Element;
            Element = &Parent->Child;
        }
    }

    return Element;
}


tJsonElement *JsonElementFind(tJsonElement *Element, const uint8_t *Path, bool Create)
{
    tJsonElement **ElementReference;

    if ((Path != NULL) && (Element != NULL) && (Element->Type == json_TypeRoot))
    {
        ElementReference = JsonElementFindSubPath(&Element->Child, Element, Path, strlen((const char *)Path), Create);
        Element = (ElementReference != NULL) ? *ElementReference : NULL;
    }
    else
    {
        Element = NULL;
    }

    return Element;
}


tJsonElement *JsonElementMoveChild(tJsonElement *To, tJsonElement *From)
{
    tJsonElement *Child;

    if ((From == NULL) || (To == NULL))
    {
        return NULL;
    }

    if (From != To)
    {
        if ((From->Child != NULL) && !JsonElementCheckChildType(To->Type, From->Child->Type))
        {
            return NULL;
        }

        if (To->Child != NULL)
        {
            JsonElementFree(&To->Child);
        }

        To->Child = From->Child;
        From->Child = NULL;

        Child = To->Child;
        while (Child != NULL)
        {
            Child->Parent = To;
            Child = Child->Next;
        }
    }

    return To->Child;
}
