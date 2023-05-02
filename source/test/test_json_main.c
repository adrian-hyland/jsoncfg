#include <string.h>
#include "json.h"
#include "test_json.h"


static tTestResult TestJsonReadStringAscii(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonElement Root;

	JsonElementSetUp(&Root);

	TEST_IS_FALSE(JsonReadStringAscii(&Root, false, NULL), TestResult);

	TEST_IS_TRUE(JsonReadStringAscii(&Root, false, ""), TestResult);
	TEST_IS_NULL(JsonElementGetChild(&Root, false), TestResult);

	TEST_IS_TRUE(JsonReadStringAscii(&Root, false, "{}"), TestResult);
	TEST_IS_NOT_NULL(JsonElementGetChild(&Root, false), TestResult);

	TEST_IS_TRUE(JsonReadStringAscii(&Root, false, ""), TestResult);
	TEST_IS_NULL(JsonElementGetChild(&Root, false), TestResult);

	JsonElementCleanUp(&Root);

	return TestResult;
}


static tTestResult TestJsonWriteFileContent(tTestResult TestResult, tJsonElement *Root, tJsonUtfType UtfType, bool RequireBom)
{
	tJsonFormat Format;
	tJsonCharacter Character;
	FILE *File;
	tJsonUtf Utf;
	int Error;
	size_t Length;
	size_t n;

	File = fopen("test.json", "w+");

	TEST_IS_NOT_NULL(File, TestResult);

	TEST_IS_TRUE(JsonWriteFile(Root, UtfType, RequireBom, 3, json_CommentNone, File, 1), TestResult);

	JsonFormatSetUpIndent(&Format, 3, json_CommentNone, Root);

	TEST_IS_ZERO(fseek(File, 0, SEEK_SET), TestResult);

	if (RequireBom)
	{
		Length = JsonUtfEncode(UtfType, Utf, sizeof(Utf), 0, 0xFEFF);
		TEST_IS_NOT_ZERO(Length, TestResult);
		for (n = 0; n < Length; n++)
		{
			TEST_IS_EQ(fgetc(File), Utf[n], TestResult);
		}
	}

	for (;;)
	{
		Error = JsonFormatCharacter(&Format, &Character);
		if (Error != JSON_FORMAT_INCOMPLETE)
		{
			break;
		}
		Length = JsonUtfEncode(UtfType, Utf, sizeof(Utf), 0, Character);
		TEST_IS_NOT_ZERO(Length, TestResult);
		for (n = 0; n < Length; n++)
		{
			TEST_IS_EQ(fgetc(File), Utf[n], TestResult);
		}
	}

	TEST_IS_EQ(Error, JSON_FORMAT_COMPLETE, TestResult);

	JsonFormatCleanUp(&Format);

	if (File != NULL)
	{
		fclose(File);
	}

	return TestResult;
}


static tTestResult TestJsonWriteFile(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonElement Root;

	JsonElementSetUp(&Root);

	TEST_IS_NOT_NULL(JsonElementFind(&Root, JsonPathAscii("/object1/key1:\"value1\""), true), TestResult);
	TEST_IS_NOT_NULL(JsonElementFind(&Root, JsonPathAscii("/object1/object2/key2:\"value2\""), true), TestResult);
	TEST_IS_NOT_NULL(JsonElementFind(&Root, JsonPathAscii("/object1/array1[/key3:\"value3\"]"), true), TestResult);

	TestResult = TestJsonWriteFileContent(TestResult, &Root, json_Utf8, false);
	TestResult = TestJsonWriteFileContent(TestResult, &Root, json_Utf8, true);
	TestResult = TestJsonWriteFileContent(TestResult, &Root, json_Utf16be, false);
	TestResult = TestJsonWriteFileContent(TestResult, &Root, json_Utf16be, true);
	TestResult = TestJsonWriteFileContent(TestResult, &Root, json_Utf16le, false);
	TestResult = TestJsonWriteFileContent(TestResult, &Root, json_Utf16le, true);

	JsonElementCleanUp(&Root);

	return TestResult;
}


static bool StringCompare(tJsonString *Left, tJsonString *Right)
{
	return (Left->Length == Right->Length) && (memcmp(Left->Content, Right->Content, Left->Length) == 0);
}


static bool JsonElementCompare(tJsonElement *Left, tJsonElement *Right)
{
	if (Left == NULL)
	{
		return Right == NULL;
	}
	else if (Right == NULL)
	{
		return false;
	}
	else if (!StringCompare(&Left->Name, &Right->Name))
	{
		return false;
	}
	else
	{
		return JsonElementCompare(Left->Child, Right->Child) && JsonElementCompare(Left->Next, Right->Next);
	}
}


static tTestResult TestJsonReadFileContent(tTestResult TestResult, tJsonElement *Root, tJsonUtfType UtfType, bool RequireBom)
{
	tJsonElement ReadRoot;
	FILE *File;

	File = fopen("test.json", "w+");

	JsonElementSetUp(&ReadRoot);

	TEST_IS_NOT_NULL(File, TestResult);

	TEST_IS_TRUE(JsonWriteFile(Root, UtfType, RequireBom, 3, json_CommentNone, File, 1), TestResult);

	TEST_IS_ZERO(fseek(File, 0, SEEK_SET), TestResult);

	TEST_IS_TRUE(JsonReadFile(&ReadRoot, true, File, 1), TestResult);

	TEST_IS_TRUE(JsonElementCompare(Root, &ReadRoot), TestResult);

	JsonElementCleanUp(&ReadRoot);

	if (File != NULL)
	{
		fclose(File);
	}

	return TestResult;
}


static tTestResult TestJsonReadFile(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonElement Root;

	JsonElementSetUp(&Root);

	TEST_IS_NOT_NULL(JsonElementFind(&Root, JsonPathAscii("/object1/key1:\"value1\""), true), TestResult);
	TEST_IS_NOT_NULL(JsonElementFind(&Root, JsonPathAscii("/object1/object2/key2:\"value2\""), true), TestResult);
	TEST_IS_NOT_NULL(JsonElementFind(&Root, JsonPathAscii("/object1/array1[/key3:\"value3\"]"), true), TestResult);

	TestResult = TestJsonReadFileContent(TestResult, &Root, json_Utf8, false);
	TestResult = TestJsonReadFileContent(TestResult, &Root, json_Utf8, true);
	TestResult = TestJsonReadFileContent(TestResult, &Root, json_Utf16be, false);
	TestResult = TestJsonReadFileContent(TestResult, &Root, json_Utf16be, true);
	TestResult = TestJsonReadFileContent(TestResult, &Root, json_Utf16le, false);
	TestResult = TestJsonReadFileContent(TestResult, &Root, json_Utf16le, true);

	JsonElementCleanUp(&Root);

	return TestResult;
}


static const tTestCase TestCaseJsonMain[] =
{
	{ "JsonReadStringAscii", TestJsonReadStringAscii },
	{ "JsonWriteFile",       TestJsonWriteFile       },
	{ "JsonReadFile",        TestJsonReadFile        }
};


const tTest TestJsonMain =
{
	"JsonMain",
	TestCaseJsonMain,
	sizeof(TestCaseJsonMain) / sizeof(TestCaseJsonMain[0])
};
