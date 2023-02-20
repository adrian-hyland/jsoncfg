#include "json_element.h"
#include "test_json.h"


static bool TestJsonElementSetUp(void)
{
    tJsonElement Element;
    bool ok;

    JsonElementSetUp(&Element);

    ok = (JsonElementGetType(&Element) == json_TypeRoot);

    ok = ok && (JsonElementGetChild(&Element, false) == NULL);

    ok = ok && (JsonElementGetNext(&Element, false) == NULL);

    JsonElementCleanUp(&Element);

    return ok;
}


static bool TestJsonElementCleanUp(void)
{
    tJsonElement Element;
    bool ok;

    JsonElementSetUp(&Element);

    ok = (JsonElementFind(&Element, (const uint8_t *)"/", true) != NULL);

    ok = ok && (JsonElementGetType(&Element) == json_TypeRoot);

    ok = ok && (JsonElementGetChild(&Element, false) != NULL);

    ok = ok && (JsonElementGetNext(&Element, false) == NULL);

    JsonElementCleanUp(&Element);

    ok = ok && (JsonElementGetType(&Element) == json_TypeRoot);

    ok = ok && (JsonElementGetChild(&Element, false) == NULL);

    ok = ok && (JsonElementGetNext(&Element, false) == NULL);

    return ok;
}


static bool TestJsonElementAllocateChild(void)
{
    const struct
    {
        tJsonType ParentType;
        tJsonType ChildType;
        bool      Valid;
    } TestAllocations[] =
    {
        { json_TypeRoot,         json_TypeRoot,         false },
        { json_TypeRoot,         json_TypeObject,       true  },
        { json_TypeRoot,         json_TypeArray,        true  },
        { json_TypeRoot,         json_TypeKey,          true  },
        { json_TypeRoot,         json_TypeValueString,  true  },
        { json_TypeRoot,         json_TypeValueLiteral, true  },
        { json_TypeRoot,         json_TypeComment,      true  },
        { json_TypeObject,       json_TypeRoot,         false },
        { json_TypeObject,       json_TypeObject,       false },
        { json_TypeObject,       json_TypeArray,        false },
        { json_TypeObject,       json_TypeKey,          true  },
        { json_TypeObject,       json_TypeValueString,  false },
        { json_TypeObject,       json_TypeValueLiteral, false },
        { json_TypeObject,       json_TypeComment,      true  },
        { json_TypeArray,        json_TypeRoot,         false },
        { json_TypeArray,        json_TypeObject,       true  },
        { json_TypeArray,        json_TypeArray,        true  },
        { json_TypeArray,        json_TypeKey,          false },
        { json_TypeArray,        json_TypeValueString,  true  },
        { json_TypeArray,        json_TypeValueLiteral, true  },
        { json_TypeArray,        json_TypeComment,      true  },
        { json_TypeKey,          json_TypeRoot,         false },
        { json_TypeKey,          json_TypeObject,       true  },
        { json_TypeKey,          json_TypeArray,        true  },
        { json_TypeKey,          json_TypeKey,          false },
        { json_TypeKey,          json_TypeValueString,  true  },
        { json_TypeKey,          json_TypeValueLiteral, true  },
        { json_TypeKey,          json_TypeComment,      true  },
        { json_TypeValueString,  json_TypeRoot,         false },
        { json_TypeValueString,  json_TypeObject,       false },
        { json_TypeValueString,  json_TypeArray,        false },
        { json_TypeValueString,  json_TypeKey,          false },
        { json_TypeValueString,  json_TypeValueString,  false },
        { json_TypeValueString,  json_TypeValueLiteral, false },
        { json_TypeValueString,  json_TypeComment,      false },
        { json_TypeValueLiteral, json_TypeRoot,         false },
        { json_TypeValueLiteral, json_TypeObject,       false },
        { json_TypeValueLiteral, json_TypeArray,        false },
        { json_TypeValueLiteral, json_TypeKey,          false },
        { json_TypeValueLiteral, json_TypeValueString,  false },
        { json_TypeValueLiteral, json_TypeValueLiteral, false },
        { json_TypeValueLiteral, json_TypeComment,      false },
        { json_TypeComment,      json_TypeRoot,         false },
        { json_TypeComment,      json_TypeObject,       false },
        { json_TypeComment,      json_TypeArray,        false },
        { json_TypeComment,      json_TypeKey,          false },
        { json_TypeComment,      json_TypeValueString,  false },
        { json_TypeComment,      json_TypeValueLiteral, false },
        { json_TypeComment,      json_TypeComment,      false }
    };
    tJsonElement Root;
    tJsonElement *Parent;
    tJsonElement *Child;
    size_t n;
    bool ok;

    JsonElementSetUp(&Root);

    for (ok = true, n = 0; ok && (n < sizeof(TestAllocations) / sizeof(TestAllocations[0])); n++)
    {
        if (TestAllocations[n].ParentType == json_TypeRoot)
        {
            ok = (JsonElementAllocateChild(&Root, TestAllocations[n].ChildType) == TestAllocations[n].Valid);

            Parent = &Root;
        }
        else
        {
            ok = JsonElementAllocateChild(&Root, TestAllocations[n].ParentType);

            Parent = JsonElementGetChild(&Root, false);

            ok = ok && (Parent != NULL);

            ok = ok && (JsonElementGetType(Parent) == TestAllocations[n].ParentType);

            ok = ok && (JsonElementGetChild(Parent, false) == NULL);

            ok = ok && (JsonElementGetNext(Parent, false) == NULL);

            ok = ok && !(!JsonElementAllocateChild(Parent, TestAllocations[n].ChildType) ^ !TestAllocations[n].Valid);
        }

        Child = JsonElementGetChild(Parent, false);

        if (Child != NULL)
        {
            ok = ok && (JsonElementGetType(Child) == TestAllocations[n].ChildType);

            ok = ok && (JsonElementGetChild(Child, false) == NULL);

            ok = ok && (JsonElementGetNext(Child, false) == NULL);
        }
    }

    JsonElementCleanUp(&Root);

    return ok;
}


static bool TestJsonElementAllocateNext(void)
{
    const struct
    {
        tJsonType ParentType;
        tJsonType ChildType;
        tJsonType SiblingType;
        bool      Valid;
    } TestAllocations[] =
    {
        { json_TypeRoot,         json_TypeObject,       json_TypeRoot,         false },
        { json_TypeRoot,         json_TypeObject,       json_TypeObject,       false },
        { json_TypeRoot,         json_TypeObject,       json_TypeArray,        false },
        { json_TypeRoot,         json_TypeObject,       json_TypeKey,          false },
        { json_TypeRoot,         json_TypeObject,       json_TypeValueString,  false },
        { json_TypeRoot,         json_TypeObject,       json_TypeValueLiteral, false },
        { json_TypeRoot,         json_TypeObject,       json_TypeComment,      true  },
        { json_TypeRoot,         json_TypeArray,        json_TypeRoot,         false },
        { json_TypeRoot,         json_TypeArray,        json_TypeObject,       false },
        { json_TypeRoot,         json_TypeArray,        json_TypeArray,        false },
        { json_TypeRoot,         json_TypeArray,        json_TypeKey,          false },
        { json_TypeRoot,         json_TypeArray,        json_TypeValueString,  false },
        { json_TypeRoot,         json_TypeArray,        json_TypeValueLiteral, false },
        { json_TypeRoot,         json_TypeArray,        json_TypeComment,      true  },
        { json_TypeRoot,         json_TypeKey,          json_TypeRoot,         false },
        { json_TypeRoot,         json_TypeKey,          json_TypeObject,       false },
        { json_TypeRoot,         json_TypeKey,          json_TypeArray,        false },
        { json_TypeRoot,         json_TypeKey,          json_TypeKey,          false },
        { json_TypeRoot,         json_TypeKey,          json_TypeValueString,  false },
        { json_TypeRoot,         json_TypeKey,          json_TypeValueLiteral, false },
        { json_TypeRoot,         json_TypeKey,          json_TypeComment,      true  },
        { json_TypeRoot,         json_TypeValueString,  json_TypeRoot,         false },
        { json_TypeRoot,         json_TypeValueString,  json_TypeObject,       false },
        { json_TypeRoot,         json_TypeValueString,  json_TypeArray,        false },
        { json_TypeRoot,         json_TypeValueString,  json_TypeKey,          false },
        { json_TypeRoot,         json_TypeValueString,  json_TypeValueString,  false },
        { json_TypeRoot,         json_TypeValueString,  json_TypeValueLiteral, false },
        { json_TypeRoot,         json_TypeValueString,  json_TypeComment,      true  },
        { json_TypeRoot,         json_TypeValueLiteral, json_TypeRoot,         false },
        { json_TypeRoot,         json_TypeValueLiteral, json_TypeObject,       false },
        { json_TypeRoot,         json_TypeValueLiteral, json_TypeArray,        false },
        { json_TypeRoot,         json_TypeValueLiteral, json_TypeKey,          false },
        { json_TypeRoot,         json_TypeValueLiteral, json_TypeValueString,  false },
        { json_TypeRoot,         json_TypeValueLiteral, json_TypeValueLiteral, false },
        { json_TypeRoot,         json_TypeValueLiteral, json_TypeComment,      true  },
        { json_TypeRoot,         json_TypeComment,      json_TypeRoot,         false },
        { json_TypeRoot,         json_TypeComment,      json_TypeObject,       true  },
        { json_TypeRoot,         json_TypeComment,      json_TypeArray,        true  },
        { json_TypeRoot,         json_TypeComment,      json_TypeKey,          true  },
        { json_TypeRoot,         json_TypeComment,      json_TypeValueString,  true  },
        { json_TypeRoot,         json_TypeComment,      json_TypeValueLiteral, true  },
        { json_TypeRoot,         json_TypeComment,      json_TypeComment,      true  },
        { json_TypeObject,       json_TypeKey,          json_TypeRoot,         false },
        { json_TypeObject,       json_TypeKey,          json_TypeObject,       false },
        { json_TypeObject,       json_TypeKey,          json_TypeArray,        false },
        { json_TypeObject,       json_TypeKey,          json_TypeKey,          true  },
        { json_TypeObject,       json_TypeKey,          json_TypeValueString,  false },
        { json_TypeObject,       json_TypeKey,          json_TypeValueLiteral, false },
        { json_TypeObject,       json_TypeKey,          json_TypeComment,      true  },
        { json_TypeObject,       json_TypeComment,      json_TypeRoot,         false },
        { json_TypeObject,       json_TypeComment,      json_TypeObject,       false },
        { json_TypeObject,       json_TypeComment,      json_TypeArray,        false },
        { json_TypeObject,       json_TypeComment,      json_TypeKey,          true  },
        { json_TypeObject,       json_TypeComment,      json_TypeValueString,  false },
        { json_TypeObject,       json_TypeComment,      json_TypeValueLiteral, false },
        { json_TypeObject,       json_TypeComment,      json_TypeComment,      true  },
        { json_TypeArray,        json_TypeObject,       json_TypeRoot,         false },
        { json_TypeArray,        json_TypeObject,       json_TypeObject,       true  },
        { json_TypeArray,        json_TypeObject,       json_TypeArray,        true  },
        { json_TypeArray,        json_TypeObject,       json_TypeKey,          false },
        { json_TypeArray,        json_TypeObject,       json_TypeValueString,  true  },
        { json_TypeArray,        json_TypeObject,       json_TypeValueLiteral, true  },
        { json_TypeArray,        json_TypeObject,       json_TypeComment,      true  },
        { json_TypeArray,        json_TypeArray,        json_TypeRoot,         false },
        { json_TypeArray,        json_TypeArray,        json_TypeObject,       true  },
        { json_TypeArray,        json_TypeArray,        json_TypeArray,        true  },
        { json_TypeArray,        json_TypeArray,        json_TypeKey,          false },
        { json_TypeArray,        json_TypeArray,        json_TypeValueString,  true  },
        { json_TypeArray,        json_TypeArray,        json_TypeValueLiteral, true  },
        { json_TypeArray,        json_TypeArray,        json_TypeComment,      true  },
        { json_TypeArray,        json_TypeValueString,  json_TypeRoot,         false },
        { json_TypeArray,        json_TypeValueString,  json_TypeObject,       true  },
        { json_TypeArray,        json_TypeValueString,  json_TypeArray,        true  },
        { json_TypeArray,        json_TypeValueString,  json_TypeKey,          false },
        { json_TypeArray,        json_TypeValueString,  json_TypeValueString,  true  },
        { json_TypeArray,        json_TypeValueString,  json_TypeValueLiteral, true  },
        { json_TypeArray,        json_TypeValueString,  json_TypeComment,      true  },
        { json_TypeArray,        json_TypeValueLiteral, json_TypeRoot,         false },
        { json_TypeArray,        json_TypeValueLiteral, json_TypeObject,       true  },
        { json_TypeArray,        json_TypeValueLiteral, json_TypeArray,        true  },
        { json_TypeArray,        json_TypeValueLiteral, json_TypeKey,          false },
        { json_TypeArray,        json_TypeValueLiteral, json_TypeValueString,  true  },
        { json_TypeArray,        json_TypeValueLiteral, json_TypeValueLiteral, true  },
        { json_TypeArray,        json_TypeValueLiteral, json_TypeComment,      true  },
        { json_TypeArray,        json_TypeComment,      json_TypeRoot,         false },
        { json_TypeArray,        json_TypeComment,      json_TypeObject,       true  },
        { json_TypeArray,        json_TypeComment,      json_TypeArray,        true  },
        { json_TypeArray,        json_TypeComment,      json_TypeKey,          false },
        { json_TypeArray,        json_TypeComment,      json_TypeValueString,  true  },
        { json_TypeArray,        json_TypeComment,      json_TypeValueLiteral, true  },
        { json_TypeArray,        json_TypeComment,      json_TypeComment,      true  },
        { json_TypeKey,          json_TypeObject,       json_TypeRoot,         false },
        { json_TypeKey,          json_TypeObject,       json_TypeObject,       false },
        { json_TypeKey,          json_TypeObject,       json_TypeArray,        false },
        { json_TypeKey,          json_TypeObject,       json_TypeKey,          false },
        { json_TypeKey,          json_TypeObject,       json_TypeValueString,  false },
        { json_TypeKey,          json_TypeObject,       json_TypeValueLiteral, false },
        { json_TypeKey,          json_TypeObject,       json_TypeComment,      true  },
        { json_TypeKey,          json_TypeArray,        json_TypeRoot,         false },
        { json_TypeKey,          json_TypeArray,        json_TypeObject,       false },
        { json_TypeKey,          json_TypeArray,        json_TypeArray,        false },
        { json_TypeKey,          json_TypeArray,        json_TypeKey,          false },
        { json_TypeKey,          json_TypeArray,        json_TypeValueString,  false },
        { json_TypeKey,          json_TypeArray,        json_TypeValueLiteral, false },
        { json_TypeKey,          json_TypeArray,        json_TypeComment,      true  },
        { json_TypeKey,          json_TypeValueString,  json_TypeRoot,         false },
        { json_TypeKey,          json_TypeValueString,  json_TypeObject,       false },
        { json_TypeKey,          json_TypeValueString,  json_TypeArray,        false },
        { json_TypeKey,          json_TypeValueString,  json_TypeKey,          false },
        { json_TypeKey,          json_TypeValueString,  json_TypeValueString,  false },
        { json_TypeKey,          json_TypeValueString,  json_TypeValueLiteral, false },
        { json_TypeKey,          json_TypeValueString,  json_TypeComment,      true  },
        { json_TypeKey,          json_TypeValueLiteral, json_TypeRoot,         false },
        { json_TypeKey,          json_TypeValueLiteral, json_TypeObject,       false },
        { json_TypeKey,          json_TypeValueLiteral, json_TypeArray,        false },
        { json_TypeKey,          json_TypeValueLiteral, json_TypeKey,          false },
        { json_TypeKey,          json_TypeValueLiteral, json_TypeValueString,  false },
        { json_TypeKey,          json_TypeValueLiteral, json_TypeValueLiteral, false },
        { json_TypeKey,          json_TypeValueLiteral, json_TypeComment,      true  },
        { json_TypeKey,          json_TypeComment,      json_TypeRoot,         false },
        { json_TypeKey,          json_TypeComment,      json_TypeObject,       true  },
        { json_TypeKey,          json_TypeComment,      json_TypeArray,        true  },
        { json_TypeKey,          json_TypeComment,      json_TypeKey,          false },
        { json_TypeKey,          json_TypeComment,      json_TypeValueString,  true  },
        { json_TypeKey,          json_TypeComment,      json_TypeValueLiteral, true  },
        { json_TypeKey,          json_TypeComment,      json_TypeComment,      true  }
    };
    tJsonElement Root;
    tJsonElement *Parent;
    tJsonElement *Child;
    tJsonElement *Sibling;
    size_t n;
    bool ok;

    JsonElementSetUp(&Root);

    for (ok = true, n = 0; ok && (n < sizeof(TestAllocations) / sizeof(TestAllocations[0])); n++)
    {
        if (TestAllocations[n].ParentType == json_TypeRoot)
        {
            Parent = &Root;
        }
        else
        {
            ok = JsonElementAllocateChild(&Root, TestAllocations[n].ParentType);

            Parent = JsonElementGetChild(&Root, false);
        }

        ok = ok && (Parent != NULL);

        ok = ok && JsonElementAllocateChild(Parent, TestAllocations[n].ChildType);

        Child = (Parent != NULL) ? JsonElementGetChild(Parent, false) : NULL;

        ok = ok && (Child != NULL);

        ok = ok && !(!JsonElementAllocateNext(Child, TestAllocations[n].SiblingType) ^ !TestAllocations[n].Valid);

        Sibling = JsonElementGetNext(Child, false);

        if (Sibling != NULL)
        {
            ok = ok && (JsonElementGetType(Sibling) == TestAllocations[n].SiblingType);

            ok = ok && (JsonElementGetChild(Sibling, false) == NULL);

            ok = ok && (JsonElementGetNext(Sibling, false) == NULL);
        }
    }

    JsonElementCleanUp(&Root);

    return ok;
}


static bool TestJsonElementGetChild(void)
{
    tJsonElement Root;
    tJsonElement *Element;
    bool ok;

    JsonElementSetUp(&Root);

    ok = (JsonElementGetChild(&Root, false) == NULL) && (JsonElementGetChild(&Root, true) == NULL);

    ok = ok && JsonElementAllocateChild(&Root, json_TypeComment);

    Element = JsonElementGetChild(&Root, false);

    ok = ok && (Element != NULL) && (Element->Type == json_TypeComment);

    ok = ok && (JsonElementGetChild(&Root, true) == NULL);

    ok = ok && JsonElementAllocateNext(Element, json_TypeObject);

    Element = JsonElementGetChild(&Root, false);

    ok = ok && (Element != NULL) && (Element->Type == json_TypeComment);

    Element = JsonElementGetChild(&Root, true);

    ok = ok && (Element != NULL) && (Element->Type == json_TypeObject);

    JsonElementCleanUp(&Root);

    return ok;
}


static bool TestJsonElementGetNext(void)
{
    tJsonElement Root;
    tJsonElement *ChildElement;
    tJsonElement *NextElement;
    bool ok;

    JsonElementSetUp(&Root);

    ok = JsonElementAllocateChild(&Root, json_TypeObject);

    ChildElement = JsonElementGetChild(&Root, false);

    ok = ok && (ChildElement != NULL);

    ok = ok && JsonElementAllocateChild(ChildElement, json_TypeKey);

    ChildElement = JsonElementGetChild(ChildElement, false);

    ok = ok && (JsonElementGetNext(ChildElement, false) == NULL) && (JsonElementGetNext(ChildElement, true) == NULL);

    ok = ok && JsonElementAllocateNext(ChildElement, json_TypeComment);

    NextElement = JsonElementGetNext(ChildElement, false);

    ok = ok && (NextElement != NULL) && (NextElement->Type == json_TypeComment);

    ok = ok && (JsonElementGetNext(ChildElement, true) == NULL);

    ok = ok && JsonElementAllocateNext(NextElement, json_TypeKey);

    NextElement = JsonElementGetNext(ChildElement, false);

    ok = ok && (NextElement != NULL) && (NextElement->Type == json_TypeComment);

    NextElement = JsonElementGetNext(ChildElement, true);

    ok = ok && (NextElement != NULL) && (NextElement->Type == json_TypeKey);

    JsonElementCleanUp(&Root);

    return ok;
}


static bool TestJsonElementFind(void)
{
    static struct
    {
        const uint8_t *Path;
        tJsonType      ElementType;
    } CreatePaths[] =
    {
        { (const uint8_t *)"/",                                               json_TypeObject       },
        { (const uint8_t *)"/key1:123",                                       json_TypeValueLiteral },
        { (const uint8_t *)"/key2:\"hello world\"",                           json_TypeValueString  },
        { (const uint8_t *)"/key3[/object:1]",                                json_TypeObject       },
        { (const uint8_t *)"/key3[/object:1]/key31:\"hello again\"",          json_TypeValueString  },
        { (const uint8_t *)"/key3[/object:2]",                                json_TypeObject       },
        { (const uint8_t *)"/key3[/object:2]/key32[/object:321]",             json_TypeObject       },
        { (const uint8_t *)"/key3[/object:2]/key32[/object:322]",             json_TypeObject       },
        { (const uint8_t *)"/key3[/object:2]/key32[/object:322]/key322:true", json_TypeValueLiteral }
    };
    const uint8_t *InvalidPaths[] =
    {
        (const uint8_t *)"/key1:12",
        (const uint8_t *)"/key4",
        (const uint8_t *)"/key3[object:3]",
        (const uint8_t *)"/key3[object:1]/key32",
        (const uint8_t *)"/key3[object:2]/key31",
        (const uint8_t *)"/key3[object:2]/key32[/object:323]",
        (const uint8_t *)"/key3[object:2]/key32[/object:322]/key321",
    };
    tJsonElement Root;
    tJsonElement *Element;
    size_t n;
    bool ok;

    JsonElementSetUp(&Root);

    for (ok = true, n = 0; ok && (n < sizeof(CreatePaths) / sizeof(CreatePaths[0])); n++)
    {
        Element = JsonElementFind(&Root, CreatePaths[n].Path, true);

        ok = (Element != NULL);

        ok = ok && (JsonElementGetType(Element) == CreatePaths[n].ElementType);
    }

    for (n = 0; ok && (n < sizeof(InvalidPaths) / sizeof(InvalidPaths[0])); n++)
    {
        Element = JsonElementFind(&Root, InvalidPaths[n], false);

        ok = (Element == NULL);
    }

    for (n = 0; ok && (n < sizeof(CreatePaths) / sizeof(CreatePaths[0])); n++)
    {
        Element = JsonElementFind(&Root, CreatePaths[n].Path, false);

        ok = (Element != NULL);

        ok = ok && (JsonElementGetType(Element) == CreatePaths[n].ElementType);
    }

    JsonElementCleanUp(&Root);

    return ok;
}


static const tTestCase TestCaseJsonElement[] =
{
    { "JsonElementSetUp",         TestJsonElementSetUp         },
    { "JsonElementCleanUp",       TestJsonElementCleanUp       },
    { "JsonElementAllocateChild", TestJsonElementAllocateChild },
    { "JsonElementAllocateNext",  TestJsonElementAllocateNext  },
    { "JsonElementGetChild",      TestJsonElementGetChild      },
    { "JsonElementGetNext",       TestJsonElementGetNext       },
    { "JsonElementFind",          TestJsonElementFind          },
};


const tTest TestJsonElement =
{
    "JsonElement",
    TestCaseJsonElement,
    sizeof(TestCaseJsonElement) / sizeof(TestCaseJsonElement[0])
};
