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


void JsonElementFree(tJsonElement *Element)
{
    JsonStringCleanUp(&Element->Name);
    if (Element->Child != NULL)
    {
        JsonElementFree(Element->Child);
        free(Element->Child);
        Element->Child = NULL;
    }
    if (Element->Next != NULL)
    {
        JsonElementFree(Element->Next);
        free(Element->Next);
        Element->Next = NULL;
    }
}


void JsonElementSetUp(tJsonElement *Element)
{
    JsonElementSetUpType(Element, json_TypeRoot, NULL);
}


void JsonElementCleanUp(tJsonElement *Element)
{
    JsonElementFree(Element);
}


static tJsonElement *JsonElementFindSubPath(tJsonElement *Element, const uint8_t *Path, size_t PathLength, int Create);


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
                JsonElementFree(Element);
                Element = NULL;
            }
        }
        else if (Type == json_TypeArray)
        {
            if (JsonElementFindSubPath(Element, Path, PathLength, 1) == NULL)
            {
                JsonElementFree(Element);
                Element = NULL;
            }
        }
    }

    return Element;
}


static tJsonElement *JsonElementFindSubPath(tJsonElement *Element, const uint8_t *Path, size_t PathLength, int Create)
{
    tJsonElement ArrayElement;
    tJsonType ComponentType;
    const uint8_t *Component;
    size_t ComponentLength;
    size_t PathIndex;
    size_t Length;
   
    JsonElementSetUpType(&ArrayElement, json_TypeArray, NULL);

    for (PathIndex = 0; (Element != NULL) && (PathIndex < PathLength); PathIndex = PathIndex + Length)
    {
        Length = JsonPathGetComponent(&Path[PathIndex], PathLength - PathIndex, &ComponentType, &Component, &ComponentLength);
        if (Length == 0)
        {
            return NULL;
        }

        if (Element->Child == NULL)
        {
            if (Create)
            {
                Element->Child = JsonElementCreatePath(ComponentType, Element, Component, ComponentLength);
            }
            Element = Element->Child;
        }
        else
        {
            Element = Element->Child;
            if ((Element == NULL) || (ComponentType != Element->Type))
            {
                return NULL;
            }

            if (ComponentType == json_TypeArray)
            {
                if (Element->Child == NULL)
                {
                    if (Create)
                    {
                        if (JsonElementFindSubPath(Element, Component, ComponentLength, 1) == NULL)
                        {
                            JsonElementCleanUp(Element);
                        }
                    }
                    Element = Element->Child;
                }
                else
                {            
                    Element = Element->Child;
                    while (Element != NULL)
                    {
                        ArrayElement.Child = Element;
                        if (JsonElementFindSubPath(&ArrayElement, Component, ComponentLength, 0) != NULL)
                        {
                            Element = &ArrayElement;
                            break;
                        }

                        if ((Element->Next == NULL) && Create)
                        {
                            ArrayElement.Child = NULL;
                            if (JsonElementFindSubPath(&ArrayElement, Component, ComponentLength, 1) == NULL)
                            {
                                JsonElementCleanUp(&ArrayElement);
                                Element = NULL;
                            }
                            else
                            {
                                Element->Next = ArrayElement.Child;
                                Element->Next->Parent = Element->Parent;
                                Element = &ArrayElement;
                            }
                            break;
                        }

                        Element = Element->Next;
                    }
                }
            }
            else if (ComponentType == json_TypeKey)
            {
                while (Element != NULL)
                {
                    if (JsonPathCompareString(Component, ComponentLength, &Element->Name))
                    {
                        break;
                    }

                    if ((Element->Next == NULL) && Create)
                    {
                        Element->Next = JsonElementCreatePath(json_TypeKey, Element->Parent, Component, ComponentLength);
                        Element = Element->Next;
                        break;
                    }

                    Element = Element->Next;
                }
            }
            else if (ComponentType != json_TypeObject)
            {
                if (!JsonPathCompareString(Component, ComponentLength, &Element->Name))
                {
                    Element = NULL;
                }
            }
        }
    }

    return Element;
}


tJsonElement *JsonElementFind(tJsonElement *Element, const uint8_t *Path, int Create)
{
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

        Element = JsonElementFindSubPath(Element, Path, PathLength, Create);
    }
    else
    {
        Element = NULL;
    }

    return Element;
}
