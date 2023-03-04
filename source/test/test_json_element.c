#include "json_element.h"
#include "test_json.h"


static tTestResult TestJsonElementSetUp(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonElement Element;

	JsonElementSetUp(&Element);

	TEST_IS_EQ(JsonElementGetType(&Element), json_TypeRoot, TestResult);

	TEST_IS_EQ(JsonElementGetChild(&Element, false), NULL, TestResult);

	TEST_IS_EQ(JsonElementGetNext(&Element, false), NULL, TestResult);

	JsonElementCleanUp(&Element);

	return TestResult;
}


static tTestResult TestJsonElementCleanUp(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonElement Element;

	JsonElementSetUp(&Element);

	TEST_IS_NOT_EQ(JsonElementFind(&Element, JsonPathAscii("/"), true), NULL, TestResult);

	TEST_IS_EQ(JsonElementGetType(&Element), json_TypeRoot, TestResult);

	TEST_IS_NOT_EQ(JsonElementGetChild(&Element, false), NULL, TestResult);

	TEST_IS_EQ(JsonElementGetNext(&Element, false), NULL, TestResult);

	JsonElementCleanUp(&Element);

	TEST_IS_EQ(JsonElementGetType(&Element), json_TypeRoot, TestResult);

	TEST_IS_EQ(JsonElementGetChild(&Element, false), NULL, TestResult);

	TEST_IS_EQ(JsonElementGetNext(&Element, false), NULL, TestResult);

	return TestResult;
}


static tTestResult TestJsonElementAllocateChild(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
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

	JsonElementSetUp(&Root);

	for (n = 0; n < sizeof(TestAllocations) / sizeof(TestAllocations[0]); n++)
	{
		if (TestAllocations[n].ParentType == json_TypeRoot)
		{
			TEST_IS_EQ(JsonElementAllocateChild(&Root, TestAllocations[n].ChildType), TestAllocations[n].Valid, TestResult);

			Parent = &Root;
		}
		else
		{
			TEST_IS_TRUE(JsonElementAllocateChild(&Root, TestAllocations[n].ParentType), TestResult);

			Parent = JsonElementGetChild(&Root, false);

			TEST_IS_NOT_EQ(Parent, NULL, TestResult);

			TEST_IS_EQ(JsonElementGetType(Parent), TestAllocations[n].ParentType, TestResult);

			TEST_IS_EQ(JsonElementGetChild(Parent, false), NULL, TestResult);

			TEST_IS_EQ(JsonElementGetNext(Parent, false), NULL, TestResult);

			if (TestAllocations[n].Valid)
			{
				TEST_IS_TRUE(JsonElementAllocateChild(Parent, TestAllocations[n].ChildType), TestResult);
			}
			else
			{
				TEST_IS_FALSE(JsonElementAllocateChild(Parent, TestAllocations[n].ChildType), TestResult);
			}
		}

		Child = JsonElementGetChild(Parent, false);

		if (Child != NULL)
		{
			TEST_IS_EQ(JsonElementGetType(Child), TestAllocations[n].ChildType, TestResult);

			TEST_IS_EQ(JsonElementGetChild(Child, false), NULL, TestResult);

			TEST_IS_EQ(JsonElementGetNext(Child, false), NULL, TestResult);
		}
	}

	JsonElementCleanUp(&Root);

	return TestResult;
}


static tTestResult TestJsonElementAllocateNext(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
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

	JsonElementSetUp(&Root);

	for (n = 0; n < sizeof(TestAllocations) / sizeof(TestAllocations[0]); n++)
	{
		if (TestAllocations[n].ParentType == json_TypeRoot)
		{
			Parent = &Root;
		}
		else
		{
			TEST_IS_TRUE(JsonElementAllocateChild(&Root, TestAllocations[n].ParentType), TestResult);

			Parent = JsonElementGetChild(&Root, false);
		}

		TEST_IS_NOT_EQ(Parent, NULL, TestResult);

		TEST_IS_TRUE(JsonElementAllocateChild(Parent, TestAllocations[n].ChildType), TestResult);

		Child = (Parent != NULL) ? JsonElementGetChild(Parent, false) : NULL;

		TEST_IS_NOT_EQ(Child, NULL, TestResult);

		if (TestAllocations[n].Valid)
		{
			TEST_IS_TRUE(JsonElementAllocateNext(Child, TestAllocations[n].SiblingType), TestResult);
		}
		else
		{
			TEST_IS_FALSE(JsonElementAllocateNext(Child, TestAllocations[n].SiblingType), TestResult);
		}

		Sibling = JsonElementGetNext(Child, false);

		if (Sibling != NULL)
		{
			TEST_IS_EQ(JsonElementGetType(Sibling), TestAllocations[n].SiblingType, TestResult);

			TEST_IS_EQ(JsonElementGetChild(Sibling, false), NULL, TestResult);

			TEST_IS_EQ(JsonElementGetNext(Sibling, false), NULL, TestResult);
		}
	}

	JsonElementCleanUp(&Root);

	return TestResult;
}


static tTestResult TestJsonElementGetChild(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonElement Root;
	tJsonElement *Element;

	JsonElementSetUp(&Root);

	TEST_IS_EQ(JsonElementGetChild(&Root, false), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(&Root, true), NULL, TestResult);

	TEST_IS_TRUE(JsonElementAllocateChild(&Root, json_TypeComment), TestResult);

	Element = JsonElementGetChild(&Root, false);

	TEST_IS_NOT_EQ(Element, NULL, TestResult);
	TEST_IS_EQ(Element->Type, json_TypeComment, TestResult);

	TEST_IS_EQ(JsonElementGetChild(&Root, true), NULL, TestResult);

	TEST_IS_TRUE(JsonElementAllocateNext(Element, json_TypeObject), TestResult);

	Element = JsonElementGetChild(&Root, false);

	TEST_IS_NOT_EQ(Element, NULL, TestResult);
	TEST_IS_EQ(Element->Type, json_TypeComment, TestResult);

	Element = JsonElementGetChild(&Root, true);

	TEST_IS_NOT_EQ(Element, NULL, TestResult);
	TEST_IS_EQ(Element->Type, json_TypeObject, TestResult);

	JsonElementCleanUp(&Root);

	return TestResult;
}


static tTestResult TestJsonElementGetNext(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonElement Root;
	tJsonElement *ChildElement;
	tJsonElement *NextElement;

	JsonElementSetUp(&Root);

	TEST_IS_TRUE(JsonElementAllocateChild(&Root, json_TypeObject), TestResult);

	ChildElement = JsonElementGetChild(&Root, false);

	TEST_IS_NOT_EQ(ChildElement, NULL, TestResult);

	TEST_IS_TRUE(JsonElementAllocateChild(ChildElement, json_TypeKey), TestResult);

	ChildElement = JsonElementGetChild(ChildElement, false);

	TEST_IS_EQ(JsonElementGetNext(ChildElement, false), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetNext(ChildElement, true), NULL, TestResult);

	TEST_IS_TRUE(JsonElementAllocateNext(ChildElement, json_TypeComment), TestResult);

	NextElement = JsonElementGetNext(ChildElement, false);

	TEST_IS_NOT_EQ(NextElement, NULL, TestResult);
	TEST_IS_EQ(NextElement->Type, json_TypeComment, TestResult);

	TEST_IS_EQ(JsonElementGetNext(ChildElement, true), NULL, TestResult);

	TEST_IS_TRUE(JsonElementAllocateNext(NextElement, json_TypeKey), TestResult);

	NextElement = JsonElementGetNext(ChildElement, false);

	TEST_IS_NOT_EQ(NextElement, NULL, TestResult);
	TEST_IS_EQ(NextElement->Type, json_TypeComment, TestResult);

	NextElement = JsonElementGetNext(ChildElement, true);

	TEST_IS_NOT_EQ(NextElement, NULL, TestResult);
	TEST_IS_EQ(NextElement->Type, json_TypeKey, TestResult);

	JsonElementCleanUp(&Root);

	return TestResult;
}


static tTestResult TestJsonElementFind(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	static struct
	{
		const char *Path;
		tJsonType      ElementType;
	} CreatePaths[] =
	{
		{ "/",                                               json_TypeObject       },
		{ "/key1:123",                                       json_TypeValueLiteral },
		{ "/key2:\"hello world\"",                           json_TypeValueString  },
		{ "/key3[/object:1]",                                json_TypeObject       },
		{ "/key3[/object:1]/key31:\"hello again\"",          json_TypeValueString  },
		{ "/key3[/object:2]",                                json_TypeObject       },
		{ "/key3[/object:2]/key32[/object:321]",             json_TypeObject       },
		{ "/key3[/object:2]/key32[/object:322]",             json_TypeObject       },
		{ "/key3[/object:2]/key32[/object:322]/key322:true", json_TypeValueLiteral }
	};
	const char *InvalidPaths[] =
	{
		"/key1:12",
		"/key4",
		"/key3[object:3]",
		"/key3[object:1]/key32",
		"/key3[object:2]/key31",
		"/key3[object:2]/key32[/object:323]",
		"/key3[object:2]/key32[/object:322]/key321",
	};
	tJsonElement Root;
	tJsonElement *Element;
	size_t n;

	JsonElementSetUp(&Root);

	for (n = 0; n < sizeof(CreatePaths) / sizeof(CreatePaths[0]); n++)
	{
		Element = JsonElementFind(&Root, JsonPathAscii(CreatePaths[n].Path), true);

		TEST_IS_NOT_EQ(Element, NULL, TestResult);

		TEST_IS_EQ(JsonElementGetType(Element), CreatePaths[n].ElementType, TestResult);
	}

	for (n = 0; n < sizeof(InvalidPaths) / sizeof(InvalidPaths[0]); n++)
	{
		Element = JsonElementFind(&Root, JsonPathAscii(InvalidPaths[n]), false);

		TEST_IS_EQ(Element, NULL, TestResult);
	}

	for (n = 0; n < sizeof(CreatePaths) / sizeof(CreatePaths[0]); n++)
	{
		Element = JsonElementFind(&Root, JsonPathAscii(CreatePaths[n].Path), false);

		TEST_IS_NOT_EQ(Element, NULL, TestResult);

		TEST_IS_EQ(JsonElementGetType(Element), CreatePaths[n].ElementType, TestResult);
	}

	JsonElementCleanUp(&Root);

	return TestResult;
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
