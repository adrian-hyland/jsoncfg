#include "json.h"
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
		{ json_TypeRoot,         json_TypeKey,          false },
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
			Parent = &Root;
		}
		else
		{
			if (TestAllocations[n].ParentType == json_TypeKey)
			{
				TEST_IS_TRUE(JsonElementAllocateChild(&Root, json_TypeObject), TestResult);

				Parent = JsonElementGetChild(&Root, false);
			}
			else
			{
				Parent = &Root;
			}

			TEST_IS_TRUE(JsonElementAllocateChild(Parent, TestAllocations[n].ParentType), TestResult);

			Parent = JsonElementGetChild(Parent, false);

			TEST_IS_NOT_EQ(Parent, NULL, TestResult);

			TEST_IS_EQ(JsonElementGetType(Parent), TestAllocations[n].ParentType, TestResult);

			TEST_IS_EQ(JsonElementGetChild(Parent, false), NULL, TestResult);

			TEST_IS_EQ(JsonElementGetNext(Parent, false), NULL, TestResult);
		}

		TEST_IS_EQ(JsonElementAllocateChild(Parent, TestAllocations[n].ChildType), TestAllocations[n].Valid, TestResult);

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
		{ json_TypeRoot,         json_TypeComment,      json_TypeKey,          false },
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
			if (TestAllocations[n].ParentType == json_TypeKey)
			{
				TEST_IS_TRUE(JsonElementAllocateChild(&Root, json_TypeObject), TestResult);

				Parent = JsonElementGetChild(&Root, false);
			}
			else
			{
				Parent = &Root;
			}

			TEST_IS_TRUE(JsonElementAllocateChild(Parent, TestAllocations[n].ParentType), TestResult);

			Parent = JsonElementGetChild(Parent, false);
		}

		TEST_IS_NOT_EQ(Parent, NULL, TestResult);

		TEST_IS_TRUE(JsonElementAllocateChild(Parent, TestAllocations[n].ChildType), TestResult);

		Child = (Parent != NULL) ? JsonElementGetChild(Parent, false) : NULL;

		TEST_IS_NOT_EQ(Child, NULL, TestResult);

		TEST_IS_EQ(JsonElementAllocateNext(Child, TestAllocations[n].SiblingType), TestAllocations[n].Valid, TestResult);

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


static tTestResult TestJsonElementMoveChild(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonElement FromRoot;
	tJsonElement ToRoot;
	tJsonElement *FromElement;
	tJsonElement *ToElement;
	tJsonElement *KeyElement;
	tJsonElement *ChildElement;
	
	JsonElementSetUp(&FromRoot);
	JsonElementSetUp(&ToRoot);

	TEST_IS_EQ(JsonElementMoveChild(NULL, NULL), NULL, TestResult);

	JsonReadStringAscii(&FromRoot, false, "123");
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(&ToRoot, &FromRoot), FromElement, TestResult);
	TEST_IS_EQ(JsonElementGetChild(&FromRoot, false), NULL, TestResult);

	JsonReadStringAscii(&FromRoot, false, "123");
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(&ToRoot, FromElement), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(FromElement, false), NULL, TestResult);

	JsonReadStringAscii(&FromRoot, false, "\"abc\"");
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(&ToRoot, &FromRoot), FromElement, TestResult);
	TEST_IS_EQ(JsonElementGetChild(&FromRoot, false), NULL, TestResult);

	JsonReadStringAscii(&FromRoot, false, "\"abc\"");
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(&ToRoot, FromElement), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(FromElement, false), NULL, TestResult);

	JsonReadStringAscii(&FromRoot, false, "{ \"key\": \"value\" }");
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(&ToRoot, &FromRoot), FromElement, TestResult);
	TEST_IS_EQ(JsonElementGetChild(&FromRoot, false), NULL, TestResult);

	JsonReadStringAscii(&FromRoot, false, "{ \"key\": \"value\" }");
	FromElement = JsonElementGetChild(&FromRoot, false);
	ChildElement = JsonElementGetChild(FromElement, false);
	TEST_IS_NOT_EQ(ChildElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(&ToRoot, FromElement), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(FromElement, false), ChildElement, TestResult);

	JsonReadStringAscii(&FromRoot, false, "[ 1, 2, 3 ]");
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(&ToRoot, &FromRoot), FromElement, TestResult);
	TEST_IS_EQ(JsonElementGetChild(&FromRoot, false), NULL, TestResult);

	JsonReadStringAscii(&FromRoot, false, "[ 1, 2, 3 ]");
	FromElement = JsonElementGetChild(&FromRoot, false);
	ChildElement = JsonElementGetChild(FromElement, false);
	TEST_IS_NOT_EQ(ChildElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(&ToRoot, FromElement), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(FromElement, false), ChildElement, TestResult);


	JsonReadStringAscii(&ToRoot, false, "123");
	JsonReadStringAscii(&FromRoot, false, "123");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, &FromRoot), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(&FromRoot, false), FromElement, TestResult);

	JsonReadStringAscii(&ToRoot, false, "123");
	JsonReadStringAscii(&FromRoot, false, "123");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, FromElement), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(FromElement, false), NULL, TestResult);

	JsonReadStringAscii(&ToRoot, false, "123");
	JsonReadStringAscii(&FromRoot, false, "\"abc\"");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, &FromRoot), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(&FromRoot, false), FromElement, TestResult);

	JsonReadStringAscii(&ToRoot, false, "123");
	JsonReadStringAscii(&FromRoot, false, "\"abc\"");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, FromElement), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(FromElement, false), NULL, TestResult);

	JsonReadStringAscii(&ToRoot, false, "123");
	JsonReadStringAscii(&FromRoot, false, "{ \"key\": \"value\" }");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, &FromRoot), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(&FromRoot, false), FromElement, TestResult);

	JsonReadStringAscii(&ToRoot, false, "123");
	JsonReadStringAscii(&FromRoot, false, "{ \"key\": \"value\" }");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	ChildElement = JsonElementGetChild(FromElement, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_NOT_EQ(ChildElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, FromElement), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(FromElement, false), ChildElement, TestResult);

	JsonReadStringAscii(&ToRoot, false, "123");
	JsonReadStringAscii(&FromRoot, false, "[ 1, 2, 3 ]");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, &FromRoot), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(&FromRoot, false), FromElement, TestResult);

	JsonReadStringAscii(&ToRoot, false, "123");
	JsonReadStringAscii(&FromRoot, false, "[ 1, 2, 3 ]");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	ChildElement = JsonElementGetChild(FromElement, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_NOT_EQ(ChildElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, FromElement), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(FromElement, false), ChildElement, TestResult);


	JsonReadStringAscii(&ToRoot, false, "\"abc\"");
	JsonReadStringAscii(&FromRoot, false, "123");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, &FromRoot), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(&FromRoot, false), FromElement, TestResult);

	JsonReadStringAscii(&ToRoot, false, "\"abc\"");
	JsonReadStringAscii(&FromRoot, false, "123");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, FromElement), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(FromElement, false), NULL, TestResult);

	JsonReadStringAscii(&ToRoot, false, "\"abc\"");
	JsonReadStringAscii(&FromRoot, false, "\"abc\"");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, &FromRoot), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(&FromRoot, false), FromElement, TestResult);

	JsonReadStringAscii(&ToRoot, false, "\"abc\"");
	JsonReadStringAscii(&FromRoot, false, "\"abc\"");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, FromElement), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(FromElement, false), NULL, TestResult);

	JsonReadStringAscii(&ToRoot, false, "\"abc\"");
	JsonReadStringAscii(&FromRoot, false, "{ \"key\": \"value\" }");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, &FromRoot), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(&FromRoot, false), FromElement, TestResult);

	JsonReadStringAscii(&ToRoot, false, "\"abc\"");
	JsonReadStringAscii(&FromRoot, false, "{ \"key\": \"value\" }");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	ChildElement = JsonElementGetChild(FromElement, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_NOT_EQ(ChildElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, FromElement), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(FromElement, false), ChildElement, TestResult);

	JsonReadStringAscii(&ToRoot, false, "\"abc\"");
	JsonReadStringAscii(&FromRoot, false, "[ 1, 2, 3 ]");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, &FromRoot), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(&FromRoot, false), FromElement, TestResult);

	JsonReadStringAscii(&ToRoot, false, "\"abc\"");
	JsonReadStringAscii(&FromRoot, false, "[ 1, 2, 3 ]");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	ChildElement = JsonElementGetChild(FromElement, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_NOT_EQ(ChildElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, FromElement), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(FromElement, false), ChildElement, TestResult);


	JsonReadStringAscii(&ToRoot, false, "{ \"key\": \"value\" }");
	JsonReadStringAscii(&FromRoot, false, "123");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, &FromRoot), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(&FromRoot, false), FromElement, TestResult);

	JsonReadStringAscii(&ToRoot, false, "{ \"key\": \"value\" }");
	JsonReadStringAscii(&FromRoot, false, "123");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, FromElement), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(FromElement, false), NULL, TestResult);

	JsonReadStringAscii(&ToRoot, false, "{ \"key\": \"value\" }");
	JsonReadStringAscii(&FromRoot, false, "\"abc\"");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, &FromRoot), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(&FromRoot, false), FromElement, TestResult);

	JsonReadStringAscii(&ToRoot, false, "{ \"key\": \"value\" }");
	JsonReadStringAscii(&FromRoot, false, "\"abc\"");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, FromElement), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(FromElement, false), NULL, TestResult);

	JsonReadStringAscii(&ToRoot, false, "{ \"key\": \"value\" }");
	JsonReadStringAscii(&FromRoot, false, "{ \"next-key\": \"next-value\" }");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, &FromRoot), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(&FromRoot, false), FromElement, TestResult);

	JsonReadStringAscii(&ToRoot, false, "{ \"key\": \"value\" }");
	JsonReadStringAscii(&FromRoot, false, "{ \"next-key\": \"next-value\" }");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	ChildElement = JsonElementGetChild(FromElement, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_NOT_EQ(ChildElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, FromElement), ChildElement, TestResult);
	TEST_IS_EQ(JsonElementGetChild(FromElement, false), NULL, TestResult);

	JsonReadStringAscii(&ToRoot, false, "{ \"key\": \"value\" }");
	JsonReadStringAscii(&FromRoot, false, "[ 1, 2, 3 ]");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, &FromRoot), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(&FromRoot, false), FromElement, TestResult);

	JsonReadStringAscii(&ToRoot, false, "{ \"key\": \"value\" }");
	JsonReadStringAscii(&FromRoot, false, "[ 1, 2, 3 ]");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	ChildElement = JsonElementGetChild(FromElement, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_NOT_EQ(ChildElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, FromElement), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(FromElement, false), ChildElement, TestResult);


	JsonReadStringAscii(&ToRoot, false, "{ \"key\": \"value\" }");
	JsonReadStringAscii(&FromRoot, false, "123");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	KeyElement = JsonElementGetChild(ToElement, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_NOT_EQ(KeyElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(KeyElement, &FromRoot), FromElement, TestResult);
	TEST_IS_EQ(JsonElementGetChild(&FromRoot, false), NULL, TestResult);

	JsonReadStringAscii(&ToRoot, false, "{ \"key\": \"value\" }");
	JsonReadStringAscii(&FromRoot, false, "123");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	KeyElement = JsonElementGetChild(ToElement, false);
	ChildElement = JsonElementGetChild(KeyElement, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_NOT_EQ(KeyElement, NULL, TestResult);
	TEST_IS_NOT_EQ(ChildElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(KeyElement, FromElement), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(KeyElement, false), ChildElement, TestResult);

	JsonReadStringAscii(&ToRoot, false, "{ \"key\": \"value\" }");
	JsonReadStringAscii(&FromRoot, false, "\"abc\"");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	KeyElement = JsonElementGetChild(ToElement, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_NOT_EQ(KeyElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(KeyElement, &FromRoot), FromElement, TestResult);
	TEST_IS_EQ(JsonElementGetChild(&FromRoot, false), NULL, TestResult);

	JsonReadStringAscii(&ToRoot, false, "{ \"key\": \"value\" }");
	JsonReadStringAscii(&FromRoot, false, "\"abc\"");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	KeyElement = JsonElementGetChild(ToElement, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_NOT_EQ(KeyElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(KeyElement, FromElement), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(FromElement, false), NULL, TestResult);

	JsonReadStringAscii(&ToRoot, false, "{ \"key\": \"value\" }");
	JsonReadStringAscii(&FromRoot, false, "{ \"next-key\": \"next-value\" }");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	KeyElement = JsonElementGetChild(ToElement, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_NOT_EQ(KeyElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(KeyElement, &FromRoot), FromElement, TestResult);
	TEST_IS_EQ(JsonElementGetChild(&FromRoot, false), NULL, TestResult);

	JsonReadStringAscii(&ToRoot, false, "{ \"key\": \"value\" }");
	JsonReadStringAscii(&FromRoot, false, "{ \"next-key\": \"next-value\" }");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	KeyElement = JsonElementGetChild(ToElement, false);
	ChildElement = JsonElementGetChild(FromElement, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_NOT_EQ(ChildElement, NULL, TestResult);
	TEST_IS_NOT_EQ(KeyElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(KeyElement, FromElement), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(FromElement, false), ChildElement, TestResult);

	JsonReadStringAscii(&ToRoot, false, "{ \"key\": \"value\" }");
	JsonReadStringAscii(&FromRoot, false, "[ 1, 2, 3 ]");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	KeyElement = JsonElementGetChild(ToElement, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_NOT_EQ(KeyElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(KeyElement, &FromRoot), FromElement, TestResult);
	TEST_IS_EQ(JsonElementGetChild(&FromRoot, false), NULL, TestResult);

	JsonReadStringAscii(&ToRoot, false, "{ \"key\": \"value\" }");
	JsonReadStringAscii(&FromRoot, false, "[ 1, 2, 3 ]");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	KeyElement = JsonElementGetChild(ToElement, false);
	ChildElement = JsonElementGetChild(FromElement, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_NOT_EQ(KeyElement, NULL, TestResult);
	TEST_IS_NOT_EQ(ChildElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(KeyElement, FromElement), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(FromElement, false), ChildElement, TestResult);


	JsonReadStringAscii(&ToRoot, false, "[ 1, 2, 3 ]");
	JsonReadStringAscii(&FromRoot, false, "123");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, &FromRoot), FromElement, TestResult);
	TEST_IS_EQ(JsonElementGetChild(&FromRoot, false), NULL, TestResult);

	JsonReadStringAscii(&ToRoot, false, "[ 1, 2, 3 ]");
	JsonReadStringAscii(&FromRoot, false, "123");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, FromElement), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(FromElement, false), NULL, TestResult);

	JsonReadStringAscii(&ToRoot, false, "[ 1, 2, 3 ]");
	JsonReadStringAscii(&FromRoot, false, "\"abc\"");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, &FromRoot), FromElement, TestResult);
	TEST_IS_EQ(JsonElementGetChild(&FromRoot, false), NULL, TestResult);

	JsonReadStringAscii(&ToRoot, false, "[ 1, 2, 3 ]");
	JsonReadStringAscii(&FromRoot, false, "\"abc\"");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, FromElement), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(FromElement, false), NULL, TestResult);

	JsonReadStringAscii(&ToRoot, false, "[ 1, 2, 3 ]");
	JsonReadStringAscii(&FromRoot, false, "{ \"next-key\": \"next-value\" }");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, &FromRoot), FromElement, TestResult);
	TEST_IS_EQ(JsonElementGetChild(&FromRoot, false), NULL, TestResult);

	JsonReadStringAscii(&ToRoot, false, "[ 1, 2, 3 ]");
	JsonReadStringAscii(&FromRoot, false, "{ \"next-key\": \"next-value\" }");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	ChildElement = JsonElementGetChild(FromElement, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_NOT_EQ(ChildElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, FromElement), NULL, TestResult);
	TEST_IS_EQ(JsonElementGetChild(FromElement, false), ChildElement, TestResult);

	JsonReadStringAscii(&ToRoot, false, "[ 1, 2, 3 ]");
	JsonReadStringAscii(&FromRoot, false, "[ 1, 2, 3 ]");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, &FromRoot), FromElement, TestResult);
	TEST_IS_EQ(JsonElementGetChild(&FromRoot, false), NULL, TestResult);

	JsonReadStringAscii(&ToRoot, false, "[ 1, 2, 3 ]");
	JsonReadStringAscii(&FromRoot, false, "[ 1, 2, 3 ]");
	ToElement = JsonElementGetChild(&ToRoot, false);
	FromElement = JsonElementGetChild(&FromRoot, false);
	ChildElement = JsonElementGetChild(FromElement, false);
	TEST_IS_NOT_EQ(ToElement, NULL, TestResult);
	TEST_IS_NOT_EQ(FromElement, NULL, TestResult);
	TEST_IS_NOT_EQ(ChildElement, NULL, TestResult);
	TEST_IS_EQ(JsonElementMoveChild(ToElement, FromElement), ChildElement, TestResult);
	TEST_IS_EQ(JsonElementGetChild(FromElement, false), NULL, TestResult);

	JsonElementCleanUp(&ToRoot);
	JsonElementCleanUp(&FromRoot);

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
	{ "JsonElementMoveChild",     TestJsonElementMoveChild     }
};


const tTest TestJsonElement =
{
	"JsonElement",
	TestCaseJsonElement,
	sizeof(TestCaseJsonElement) / sizeof(TestCaseJsonElement[0])
};
