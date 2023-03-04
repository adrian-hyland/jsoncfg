#include <string.h>
#include "json_character.h"
#include "json_parse.h"
#include "test_json.h"


static tTestResult TestJsonParseComplete(tTestResult TestResult, const tJsonUtf8Unit *Content, bool StripComments)
{
	tJsonElement Root;
	tJsonParse Parse;

	JsonElementSetUp(&Root);
	JsonParseSetUp(&Parse, StripComments, &Root);

	for (; *Content != '\0'; Content++)
	{
		TEST_IS_EQ(JsonParse(&Parse, *Content), JSON_PARSE_INCOMPLETE, TestResult);
	}

	TEST_IS_EQ(JsonParse(&Parse, *Content), JSON_PARSE_COMPLETE, TestResult);

	JsonParseCleanUp(&Parse);
	JsonElementCleanUp(&Root);

	return TestResult;
}


static tTestResult TestJsonParseIncomplete(tTestResult TestResult, const tJsonUtf8Unit *Content, bool StripComments)
{
	tJsonElement Root;
	tJsonParse Parse;
	size_t Length;
	size_t Index;

	Length = strlen((const char *)Content);

	while (Length > 1)
	{
		Length--;

		JsonElementSetUp(&Root);
		JsonParseSetUp(&Parse, StripComments, &Root);

		for (Index = 0; Index < Length; Index++)
		{
			TEST_IS_EQ(JsonParse(&Parse, Content[Index]), JSON_PARSE_INCOMPLETE, TestResult);
		}

		TEST_IS_EQ(JsonParse(&Parse, '\0'), JSON_PARSE_ERROR, TestResult);

		JsonParseCleanUp(&Parse);
		JsonElementCleanUp(&Root);
	}

	return TestResult;
}


static tTestResult TestJsonParseError(tTestResult TestResult, const tJsonUtf8Unit *Content)
{
	tJsonElement Root;
	tJsonParse Parse;

	JsonElementSetUp(&Root);
	JsonParseSetUp(&Parse, true, &Root);

	for (; (Content[0] != '\0') && (Content[1] != '\0'); Content++)
	{
		TEST_IS_EQ(JsonParse(&Parse, *Content), JSON_PARSE_INCOMPLETE, TestResult);
	}

	TEST_IS_EQ(JsonParse(&Parse, *Content), JSON_PARSE_ERROR, TestResult);

	JsonParseCleanUp(&Parse);
	JsonElementCleanUp(&Root);

	return TestResult;
}


static tTestResult TestJsonParseContent(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	static const tJsonUtf8Unit *ValidContent[] =
	{
		(const tJsonUtf8Unit *)"{ "
		                          "\"key-true\": true, "
		                          "\"key-false\": false, "
		                          "\"key-null\": null, "
		                          "\"key-int\": 123, "
		                          "\"key-real\": 1.23e4, "
		                          "\"key-real-nan\": NaN, "
		                          "\"key-string\": \"hello world\", "
		                          "\"key-string-escape\": \"\\t\\r\\n\\b\\f\\\\\\\"\", "
		                          "\"key-object-empty\": {}, "
		                          "\"key-object\": { \"key\": value }, "
		                          "\"key-array-empty\": [], "
		                          "\"key-array\": [ "
		                              "true, "
		                              "false, "
		                              "null, "
		                              "123, "
		                              "1.23e4, "
		                              "NaN, "
		                              "\"hello world\", "
		                              "\"\\t\\r\\n\\b\\f\\\\\\\"\", "
		                              "{}, "
		                              "{ \"key\": value }, "
		                              "[], "
		                              "[ 1, 2, 3 ]"
		                         " ]"
		                      " }",
		(const tJsonUtf8Unit *)"[ "
		                          "true, "
		                          "false, "
		                          "null, "
		                          "123, "
		                          "1.23e4, "
		                          "NaN, "
		                          "\"hello world\", "
		                          "\"\\t\\r\\n\\b\\f\\\\\\\"\", "
		                          "{}, "
		                          "{ \"key\": value }, "
		                          "[], "
		                          "[ 1, 2, 3 ]"
		                      " ]",
		(const tJsonUtf8Unit *)"\"string:{}[]\\t\\r\\n\\b\\f\\\\\\\"\"",
		(const tJsonUtf8Unit *)"true",
		(const tJsonUtf8Unit *)"false",
		(const tJsonUtf8Unit *)"null",
		(const tJsonUtf8Unit *)"1234567890",
		(const tJsonUtf8Unit *)"1.234567890e-99",
		(const tJsonUtf8Unit *)"// Comment 1\n"
		                       "// Comment 2\n"
		                       "{ // Comment 3\n"
		                       "  // Comment 4\n"
		                       "   \"key1\" // Comment 5\n"
		                       "            // Comment 6\n"
		                       "   : // Comment 7\n"
		                       "     // Comment 8\n"
		                       "   \"value1\" // Comment 9\n"
		                       "              // Comment 10\n"
		                       "   , // Comment 11\n"
		                       "     // Comment 12\n"
		                       "   \"key2\": [ // Comment 13\n"
		                       "               // Comment 14\n"
		                       "      \"value2\" // Comment 15\n"
		                       "                 // Comment 16\n"
		                       "      , // Comment 17\n"
		                       "        // Comment 18\n"
		                       "      \"value3\" // Comment 19\n"
		                       "                 // Comment 20\n"
		                       "   ] // Comment 21\n"
		                       "     // Comment 22\n"
		                       "   , // Comment 23\n"
		                       "     // Comment 24\n"
		                       "   \"key3\": { // Comment 25\n"
		                       "               // Comment 26\n"
		                       "      \"key4\": \"value4\", // Comment 27\n"
		                       "                            // Comment 28\n"
		                       "      \"key5\": [] // Comment 29\n"
		                       "                   // Comment 30\n"
		                       "   } // Comment 31\n"
		                       "     // Comment 32\n"
		                       "} // Comment 33\n"
		                       "// Comment 34\n"
	};
	static const tJsonUtf8Unit *InvalidContent[] =
	{
		(const tJsonUtf8Unit *)"{ ,",
		(const tJsonUtf8Unit *)"{ 1",
		(const tJsonUtf8Unit *)"{ {",
		(const tJsonUtf8Unit *)"{ [",
		(const tJsonUtf8Unit *)"{ :",
		(const tJsonUtf8Unit *)"{ \"abc\" ,",
		(const tJsonUtf8Unit *)"{ \"abc\" 1",
		(const tJsonUtf8Unit *)"{ \"abc\" {",
		(const tJsonUtf8Unit *)"{ \"abc\" [",
		(const tJsonUtf8Unit *)"{ \"abc\" \"",
		(const tJsonUtf8Unit *)"[ ,",
		(const tJsonUtf8Unit *)"[ :",
		(const tJsonUtf8Unit *)"[ \"abc\" {",
		(const tJsonUtf8Unit *)"[ \"abc\" [",
		(const tJsonUtf8Unit *)"[ \"abc\" :",
		(const tJsonUtf8Unit *)"[ \"abc\" 1",
		(const tJsonUtf8Unit *)"[ \"abc\" \"",
		(const tJsonUtf8Unit *)"[ 123 {",
		(const tJsonUtf8Unit *)"[ 123 [",
		(const tJsonUtf8Unit *)"[ 123 :",
		(const tJsonUtf8Unit *)"[ 123 1",
		(const tJsonUtf8Unit *)"[ 123 \"",
		(const tJsonUtf8Unit *)"[ {} {",
		(const tJsonUtf8Unit *)"[ {} [",
		(const tJsonUtf8Unit *)"[ {} :",
		(const tJsonUtf8Unit *)"[ {} 1",
		(const tJsonUtf8Unit *)"[ {} \"",
		(const tJsonUtf8Unit *)"[ [] {",
		(const tJsonUtf8Unit *)"[ [] [",
		(const tJsonUtf8Unit *)"[ [] :",
		(const tJsonUtf8Unit *)"[ [] 1",
		(const tJsonUtf8Unit *)"[ [] \"",
		(const tJsonUtf8Unit *)"\"abc\" 1",
		(const tJsonUtf8Unit *)"\"abc\" {",
		(const tJsonUtf8Unit *)"\"abc\" [",
		(const tJsonUtf8Unit *)"\"abc\" :",
		(const tJsonUtf8Unit *)"\"abc\" \"",
		(const tJsonUtf8Unit *)"\"abc\" ,",
		(const tJsonUtf8Unit *)"123 1",
		(const tJsonUtf8Unit *)"123 {",
		(const tJsonUtf8Unit *)"123 [",
		(const tJsonUtf8Unit *)"123 :",
		(const tJsonUtf8Unit *)"123 \"",
		(const tJsonUtf8Unit *)"123 ,",
	};
	size_t n;

	for (n = 0; n < sizeof(ValidContent) / sizeof(ValidContent[0]); n++)
	{
		TestResult = TestJsonParseComplete(TestResult, ValidContent[n], false);
		TestResult = TestJsonParseComplete(TestResult, ValidContent[n], true);
	}

	for (n = 0; n < sizeof(ValidContent) / sizeof(ValidContent[0]); n++)
	{
		if (!JsonCharacterIsLiteral(*ValidContent[n]) && (*ValidContent[n] != '/'))
		{
			TestResult = TestJsonParseIncomplete(TestResult, ValidContent[n], false);
			TestResult = TestJsonParseIncomplete(TestResult, ValidContent[n], true);
		}
	}

	for (n = 0; n < sizeof(InvalidContent) / sizeof(InvalidContent[0]); n++)
	{
		TestResult = TestJsonParseError(TestResult, InvalidContent[n]);
	}

	return TestResult;
}


static const tTestCase TestCaseJsonParse[] =
{
	{ "JsonParse", TestJsonParseContent },
};


const tTest TestJsonParse =
{
	"JsonParse",
	TestCaseJsonParse,
	sizeof(TestCaseJsonParse) / sizeof(TestCaseJsonParse[0])
};
