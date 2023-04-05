#include <string.h>
#include "json_parse.h"
#include "json_format.h"
#include "test_json.h"


static tTestResult TestJsonFormatCompressContent(tTestResult TestResult, tJsonUtfType UtfType, const char *Content)
{
	tJsonElement Root;
	tJsonParse Parse;
	tJsonFormat Format;
	tJsonUtf Utf;
	tJsonCharacter Character;
	size_t Offset;
	size_t Index;

	JsonElementSetUp(&Root);

	JsonParseSetUp(&Parse, false, &Root);

	TEST_IS_EQ(JsonParse(&Parse, json_Utf8, (const uint8_t *)Content, strlen(Content) + 1, 0), JSON_PARSE_COMPLETE, TestResult);

	JsonParseCleanUp(&Parse);

	JsonFormatSetUpCompress(&Format, &Root);

	for (Index = 0; Content[Index] != 0; Index++)
	{
		Offset = 0;
		TEST_IS_EQ(JsonFormat(&Format, UtfType, Utf, sizeof(Utf), &Offset), JSON_FORMAT_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonUtfDecode(UtfType, Utf, Offset, 0, &Character), Offset, TestResult);
		TEST_IS_EQ(Character, Content[Index], TestResult);
	}

	Offset = 0;
	TEST_IS_EQ(JsonFormat(&Format, UtfType, Utf, sizeof(Utf), &Offset), JSON_FORMAT_COMPLETE, TestResult);
	TEST_IS_EQ(JsonUtfDecode(UtfType, Utf, Offset, 0, &Character), Offset, TestResult);
	TEST_IS_EQ(Character, Content[Index], TestResult);

	JsonFormatCleanUp(&Format);

	JsonElementCleanUp(&Root);

	return TestResult;
}


static tTestResult TestJsonFormatSpaceContent(tTestResult TestResult, tJsonUtfType UtfType, const char *Content)
{
	tJsonElement Root;
	tJsonParse Parse;
	tJsonFormat Format;
	tJsonUtf Utf;
	tJsonCharacter Character;
	size_t Offset;
	size_t Index;

	JsonElementSetUp(&Root);

	JsonParseSetUp(&Parse, false, &Root);

	TEST_IS_EQ(JsonParse(&Parse, json_Utf8, (const uint8_t *)Content, strlen(Content) + 1, 0), JSON_PARSE_COMPLETE, TestResult);

	JsonParseCleanUp(&Parse);

	JsonFormatSetUpSpace(&Format, &Root);

	for (Index = 0; Content[Index] != 0; Index++)
	{
		Offset = 0;
		TEST_IS_EQ(JsonFormat(&Format, UtfType, Utf, sizeof(Utf), &Offset), JSON_FORMAT_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonUtfDecode(UtfType, Utf, Offset, 0, &Character), Offset, TestResult);
		TEST_IS_EQ(Character, Content[Index], TestResult);
	}

	Offset = 0;
	TEST_IS_EQ(JsonFormat(&Format, UtfType, Utf, sizeof(Utf), &Offset), JSON_FORMAT_COMPLETE, TestResult);
	TEST_IS_EQ(JsonUtfDecode(UtfType, Utf, Offset, 0, &Character), Offset, TestResult);
	TEST_IS_EQ(Character, Content[Index], TestResult);

	JsonFormatCleanUp(&Format);

	JsonElementCleanUp(&Root);

	return TestResult;
}


static tTestResult TestJsonFormatIndentContent(tTestResult TestResult, tJsonUtfType UtfType, const char *Content, size_t IndentSize, tJsonCommentType CommentType)
{
	tJsonElement Root;
	tJsonParse Parse;
	tJsonFormat Format;
	tJsonUtf Utf;
	tJsonCharacter Character;
	size_t Offset;
	size_t Index;

	JsonElementSetUp(&Root);

	JsonParseSetUp(&Parse, false, &Root);

	TEST_IS_EQ(JsonParse(&Parse, json_Utf8, (const uint8_t *)Content, strlen(Content) + 1, 0), JSON_PARSE_COMPLETE, TestResult);

	JsonParseCleanUp(&Parse);

	JsonFormatSetUpIndent(&Format, IndentSize, CommentType, &Root);

	for (Index = 0; Content[Index] != 0; Index++)
	{
		Offset = 0;
		TEST_IS_EQ(JsonFormat(&Format, UtfType, Utf, sizeof(Utf), &Offset), JSON_FORMAT_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonUtfDecode(UtfType, Utf, Offset, 0, &Character), Offset, TestResult);
		TEST_IS_EQ(Character, Content[Index], TestResult);
	}

	Offset = 0;
	TEST_IS_EQ(JsonFormat(&Format, UtfType, Utf, sizeof(Utf), &Offset), JSON_FORMAT_COMPLETE, TestResult);
	TEST_IS_EQ(JsonUtfDecode(UtfType, Utf, Offset, 0, &Character), Offset, TestResult);
	TEST_IS_EQ(Character, Content[Index], TestResult);

	JsonFormatCleanUp(&Format);

	JsonElementCleanUp(&Root);

	return TestResult;
}


static tTestResult TestJsonFormatCompress(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	static const char *Content[] =
	{
		"{"
		   "\"key-true\":true,"
		   "\"key-false\":false,"
		   "\"key-null\":null,"
		   "\"key-int\":123,"
		   "\"key-real\":1.23e4,"
		   "\"key-real-nan\":NaN,"
		   "\"key-string\":\"hello world\","
		   "\"key-string-escape\":\"\\t\\r\\n\\b\\f\\\\\\\"\\u0001\","
		   "\"key-object-empty\":{},"
		   "\"key-object\":{\"key\":value},"
		   "\"key-array-empty\":[],"
		   "\"key-array\":["
		      "true,"
		      "false,"
		      "null,"
		      "123,"
		      "1.23e4,"
		      "NaN,"
		      "\"hello world\","
		      "\"\\t\\r\\n\\b\\f\\\\\\\"\\u0001\","
		      "{},"
		      "{\"key\":value},"
		      "[],"
		      "[1,2,3]"
		   "]"
		"}",
		"["
		   "true,"
		   "false,"
		   "null,"
		   "123,"
		   "1.23e4,"
		   "NaN,"
		   "\"hello world\","
		   "\"\\t\\r\\n\\b\\f\\\\\\\"\\u0001\","
		   "{},"
		   "{\"key\":value},"
		   "[],"
		   "[1,2,3]"
		"]",
		"\"string:{}[]\\t\\r\\n\\b\\f\\\\\\\"\\u0001\"",
		"true",
		"false",
		"null",
		"1234567890",
		"1.234567890e-99"
	};
	tJsonUtfType UtfTypes[] = { json_Utf8, json_Utf16be, json_Utf16le };
	size_t Type;
	size_t n;

	for (Type = 0; Type < sizeof(UtfTypes) / sizeof(UtfTypes[0]); Type++)
	{
		for (n = 0; n < sizeof(Content) / sizeof(Content[0]); n++)
		{
			TestResult = TestJsonFormatCompressContent(TestResult, UtfTypes[Type], Content[n]);
		}
	}

	return TestResult;
}


static tTestResult TestJsonFormatSpace(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	static const char *Content[] =
	{
		"{ "
		   "\"key-true\": true, "
		   "\"key-false\": false, "
		   "\"key-null\": null, "
		   "\"key-int\": 123, "
		   "\"key-real\": 1.23e4, "
		   "\"key-real-nan\": NaN, "
		   "\"key-string\": \"hello world\", "
		   "\"key-string-escape\": \"\\t\\r\\n\\b\\f\\\\\\\"\\u0001\", "
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
		      "\"\\t\\r\\n\\b\\f\\\\\\\"\\u0001\", "
		      "{}, "
		      "{ \"key\": value }, "
		      "[], "
		      "[ 1, 2, 3 ] "
		   "] "
		"}",
		"[ "
		   "true, "
		   "false, "
		   "null, "
		   "123, "
		   "1.23e4, "
		   "NaN, "
		   "\"hello world\", "
		   "\"\\t\\r\\n\\b\\f\\\\\\\"\\u0001\", "
		   "{}, "
		   "{ \"key\": value }, "
		   "[], "
		   "[ 1, 2, 3 ] "
		"]",
		"\"string:{}[]\\t\\r\\n\\b\\f\\\\\\\"\\u0001\"",
		"true",
		"false",
		"null",
		"1234567890",
		"1.234567890e-99"
	};
	tJsonUtfType UtfTypes[] = { json_Utf8, json_Utf16be, json_Utf16le };
	size_t Type;
	size_t n;

	for (Type = 0; Type < sizeof(UtfTypes) / sizeof(UtfTypes[0]); Type++)
	{
		for (n = 0; n < sizeof(Content) / sizeof(Content[0]); n++)
		{
			TestResult = TestJsonFormatSpaceContent(TestResult, UtfTypes[Type], Content[n]);
		}
	}

	return TestResult;
}


static tTestResult TestJsonFormatIndent(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	static const char *Content[] =
	{
		"{\n"
		"   \"key-true\": true,\n"
		"   \"key-false\": false,\n"
		"   \"key-null\": null,\n"
		"   \"key-int\": 123,\n"
		"   \"key-real\": 1.23e4,\n"
		"   \"key-real-nan\": NaN,\n"
		"   \"key-string\": \"hello world\",\n"
		"   \"key-string-escape\": \"\\t\\r\\n\\b\\f\\\\\\\"\\u0001\",\n"
		"   \"key-object-empty\": {},\n"
		"   \"key-object\": {\n"
		"      \"key\": value\n"
		"   },\n"
		"   \"key-array-empty\": [],\n"
		"   \"key-array\": [\n"
		"      true,\n"
		"      false,\n"
		"      null,\n"
		"      123,\n"
		"      1.23e4,\n"
		"      NaN,\n"
		"      \"hello world\",\n"
		"      \"\\t\\r\\n\\b\\f\\\\\\\"\\u0001\",\n"
		"      {},\n"
		"      {\n"
		"         \"key\": value\n"
		"      },\n"
		"      [],\n"
		"      [\n"
		"         1,\n"
		"         2,\n"
		"         3\n"
		"      ]\n"
		"   ]\n"
		"}",
		"[]",
		"[\n"
		"   1\n"
		"]",
		"[\n"
		"   {}\n"
		"]",
		"[\n"
		"   {\n"
		"      \"key\": \"value\"\n"
		"   }\n"
		"]",
		"[\n"
		"   []\n"
		"]",
		"[\n"
		"   [\n"
		"      1\n"
		"   ]\n"
		"]",
		"[\n"
		"   [\n"
		"      1,\n"
		"      2\n"
		"   ]\n"
		"]",
		"[\n"
		"   true,\n"
		"   false,\n"
		"   null,\n"
		"   123,\n"
		"   1.23e4,\n"
		"   NaN,\n"
		"   \"hello world\",\n"
		"   \"\\t\\r\\n\\b\\f\\\\\\\"\\u0001\",\n"
		"   {},\n"
		"   {\n"
		"      \"key\": value\n"
		"   },\n"
		"   [],\n"
		"   [\n"
		"      1,\n"
		"      2,\n"
		"      3\n"
		"   ]\n"
		"]",
		"\"string:{}[]\\t\\r\\n\\b\\f\\\\\\\"\\u0001\"",
		"true",
		"false",
		"null",
		"1234567890",
		"1.234567890e-99"
	};
	tJsonUtfType UtfTypes[] = { json_Utf8, json_Utf16be, json_Utf16le };
	size_t Type;
	size_t n;

	for (Type = 0; Type < sizeof(UtfTypes) / sizeof(UtfTypes[0]); Type++)
	{
		for (n = 0; n < sizeof(Content) / sizeof(Content[0]); n++)
		{
			TestResult = TestJsonFormatIndentContent(TestResult, UtfTypes[Type], Content[n], 3, json_CommentNone);
			TestResult = TestJsonFormatIndentContent(TestResult, UtfTypes[Type], Content[n], 3, json_CommentLine);
			TestResult = TestJsonFormatIndentContent(TestResult, UtfTypes[Type], Content[n], 3, json_CommentBlock);
		}
	}

	return TestResult;
}


static tTestResult TestJsonFormatCommentLine(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	static const char Content[] =
		"// Comment 1\n"
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
		"// Comment 34\n";
	static const char ContentIndentCommentNone[] =
		"{\n"
		"   \"key1\": \"value1\",\n"
		"   \"key2\": [\n"
		"      \"value2\",\n"
		"      \"value3\"\n"
		"   ],\n"
		"   \"key3\": {\n"
		"      \"key4\": \"value4\",\n"
		"      \"key5\": []\n"
		"   }\n"
		"}";
	static const char ContentIndentCommentLine[] =
		"// Comment 1\n"
		"// Comment 2\n"
		"{\n"
		"   // Comment 3\n"
		"   // Comment 4\n"
		"   \"key1\":\n"
		"   // Comment 5\n"
		"   // Comment 6\n"
		"   // Comment 7\n"
		"   // Comment 8\n"
		"   \"value1\",\n"
		"   // Comment 9\n"
		"   // Comment 10\n"
		"   // Comment 11\n"
		"   // Comment 12\n"
		"   \"key2\": [\n"
		"      // Comment 13\n"
		"      // Comment 14\n"
		"      \"value2\",\n"
		"      // Comment 15\n"
		"      // Comment 16\n"
		"      // Comment 17\n"
		"      // Comment 18\n"
		"      \"value3\"\n"
		"      // Comment 19\n"
		"      // Comment 20\n"
		"   ],\n"
		"   // Comment 21\n"
		"   // Comment 22\n"
		"   // Comment 23\n"
		"   // Comment 24\n"
		"   \"key3\": {\n"
		"      // Comment 25\n"
		"      // Comment 26\n"
		"      \"key4\": \"value4\",\n"
		"      // Comment 27\n"
		"      // Comment 28\n"
		"      \"key5\": []\n"
		"      // Comment 29\n"
		"      // Comment 30\n"
		"   }\n"
		"   // Comment 31\n"
		"   // Comment 32\n"
		"}\n"
		"// Comment 33\n"
		"// Comment 34";
	static const char ContentIndentCommentBlock[] =
		"/* Comment 1\n"
		"   Comment 2 */\n"
		"{\n"
		"   /* Comment 3\n"
		"      Comment 4 */\n"
		"   \"key1\":\n"
		"   /* Comment 5\n"
		"      Comment 6\n"
		"      Comment 7\n"
		"      Comment 8 */\n"
		"   \"value1\",\n"
		"   /* Comment 9\n"
		"      Comment 10 */\n"
		"   /* Comment 11\n"
		"      Comment 12 */\n"
		"   \"key2\": [\n"
		"      /* Comment 13\n"
		"         Comment 14 */\n"
		"      \"value2\",\n"
		"      /* Comment 15\n"
		"         Comment 16\n"
		"         Comment 17\n"
		"         Comment 18 */\n"
		"      \"value3\"\n"
		"      /* Comment 19\n"
		"         Comment 20 */\n"
		"   ],\n"
		"   /* Comment 21\n"
		"      Comment 22 */\n"
		"   /* Comment 23\n"
		"      Comment 24 */\n"
		"   \"key3\": {\n"
		"      /* Comment 25\n"
		"         Comment 26 */\n"
		"      \"key4\": \"value4\",\n"
		"      /* Comment 27\n"
		"         Comment 28 */\n"
		"      \"key5\": []\n"
		"      /* Comment 29\n"
		"         Comment 30 */\n"
		"   }\n"
		"   /* Comment 31\n"
		"      Comment 32 */\n"
		"}\n"
		"/* Comment 33\n"
		"   Comment 34 */";
	static const char ContentSpace[] =
		"{ "
		   "\"key1\": \"value1\", "
		   "\"key2\": [ "
		     "\"value2\", "
		     "\"value3\" "
		   "], "
		   "\"key3\": { "
		     "\"key4\": \"value4\", "
		     "\"key5\": [] "
		   "} "
		"}";
	static const char ContentCompress[] =
		"{"
		   "\"key1\":\"value1\","
		   "\"key2\":["
		     "\"value2\","
		     "\"value3\""
		   "],"
		   "\"key3\":{"
		     "\"key4\":\"value4\","
		     "\"key5\":[]"
		   "}"
		"}";
	tJsonElement Root;
	tJsonParse Parse;
	tJsonFormat Format;
	tJsonCharacter Character;
	size_t Index;

	JsonElementSetUp(&Root);

	JsonParseSetUp(&Parse, false, &Root);

	for (Index = 0; Content[Index] != '\0'; Index++)
	{
		TEST_IS_EQ(JsonParseCharacter(&Parse, Content[Index]), JSON_PARSE_INCOMPLETE, TestResult);
	}

	TEST_IS_EQ(JsonParseCharacter(&Parse, Content[Index]), JSON_PARSE_COMPLETE, TestResult);

	JsonParseCleanUp(&Parse);

	JsonFormatSetUpIndent(&Format, 3, json_CommentNone, &Root);

	for (Index = 0; ContentIndentCommentNone[Index] != 0; Index++)
	{
		TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
		TEST_IS_EQ(Character, ContentIndentCommentNone[Index], TestResult);
	}

	TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_COMPLETE, TestResult);
	TEST_IS_EQ(Character, ContentIndentCommentNone[Index], TestResult);

	JsonFormatCleanUp(&Format);

	JsonFormatSetUpIndent(&Format, 3, json_CommentLine, &Root);

	for (Index = 0; ContentIndentCommentLine[Index] != 0; Index++)
	{
		TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
		TEST_IS_EQ(Character, ContentIndentCommentLine[Index], TestResult);
	}

	TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_COMPLETE, TestResult);
	TEST_IS_EQ(Character, ContentIndentCommentLine[Index], TestResult);

	JsonFormatCleanUp(&Format);

	JsonFormatSetUpIndent(&Format, 3, json_CommentBlock, &Root);

	for (Index = 0; ContentIndentCommentBlock[Index] != 0; Index++)
	{
		TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
		TEST_IS_EQ(Character, ContentIndentCommentBlock[Index], TestResult);
	}

	TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_COMPLETE, TestResult);
	TEST_IS_EQ(Character, ContentIndentCommentBlock[Index], TestResult);

	JsonFormatCleanUp(&Format);

	JsonFormatSetUpSpace(&Format, &Root);

	for (Index = 0; ContentSpace[Index] != 0; Index++)
	{
		TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
		TEST_IS_EQ(Character, ContentSpace[Index], TestResult);
	}

	TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_COMPLETE, TestResult);
	TEST_IS_EQ(Character, ContentSpace[Index], TestResult);

	JsonFormatCleanUp(&Format);

	JsonFormatSetUpCompress(&Format, &Root);

	for (Index = 0; ContentCompress[Index] != 0; Index++)
	{
		TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
		TEST_IS_EQ(Character, ContentCompress[Index], TestResult);
	}

	TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_COMPLETE, TestResult);
	TEST_IS_EQ(Character, ContentCompress[Index], TestResult);

	JsonFormatCleanUp(&Format);

	JsonElementCleanUp(&Root);

	return TestResult;
}


static tTestResult TestJsonFormatCommentBlock(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	static const char Content[] =
		"/* Comment 1\n"
		"   Comment 2 */\n"
		"{ /* Comment 3\n"
		"     Comment 4 */\n"
		"   \"key1\" /* Comment 5\n"
		"               Comment 6 */\n"
		"   : /* Comment 7\n"
		"        Comment 8 */\n"
		"   \"value1\" /* Comment 9\n"
		"                 Comment 10 */\n"
		"   , /* Comment 11\n"
		"        Comment 12 */\n"
		"   \"key2\": [ /* Comment 13\n"
		"                  Comment 14 */\n"
		"      \"value2\" /* Comment 15\n"
		"                    Comment 16 */\n"
		"      , /* Comment 17\n"
		"           Comment 18 */\n"
		"      \"value3\" /* Comment 19\n"
		"                    Comment 20 */\n"
		"   ] /* Comment 21\n"
		"        Comment 22 */\n"
		"   , /* Comment 23\n"
		"        Comment 24 */\n"
		"   \"key3\": { /* Comment 25\n"
		"                  Comment 26 */\n"
		"      \"key4\": \"value4\", /* Comment 27\n"
		"                               Comment 28 */\n"
		"      \"key5\": [] /* Comment 29\n"
		"                      Comment 30 */\n"
		"   } /* Comment 31\n"
		"        Comment 32 */\n"
		"} /* Comment 33 */\n"
		"/* Comment 34 */\n";
	static const char ContentIndentCommentNone[] =
		"{\n"
		"   \"key1\": \"value1\",\n"
		"   \"key2\": [\n"
		"      \"value2\",\n"
		"      \"value3\"\n"
		"   ],\n"
		"   \"key3\": {\n"
		"      \"key4\": \"value4\",\n"
		"      \"key5\": []\n"
		"   }\n"
		"}";
	static const char ContentIndentCommentLine[] =
		"// Comment 1\n"
		"//Comment 2 \n"
		"{\n"
		"   // Comment 3\n"
		"   //Comment 4 \n"
		"   \"key1\":\n"
		"   // Comment 5\n"
		"   //Comment 6 \n"
		"   // Comment 7\n"
		"   //Comment 8 \n"
		"   \"value1\",\n"
		"   // Comment 9\n"
		"   //Comment 10 \n"
		"   // Comment 11\n"
		"   //Comment 12 \n"
		"   \"key2\": [\n"
		"      // Comment 13\n"
		"      //Comment 14 \n"
		"      \"value2\",\n"
		"      // Comment 15\n"
		"      //Comment 16 \n"
		"      // Comment 17\n"
		"      //Comment 18 \n"
		"      \"value3\"\n"
		"      // Comment 19\n"
		"      //Comment 20 \n"
		"   ],\n"
		"   // Comment 21\n"
		"   //Comment 22 \n"
		"   // Comment 23\n"
		"   //Comment 24 \n"
		"   \"key3\": {\n"
		"      // Comment 25\n"
		"      //Comment 26 \n"
		"      \"key4\": \"value4\",\n"
		"      // Comment 27\n"
		"      //Comment 28 \n"
		"      \"key5\": []\n"
		"      // Comment 29\n"
		"      //Comment 30 \n"
		"   }\n"
		"   // Comment 31\n"
		"   //Comment 32 \n"
		"}\n"
		"// Comment 33 \n"
		"// Comment 34 ";
	static const char ContentIndentCommentBlock[] =
		"/* Comment 1\n"
		"  Comment 2 */\n"
		"{\n"
		"   /* Comment 3\n"
		"     Comment 4 */\n"
		"   \"key1\":\n"
		"   /* Comment 5\n"
		"     Comment 6 \n"
		"      Comment 7\n"
		"     Comment 8 */\n"
		"   \"value1\",\n"
		"   /* Comment 9\n"
		"     Comment 10 */\n"
		"   /* Comment 11\n"
		"     Comment 12 */\n"
		"   \"key2\": [\n"
		"      /* Comment 13\n"
		"        Comment 14 */\n"
		"      \"value2\",\n"
		"      /* Comment 15\n"
		"        Comment 16 \n"
		"         Comment 17\n"
		"        Comment 18 */\n"
		"      \"value3\"\n"
		"      /* Comment 19\n"
		"        Comment 20 */\n"
		"   ],\n"
		"   /* Comment 21\n"
		"     Comment 22 */\n"
		"   /* Comment 23\n"
		"     Comment 24 */\n"
		"   \"key3\": {\n"
		"      /* Comment 25\n"
		"        Comment 26 */\n"
		"      \"key4\": \"value4\",\n"
		"      /* Comment 27\n"
		"        Comment 28 */\n"
		"      \"key5\": []\n"
		"      /* Comment 29\n"
		"        Comment 30 */\n"
		"   }\n"
		"   /* Comment 31\n"
		"     Comment 32 */\n"
		"}\n"
		"/* Comment 33 \n"
		"   Comment 34 */";
	static const char ContentSpace[] =
		"{ "
		   "\"key1\": \"value1\", "
		   "\"key2\": [ "
		     "\"value2\", "
		     "\"value3\" "
		   "], "
		   "\"key3\": { "
		     "\"key4\": \"value4\", "
		     "\"key5\": [] "
		   "} "
		"}";
	static const char ContentCompress[] =
		"{"
		   "\"key1\":\"value1\","
		   "\"key2\":["
		     "\"value2\","
		     "\"value3\""
		   "],"
		   "\"key3\":{"
		     "\"key4\":\"value4\","
		     "\"key5\":[]"
		   "}"
		"}";
	tJsonElement Root;
	tJsonParse Parse;
	tJsonFormat Format;
	tJsonCharacter Character;
	size_t Index;

	JsonElementSetUp(&Root);

	JsonParseSetUp(&Parse, false, &Root);

	for (Index = 0; Content[Index] != '\0'; Index++)
	{
		TEST_IS_EQ(JsonParseCharacter(&Parse, Content[Index]), JSON_PARSE_INCOMPLETE, TestResult);
	}

	TEST_IS_EQ(JsonParseCharacter(&Parse, Content[Index]), JSON_PARSE_COMPLETE, TestResult);

	JsonParseCleanUp(&Parse);

	JsonFormatSetUpIndent(&Format, 3, json_CommentNone, &Root);

	for (Index = 0; ContentIndentCommentNone[Index] != 0; Index++)
	{
		TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
		TEST_IS_EQ(Character, ContentIndentCommentNone[Index], TestResult);
	}

	TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_COMPLETE, TestResult);
	TEST_IS_EQ(Character, ContentIndentCommentNone[Index], TestResult);

	JsonFormatCleanUp(&Format);

	JsonFormatSetUpIndent(&Format, 3, json_CommentLine, &Root);

	for (Index = 0; ContentIndentCommentLine[Index] != 0; Index++)
	{
		TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
		TEST_IS_EQ(Character, ContentIndentCommentLine[Index], TestResult);
	}

	TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_COMPLETE, TestResult);
	TEST_IS_EQ(Character, ContentIndentCommentLine[Index], TestResult);

	JsonFormatCleanUp(&Format);

	JsonFormatSetUpIndent(&Format, 3, json_CommentBlock, &Root);

	for (Index = 0; ContentIndentCommentBlock[Index] != 0; Index++)
	{
		TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
		TEST_IS_EQ(Character, ContentIndentCommentBlock[Index], TestResult);
	}

	TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_COMPLETE, TestResult);
	TEST_IS_EQ(Character, ContentIndentCommentBlock[Index], TestResult);

	JsonFormatCleanUp(&Format);

	JsonFormatSetUpSpace(&Format, &Root);

	for (Index = 0; ContentSpace[Index] != 0; Index++)
	{
		TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
		TEST_IS_EQ(Character, ContentSpace[Index], TestResult);
	}

	TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_COMPLETE, TestResult);
	TEST_IS_EQ(Character, ContentSpace[Index], TestResult);

	JsonFormatCleanUp(&Format);

	JsonFormatSetUpCompress(&Format, &Root);

	for (Index = 0; ContentCompress[Index] != 0; Index++)
	{
		TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
		TEST_IS_EQ(Character, ContentCompress[Index], TestResult);
	}

	TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_COMPLETE, TestResult);
	TEST_IS_EQ(Character, ContentCompress[Index], TestResult);

	JsonFormatCleanUp(&Format);

	JsonElementCleanUp(&Root);

	return TestResult;
}


static tTestResult TestJsonFormatKeyUtf16(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonCharacter Character;
	tJsonCharacter ExpectedCharacter;
	tJsonElement Root;
	tJsonParse Parse;
	tJsonFormat Format;
	tJsonUtf16 Utf16;
	size_t Length;
	size_t n;

	JsonElementSetUp(&Root);
	JsonParseSetUp(&Parse, false, &Root);

	TEST_IS_EQ(JsonParseCharacter(&Parse, '{'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '"'), JSON_PARSE_INCOMPLETE, TestResult);

	for (Character = 0x01; Character < 0x110000; Character++)
	{
		if (JsonCharacterIsEscapable(Character))
		{
			TEST_IS_EQ(JsonParseCharacter(&Parse, '\\'), JSON_PARSE_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonParseCharacter(&Parse, JsonCharacterToEscape(Character)), JSON_PARSE_INCOMPLETE, TestResult);
		}
		else if ((Character < 0xD800) || (Character >= 0xE000))
		{
			TEST_IS_EQ(JsonParseCharacter(&Parse, Character), JSON_PARSE_INCOMPLETE, TestResult);
		}
	}

	TEST_IS_EQ(JsonParseCharacter(&Parse, '"'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, ':'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '{'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '}'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '}'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '\0'), JSON_PARSE_COMPLETE, TestResult);

	JsonParseCleanUp(&Parse);

	JsonFormatSetUpCompress(&Format, &Root);

	TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
	TEST_IS_EQ(Character, '{', TestResult);
	TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
	TEST_IS_EQ(Character, '"', TestResult);

	for (ExpectedCharacter = 0x01; ExpectedCharacter < 0x110000; ExpectedCharacter++)
	{
		if (JsonCharacterIsEscapable(ExpectedCharacter))
		{
			TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
			TEST_IS_EQ(Character, '\\', TestResult);
			TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonCharacterFromEscape(Character), ExpectedCharacter, TestResult);
		}
		else if (JsonCharacterIsControl(ExpectedCharacter))
		{
			Length = JsonUtf16beEncode(Utf16, sizeof(Utf16), 0, ExpectedCharacter);
			for (n = 0; n < Length; n++)
			{
				if (n % JSON_UTF16_UNIT_SIZE == 0)
				{
					TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
					TEST_IS_EQ(Character, '\\', TestResult);
					TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
					TEST_IS_EQ(Character, 'u', TestResult);
				}
				TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
				TEST_IS_TRUE(JsonCharacterIsHexDigit(Character), TestResult)
				TEST_IS_EQ(JsonCharacterToHexDigit(Character), Utf16[n] >> 4, TestResult);
				TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
				TEST_IS_TRUE(JsonCharacterIsHexDigit(Character), TestResult)
				TEST_IS_EQ(JsonCharacterToHexDigit(Character), Utf16[n] & 0x0F, TestResult);
			}
		}
		else if ((ExpectedCharacter < 0xD800) || (ExpectedCharacter >= 0xE000))
		{
			TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
			TEST_IS_EQ(Character, ExpectedCharacter, TestResult);
		}
	}

	TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
	TEST_IS_EQ(Character, '"', TestResult);
	TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
	TEST_IS_EQ(Character, ':', TestResult);
	TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
	TEST_IS_EQ(Character, '{', TestResult);
	TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
	TEST_IS_EQ(Character, '}', TestResult);
	TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
	TEST_IS_EQ(Character, '}', TestResult);
	TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_COMPLETE, TestResult);
	TEST_IS_EQ(Character, '\0', TestResult);

	JsonFormatCleanUp(&Format);

	JsonElementCleanUp(&Root);

	return TestResult;
}


static tTestResult TestJsonFormatValueUtf16(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonCharacter Character;
	tJsonCharacter ExpectedCharacter;
	tJsonElement Root;
	tJsonParse Parse;
	tJsonFormat Format;
	tJsonUtf16 Utf16;
	size_t Length;
	size_t n;

	JsonElementSetUp(&Root);
	JsonParseSetUp(&Parse, false, &Root);

	TEST_IS_EQ(JsonParseCharacter(&Parse, '"'), JSON_PARSE_INCOMPLETE, TestResult);

	for (Character = 0x01; Character < 0x110000; Character++)
	{
		if (JsonCharacterIsEscapable(Character))
		{
			TEST_IS_EQ(JsonParseCharacter(&Parse, '\\'), JSON_PARSE_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonParseCharacter(&Parse, JsonCharacterToEscape(Character)), JSON_PARSE_INCOMPLETE, TestResult);
		}
		else if ((Character < 0xD800) || (Character >= 0xE000))
		{
			TEST_IS_EQ(JsonParseCharacter(&Parse, Character), JSON_PARSE_INCOMPLETE, TestResult);
		}
	}

	TEST_IS_EQ(JsonParseCharacter(&Parse, '"'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '\0'), JSON_PARSE_COMPLETE, TestResult);

	JsonParseCleanUp(&Parse);

	JsonFormatSetUpCompress(&Format, &Root);

	TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
	TEST_IS_EQ(Character, '"', TestResult);

	for (ExpectedCharacter = 0x01; ExpectedCharacter < 0x110000; ExpectedCharacter++)
	{
		if (JsonCharacterIsEscapable(ExpectedCharacter))
		{
			TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
			TEST_IS_EQ(Character, '\\', TestResult);
			TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonCharacterFromEscape(Character), ExpectedCharacter, TestResult);
		}
		else if (JsonCharacterIsControl(ExpectedCharacter))
		{
			Length = JsonUtf16beEncode(Utf16, sizeof(Utf16), 0, ExpectedCharacter);
			for (n = 0; n < Length; n++)
			{
				if (n % JSON_UTF16_UNIT_SIZE == 0)
				{
					TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
					TEST_IS_EQ(Character, '\\', TestResult);
					TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
					TEST_IS_EQ(Character, 'u', TestResult);
				}
				TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
				TEST_IS_TRUE(JsonCharacterIsHexDigit(Character), TestResult)
				TEST_IS_EQ(JsonCharacterToHexDigit(Character), Utf16[n] >> 4, TestResult);
				TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
				TEST_IS_TRUE(JsonCharacterIsHexDigit(Character), TestResult)
				TEST_IS_EQ(JsonCharacterToHexDigit(Character), Utf16[n] & 0x0F, TestResult);
			}
		}
		else if ((ExpectedCharacter < 0xD800) || (ExpectedCharacter >= 0xE000))
		{
			TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
			TEST_IS_EQ(Character, ExpectedCharacter, TestResult);
		}
	}

	TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_INCOMPLETE, TestResult);
	TEST_IS_EQ(Character, '"', TestResult);
	TEST_IS_EQ(JsonFormatCharacter(&Format, &Character), JSON_FORMAT_COMPLETE, TestResult);
	TEST_IS_EQ(Character, '\0', TestResult);

	JsonFormatCleanUp(&Format);

	JsonElementCleanUp(&Root);

	return TestResult;
}


static const tTestCase TestCaseJsonFormat[] =
{
	{ "JsonFormatCompress",     TestJsonFormatCompress     },
	{ "JsonFormatSpace",        TestJsonFormatSpace        },
	{ "JsonFormatIndent",       TestJsonFormatIndent       },
	{ "JsonFormatCommentLine",  TestJsonFormatCommentLine  },
	{ "JsonFormatCommentBlock", TestJsonFormatCommentBlock },
	{ "JsonFormatKeyUtf16",     TestJsonFormatKeyUtf16     },
	{ "JsonFormatValueUtf16",   TestJsonFormatValueUtf16   },
};


const tTest TestJsonFormat =
{
	"JsonFormat",
	TestCaseJsonFormat,
	sizeof(TestCaseJsonFormat) / sizeof(TestCaseJsonFormat[0])
};
