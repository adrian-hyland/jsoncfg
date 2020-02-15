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


tJsonElement *JsonElementAllocate(tJsonType Type, tJsonElement *Parent)
{
    tJsonElement *Element;

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
                    JsonElementCleanUp(*Element);
                    return NULL;
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
    size_t PathLength;

    if ((Path != NULL) && (Element != NULL) && (Element->Type == json_TypeRoot))
    {
        PathLength = strlen((char *)Path);

        while ((PathLength > 0) && (*Path == ' '))
        {
            Path++;
            PathLength--;
        }

        while ((PathLength > 0) && (Path[PathLength - 1] == ' '))
        {
            PathLength--;
        }

        ElementReference = JsonElementFindSubPath(&Element->Child, Element, Path, PathLength, Create);
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
