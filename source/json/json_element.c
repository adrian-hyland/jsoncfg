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


static int JsonElementCheckChildType(tJsonType ParentType, tJsonType ChildType)
{
    int ok;

    ok = (ParentType != ChildType);

    if (ParentType != json_TypeRoot)
    {
        ok = ok && (ParentType != json_TypeValueLiteral);
        
        ok = ok && (ParentType != json_TypeValueString);
        
        ok = ok && ((ParentType != json_TypeObject) || (ChildType == json_TypeKey));
        
        ok = ok && ((ParentType != json_TypeArray) || (ChildType != json_TypeKey));
    }

    return ok;
}


tJsonElement *JsonElementAllocate(tJsonType Type, tJsonElement *Parent)
{
    tJsonElement *Element;

    if (Type == json_TypeRoot)
    {
        if (Parent != NULL)
        {
            return NULL;
        }
    }
    else
    {
        if ((Parent == NULL) || !JsonElementCheckChildType(Parent->Type, Type))
        {
            return NULL;
        }
    }

    Element = malloc(sizeof(tJsonElement));
    if (Element != NULL)
    {
        JsonElementSetUpType(Element, Type, Parent);
    }

    return Element;
}


void JsonElementFree(tJsonElement **Element)
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


tJsonType JsonElementGetType(tJsonElement *Element)
{
    return Element->Type;
}


tJsonElement *JsonElementGetChild(tJsonElement *Element)
{
    return (Element != NULL) ? Element->Child : NULL;
}


tJsonElement *JsonElementGetNext(tJsonElement *Element)
{
    return (Element != NULL) ? Element->Next : NULL;
}


static tJsonElement **JsonElementFindSubPath(tJsonElement **Element, tJsonElement *Parent, const uint8_t *Path, size_t PathLength, int Create);


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
            if (JsonElementFindSubPath(&Element->Child, Element, Path, PathLength, 1) == NULL)
            {
                JsonElementFree(&Element);
            }
        }
    }

    return Element;
}


static tJsonElement **JsonElementFindSubPath(tJsonElement **Element, tJsonElement *Parent, const uint8_t *Path, size_t PathLength, int Create)
{
    tJsonType ComponentType;
    const uint8_t *Component;
    size_t ComponentLength;
    size_t PathIndex;
    size_t Length;

    PathIndex = 0;

    for (;;)
    {
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
                if (JsonElementFindSubPath(Element, Parent, Component, ComponentLength, 0) != NULL)
                {
                    break;
                }

                Element = &(*Element)->Next;
            }

            if ((*Element == NULL) && Create)
            {
                if (JsonElementFindSubPath(Element, Parent, Component, ComponentLength, 1) == NULL)
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

                Element = &(*Element)->Next;
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


tJsonElement *JsonElementFind(tJsonElement *Element, const uint8_t *Path, int Create)
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
