#include "json_element.h"
#include "test_json.h"


static int TestJsonElementSetUp(void)
{
    tJsonElement Element;
    int ok;

    JsonElementSetUp(&Element);

    ok = (JsonElementGetType(&Element) == json_TypeRoot);

    ok = ok && (JsonElementGetChild(&Element) == NULL);

    ok = ok && (JsonElementGetNext(&Element) == NULL);

    JsonElementCleanUp(&Element);

    return ok;
}


static int TestJsonElementCleanUp(void)
{
    tJsonElement Element;
    int ok;

    JsonElementSetUp(&Element);

    ok = (JsonElementFind(&Element, (const uint8_t *)"/", 1) != NULL);

    ok = ok && (JsonElementGetType(&Element) == json_TypeRoot);

    ok = ok && (JsonElementGetChild(&Element) != NULL);

    ok = ok && (JsonElementGetNext(&Element) == NULL);

    JsonElementCleanUp(&Element);

    ok = ok && (JsonElementGetType(&Element) == json_TypeRoot);

    ok = ok && (JsonElementGetChild(&Element) == NULL);

    ok = ok && (JsonElementGetNext(&Element) == NULL);

    return ok;
}


static int TestJsonElementAllocateChild(void)
{
    const struct
    {
        tJsonType ParentType;
        tJsonType ChildType;
        int       Valid;
    } TestAllocations[] =
    {
        { json_TypeRoot,         json_TypeRoot,         0 },
        { json_TypeRoot,         json_TypeObject,       1 },
        { json_TypeRoot,         json_TypeArray,        1 },
        { json_TypeRoot,         json_TypeKey,          1 },
        { json_TypeRoot,         json_TypeValueString,  1 },
        { json_TypeRoot,         json_TypeValueLiteral, 1 },
        { json_TypeObject,       json_TypeRoot,         0 },
        { json_TypeObject,       json_TypeObject,       0 },
        { json_TypeObject,       json_TypeArray,        0 },
        { json_TypeObject,       json_TypeKey,          1 },
        { json_TypeObject,       json_TypeValueString,  0 },
        { json_TypeObject,       json_TypeValueLiteral, 0 },
        { json_TypeArray,        json_TypeRoot,         0 },
        { json_TypeArray,        json_TypeObject,       1 },
        { json_TypeArray,        json_TypeArray,        1 },
        { json_TypeArray,        json_TypeKey,          0 },
        { json_TypeArray,        json_TypeValueString,  1 },
        { json_TypeArray,        json_TypeValueLiteral, 1 },
        { json_TypeKey,          json_TypeRoot,         0 },
        { json_TypeKey,          json_TypeObject,       1 },
        { json_TypeKey,          json_TypeArray,        1 },
        { json_TypeKey,          json_TypeKey,          0 },
        { json_TypeKey,          json_TypeValueString,  1 },
        { json_TypeKey,          json_TypeValueLiteral, 1 },
        { json_TypeValueString,  json_TypeRoot,         0 },
        { json_TypeValueString,  json_TypeObject,       0 },
        { json_TypeValueString,  json_TypeArray,        0 },
        { json_TypeValueString,  json_TypeKey,          0 },
        { json_TypeValueString,  json_TypeValueString,  0 },
        { json_TypeValueString,  json_TypeValueLiteral, 0 },
        { json_TypeValueLiteral, json_TypeRoot,         0 },
        { json_TypeValueLiteral, json_TypeObject,       0 },
        { json_TypeValueLiteral, json_TypeArray,        0 },
        { json_TypeValueLiteral, json_TypeKey,          0 },
        { json_TypeValueLiteral, json_TypeValueString,  0 },
        { json_TypeValueLiteral, json_TypeValueLiteral, 0 },
    };
    tJsonElement Root;
    tJsonElement *Parent;
    tJsonElement *Child;
    size_t n;
    int ok;

    JsonElementSetUp(&Root);

    for (ok = 1, n = 0; ok && (n < sizeof(TestAllocations) / sizeof(TestAllocations[0])); n++)
    {
        if (TestAllocations[n].ParentType == json_TypeRoot)
        {
            ok = (JsonElementAllocateChild(&Root, TestAllocations[n].ChildType) == TestAllocations[n].Valid);

            Parent = &Root;
        }
        else
        {
            ok = JsonElementAllocateChild(&Root, TestAllocations[n].ParentType);

            Parent = JsonElementGetChild(&Root);

            ok = ok && (Parent != NULL);

            ok = ok && (JsonElementGetType(Parent) == TestAllocations[n].ParentType);

            ok = ok && (JsonElementGetChild(Parent) == NULL);

            ok = ok && (JsonElementGetNext(Parent) == NULL);

            ok = ok && !(!JsonElementAllocateChild(Parent, TestAllocations[n].ChildType) ^ !TestAllocations[n].Valid);
        }

        Child = JsonElementGetChild(Parent);

        if (Child != NULL)
        {
            ok = ok && (JsonElementGetType(Child) == TestAllocations[n].ChildType);

            ok = ok && (JsonElementGetChild(Child) == NULL);

            ok = ok && (JsonElementGetNext(Child) == NULL);
        }
    }

    JsonElementCleanUp(&Root);

    return ok;
}


static int TestJsonElementAllocateNext(void)
{
    const struct
    {
        tJsonType ParentType;
        tJsonType ChildType;
        tJsonType SiblingType;
        int       Valid;
    } TestAllocations[] =
    {
        { json_TypeRoot,         json_TypeObject,       json_TypeRoot,         0 },
        { json_TypeRoot,         json_TypeObject,       json_TypeObject,       0 },
        { json_TypeRoot,         json_TypeObject,       json_TypeArray,        0 },
        { json_TypeRoot,         json_TypeObject,       json_TypeKey,          0 },
        { json_TypeRoot,         json_TypeObject,       json_TypeValueString,  0 },
        { json_TypeRoot,         json_TypeObject,       json_TypeValueLiteral, 0 },
        { json_TypeRoot,         json_TypeArray,        json_TypeRoot,         0 },
        { json_TypeRoot,         json_TypeArray,        json_TypeObject,       0 },
        { json_TypeRoot,         json_TypeArray,        json_TypeArray,        0 },
        { json_TypeRoot,         json_TypeArray,        json_TypeKey,          0 },
        { json_TypeRoot,         json_TypeArray,        json_TypeValueString,  0 },
        { json_TypeRoot,         json_TypeArray,        json_TypeValueLiteral, 0 },
        { json_TypeRoot,         json_TypeKey,          json_TypeRoot,         0 },
        { json_TypeRoot,         json_TypeKey,          json_TypeObject,       0 },
        { json_TypeRoot,         json_TypeKey,          json_TypeArray,        0 },
        { json_TypeRoot,         json_TypeKey,          json_TypeKey,          0 },
        { json_TypeRoot,         json_TypeKey,          json_TypeValueString,  0 },
        { json_TypeRoot,         json_TypeKey,          json_TypeValueLiteral, 0 },
        { json_TypeRoot,         json_TypeValueString,  json_TypeRoot,         0 },
        { json_TypeRoot,         json_TypeValueString,  json_TypeObject,       0 },
        { json_TypeRoot,         json_TypeValueString,  json_TypeArray,        0 },
        { json_TypeRoot,         json_TypeValueString,  json_TypeKey,          0 },
        { json_TypeRoot,         json_TypeValueString,  json_TypeValueString,  0 },
        { json_TypeRoot,         json_TypeValueString,  json_TypeValueLiteral, 0 },
        { json_TypeRoot,         json_TypeValueLiteral, json_TypeRoot,         0 },
        { json_TypeRoot,         json_TypeValueLiteral, json_TypeObject,       0 },
        { json_TypeRoot,         json_TypeValueLiteral, json_TypeArray,        0 },
        { json_TypeRoot,         json_TypeValueLiteral, json_TypeKey,          0 },
        { json_TypeRoot,         json_TypeValueLiteral, json_TypeValueString,  0 },
        { json_TypeRoot,         json_TypeValueLiteral, json_TypeValueLiteral, 0 },
        { json_TypeObject,       json_TypeKey,          json_TypeRoot,         0 },
        { json_TypeObject,       json_TypeKey,          json_TypeObject,       0 },
        { json_TypeObject,       json_TypeKey,          json_TypeArray,        0 },
        { json_TypeObject,       json_TypeKey,          json_TypeKey,          1 },
        { json_TypeObject,       json_TypeKey,          json_TypeValueString,  0 },
        { json_TypeObject,       json_TypeKey,          json_TypeValueLiteral, 0 },
        { json_TypeArray,        json_TypeObject,       json_TypeRoot,         0 },
        { json_TypeArray,        json_TypeObject,       json_TypeObject,       1 },
        { json_TypeArray,        json_TypeObject,       json_TypeArray,        1 },
        { json_TypeArray,        json_TypeObject,       json_TypeKey,          0 },
        { json_TypeArray,        json_TypeObject,       json_TypeValueString,  1 },
        { json_TypeArray,        json_TypeObject,       json_TypeValueLiteral, 1 },
        { json_TypeArray,        json_TypeArray,        json_TypeRoot,         0 },
        { json_TypeArray,        json_TypeArray,        json_TypeObject,       1 },
        { json_TypeArray,        json_TypeArray,        json_TypeArray,        1 },
        { json_TypeArray,        json_TypeArray,        json_TypeKey,          0 },
        { json_TypeArray,        json_TypeArray,        json_TypeValueString,  1 },
        { json_TypeArray,        json_TypeArray,        json_TypeValueLiteral, 1 },
        { json_TypeArray,        json_TypeValueString,  json_TypeRoot,         0 },
        { json_TypeArray,        json_TypeValueString,  json_TypeObject,       1 },
        { json_TypeArray,        json_TypeValueString,  json_TypeArray,        1 },
        { json_TypeArray,        json_TypeValueString,  json_TypeKey,          0 },
        { json_TypeArray,        json_TypeValueString,  json_TypeValueString,  1 },
        { json_TypeArray,        json_TypeValueString,  json_TypeValueLiteral, 1 },
        { json_TypeArray,        json_TypeValueLiteral, json_TypeRoot,         0 },
        { json_TypeArray,        json_TypeValueLiteral, json_TypeObject,       1 },
        { json_TypeArray,        json_TypeValueLiteral, json_TypeArray,        1 },
        { json_TypeArray,        json_TypeValueLiteral, json_TypeKey,          0 },
        { json_TypeArray,        json_TypeValueLiteral, json_TypeValueString,  1 },
        { json_TypeArray,        json_TypeValueLiteral, json_TypeValueLiteral, 1 },
        { json_TypeKey,          json_TypeObject,       json_TypeRoot,         0 },
        { json_TypeKey,          json_TypeObject,       json_TypeObject,       0 },
        { json_TypeKey,          json_TypeObject,       json_TypeArray,        0 },
        { json_TypeKey,          json_TypeObject,       json_TypeKey,          0 },
        { json_TypeKey,          json_TypeObject,       json_TypeValueString,  0 },
        { json_TypeKey,          json_TypeObject,       json_TypeValueLiteral, 0 },
        { json_TypeKey,          json_TypeArray,        json_TypeRoot,         0 },
        { json_TypeKey,          json_TypeArray,        json_TypeObject,       0 },
        { json_TypeKey,          json_TypeArray,        json_TypeArray,        0 },
        { json_TypeKey,          json_TypeArray,        json_TypeKey,          0 },
        { json_TypeKey,          json_TypeArray,        json_TypeValueString,  0 },
        { json_TypeKey,          json_TypeArray,        json_TypeValueLiteral, 0 },
        { json_TypeKey,          json_TypeValueString,  json_TypeRoot,         0 },
        { json_TypeKey,          json_TypeValueString,  json_TypeObject,       0 },
        { json_TypeKey,          json_TypeValueString,  json_TypeArray,        0 },
        { json_TypeKey,          json_TypeValueString,  json_TypeKey,          0 },
        { json_TypeKey,          json_TypeValueString,  json_TypeValueString,  0 },
        { json_TypeKey,          json_TypeValueString,  json_TypeValueLiteral, 0 },
        { json_TypeKey,          json_TypeValueLiteral, json_TypeRoot,         0 },
        { json_TypeKey,          json_TypeValueLiteral, json_TypeObject,       0 },
        { json_TypeKey,          json_TypeValueLiteral, json_TypeArray,        0 },
        { json_TypeKey,          json_TypeValueLiteral, json_TypeKey,          0 },
        { json_TypeKey,          json_TypeValueLiteral, json_TypeValueString,  0 },
        { json_TypeKey,          json_TypeValueLiteral, json_TypeValueLiteral, 0 },
    };
    tJsonElement Root;
    tJsonElement *Parent;
    tJsonElement *Child;
    tJsonElement *Sibling;
    size_t n;
    int ok;

    JsonElementSetUp(&Root);

    for (ok = 1, n = 0; ok && (n < sizeof(TestAllocations) / sizeof(TestAllocations[0])); n++)
    {
        if (TestAllocations[n].ParentType == json_TypeRoot)
        {
            Parent = &Root;
        }
        else
        {
            ok = JsonElementAllocateChild(&Root, TestAllocations[n].ParentType);

            Parent = JsonElementGetChild(&Root);
        }

        ok = ok && (Parent != NULL);

        ok = ok && JsonElementAllocateChild(Parent, TestAllocations[n].ChildType);

        Child = (Parent != NULL) ? JsonElementGetChild(Parent) : NULL;

        ok = ok && (Child != NULL);

        ok = ok && !(!JsonElementAllocateNext(Child, TestAllocations[n].SiblingType) ^ !TestAllocations[n].Valid);

        Sibling = JsonElementGetNext(Child);

        if (Sibling != NULL)
        {
            ok = ok && (JsonElementGetType(Sibling) == TestAllocations[n].SiblingType);

            ok = ok && (JsonElementGetChild(Sibling) == NULL);

            ok = ok && (JsonElementGetNext(Sibling) == NULL);
        }
    }

    JsonElementCleanUp(&Root);

    return ok;
}


static int TestJsonElementFind(void)
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
    int ok;

    JsonElementSetUp(&Root);

    for (ok = 1, n = 0; ok && (n < sizeof(CreatePaths) / sizeof(CreatePaths[0])); n++)
    {
        Element = JsonElementFind(&Root, CreatePaths[n].Path, 1);

        ok = (Element != NULL);

        ok = ok && (JsonElementGetType(Element) == CreatePaths[n].ElementType);
    }

    for (n = 0; ok && (n < sizeof(InvalidPaths) / sizeof(InvalidPaths[0])); n++)
    {
        Element = JsonElementFind(&Root, InvalidPaths[n], 0);

        ok = (Element == NULL);
    }

    for (n = 0; ok && (n < sizeof(CreatePaths) / sizeof(CreatePaths[0])); n++)
    {
        Element = JsonElementFind(&Root, CreatePaths[n].Path, 0);

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
    { "JsonElementFind",          TestJsonElementFind          },
};


const tTest TestJsonElement =
{
    "JsonElement",
    TestCaseJsonElement,
    sizeof(TestCaseJsonElement) / sizeof(TestCaseJsonElement[0])
};
