#include <stdlib.h>
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
	else
	{
		ok = ok && (ChildType != json_TypeKey);
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


static void JsonElementClearChild(tJsonElement *Element);


static void JsonElementClearNext(tJsonElement *Element)
{
	tJsonElement *Next;

	while (Element->Next != NULL)
	{
		Next = Element->Next;
		Element->Next = Next->Next;
		Next->Next = NULL;
		JsonStringClear(&Next->Name);
		JsonElementClearChild(Next);
		free(Next);
	}
}


static void JsonElementClearChild(tJsonElement *Element)
{
	tJsonElement *Child;

	while (Element->Child != NULL)
	{
		Child = Element->Child;
		Element->Child = Child->Child;
		Child->Child = NULL;
		JsonStringClear(&Child->Name);
		JsonElementClearNext(Child);
		free(Child);
	}
}


void JsonElementClear(tJsonElement *Element)
{
	JsonStringClear(&Element->Name);
	JsonElementClearChild(Element);
	JsonElementClearNext(Element);
}


void JsonElementSetUp(tJsonElement *Element)
{
	JsonElementSetUpType(Element, json_TypeRoot, NULL);
}


void JsonElementCleanUp(tJsonElement *Element)
{
	JsonElementClear(Element);
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


static tJsonElement **JsonElementFindSubPath(tJsonElement **Element, tJsonElement *Parent, tJsonPath Path, bool Create);


static tJsonElement *JsonElementCreatePath(tJsonType Type, tJsonElement *Parent, tJsonPath Path)
{
	tJsonElement *Element;

	Element = JsonElementAllocate(Type, Parent);
	if (Element != NULL)
	{
		if ((Type == json_TypeKey) || (Type == json_TypeValueString) || (Type == json_TypeValueLiteral))
		{
			if (!JsonPathGetString(Path, &Element->Name))
			{
				JsonElementFree(&Element);
			}
		}
		else if (Type == json_TypeArray)
		{
			if (JsonElementFindSubPath(&Element->Child, Element, Path, true) == NULL)
			{
				JsonElementFree(&Element);
			}
		}
	}

	return Element;
}


static tJsonElement **JsonElementFindSubPath(tJsonElement **Element, tJsonElement *Parent, tJsonPath Path, bool Create)
{
	tJsonPath Component;
	tJsonType ComponentType;
	size_t Length;

	for (;;)
	{
		while ((*Element != NULL) && ((*Element)->Type == json_TypeComment))
		{
			Element = &(*Element)->Next;
		}

		Length = JsonPathGetComponent(Path, &ComponentType, &Component);
		if (Length == 0)
		{
			return NULL;
		}
		else if (*Element == NULL)
		{
			if (Create)
			{
				*Element = JsonElementCreatePath(ComponentType, Parent, Component);
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
				if (JsonElementFindSubPath(Element, Parent, Component, false) != NULL)
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
				if (JsonElementFindSubPath(Element, Parent, Component, true) == NULL)
				{
					JsonElementFree(Element);
				}
			}
		}
		else if (ComponentType == json_TypeKey)
		{
			while (*Element != NULL)
			{
				if (JsonPathCompareString(Component, &(*Element)->Name))
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
				*Element = JsonElementCreatePath(json_TypeKey, Parent, Component);
			}
		}
		else if (ComponentType != json_TypeObject)
		{
			if (!JsonPathCompareString(Component, &(*Element)->Name))
			{
				return NULL;
			}
		}

		if (*Element == NULL)
		{
			return NULL;
		}

		Path = JsonPathRight(Path, Length);
		if (Path.Length == 0)
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


tJsonElement *JsonElementFind(tJsonElement *Element, tJsonPath Path, bool Create)
{
	tJsonElement **ElementReference;

	if ((Element != NULL) && (Element->Type == json_TypeRoot))
	{
		ElementReference = JsonElementFindSubPath(&Element->Child, Element, Path, Create);
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
		if (((To->Type == json_TypeRoot)   && (From->Type == json_TypeRoot))   ||
		    ((To->Type == json_TypeObject) && (From->Type == json_TypeObject)) ||
			 ((To->Type == json_TypeKey)    && ((From->Type == json_TypeRoot) || (From->Type == json_TypeKey))) ||
		    ((To->Type == json_TypeArray)  && ((From->Type == json_TypeRoot) || (From->Type == json_TypeArray))))
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
		else
		{
			return NULL;
		}
	}

	return To->Child;
}
