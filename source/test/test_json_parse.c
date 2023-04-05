#include <string.h>
#include "json_character.h"
#include "json_parse.h"
#include "test_json.h"


static tTestResult TestJsonParseComplete(tTestResult TestResult, tJsonUtfType UtfType, const char *Content, bool StripComments)
{
	tJsonElement Root;
	tJsonParse Parse;
	tJsonUtf Utf;
	size_t Length;

	JsonElementSetUp(&Root);
	JsonParseSetUp(&Parse, StripComments, &Root);

	for (; *Content != '\0'; Content++)
	{
		Length = JsonUtfEncode(UtfType, Utf, sizeof(Utf), 0, *Content);
		TEST_IS_EQ(JsonParse(&Parse, UtfType, Utf, Length, NULL), JSON_PARSE_INCOMPLETE, TestResult);
	}

	Length = JsonUtfEncode(UtfType, Utf, sizeof(Utf), 0, *Content);
	TEST_IS_EQ(JsonParse(&Parse, UtfType, Utf, Length, NULL), JSON_PARSE_COMPLETE, TestResult);

	JsonParseCleanUp(&Parse);
	JsonElementCleanUp(&Root);

	return TestResult;
}


static tTestResult TestJsonParseIncomplete(tTestResult TestResult, tJsonUtfType UtfType, const char *Content, bool StripComments)
{
	tJsonElement Root;
	tJsonParse Parse;
	tJsonUtf Utf;
	size_t UtfLength;
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
			UtfLength = JsonUtfEncode(UtfType, Utf, sizeof(Utf), 0, Content[Index]);
			TEST_IS_EQ(JsonParse(&Parse, UtfType, Utf, UtfLength, NULL), JSON_PARSE_INCOMPLETE, TestResult);
		}

		UtfLength = JsonUtfEncode(UtfType, Utf, sizeof(Utf), 0, '\0');
		TEST_IS_EQ(JsonParse(&Parse, UtfType, Utf, UtfLength, NULL), JSON_PARSE_ERROR, TestResult);

		JsonParseCleanUp(&Parse);
		JsonElementCleanUp(&Root);
	}

	return TestResult;
}


static tTestResult TestJsonParseError(tTestResult TestResult, tJsonUtfType UtfType, const char *Content)
{
	tJsonElement Root;
	tJsonParse Parse;
	tJsonUtf Utf;
	size_t UtfLength;

	JsonElementSetUp(&Root);
	JsonParseSetUp(&Parse, true, &Root);

	for (; (Content[0] != '\0') && (Content[1] != '\0'); Content++)
	{
		UtfLength = JsonUtfEncode(UtfType, Utf, sizeof(Utf), 0, *Content);
		TEST_IS_EQ(JsonParse(&Parse, UtfType, Utf, UtfLength, NULL), JSON_PARSE_INCOMPLETE, TestResult);
	}

	UtfLength = JsonUtfEncode(UtfType, Utf, sizeof(Utf), 0, *Content);
	TEST_IS_EQ(JsonParse(&Parse, UtfType, Utf, UtfLength, NULL), JSON_PARSE_ERROR, TestResult);

	JsonParseCleanUp(&Parse);
	JsonElementCleanUp(&Root);

	return TestResult;
}


static tTestResult TestJsonParseContent(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	static const char *ValidContent[] =
	{
		"{ "
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
		   "\"\\t\\r\\n\\b\\f\\\\\\\"\", "
		   "{}, "
		   "{ \"key\": value }, "
		   "[], "
		   "[ 1, 2, 3 ] "
		"]",
		"\"string:{}[]\\t\\r\\n\\b\\f\\\\\\\"\"",
		"\"\\u0001\\u0002\\u0003\\u0004\\u0005\\u0006\\u0007\\u0008\\u0009\\u000A\\u000B\\u000C\\u000D\\u000E\\u000F"
		  "\\u0010\\u0011\\u0012\\u0013\\u0014\\u0015\\u0016\\u0017\\u0018\\u0019\\u001A\\u001B\\u001C\\u001D\\u001E\\u001F\"",
		"true",
		"false",
		"null",
		"1234567890",
		"1.234567890e-99",
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
		"// Comment 34\n"
	};
	static const char *InvalidContent[] =
	{
		"{ ,",
		"{ 1",
		"{ {",
		"{ [",
		"{ :",
		"{ \"abc\" ,",
		"{ \"abc\" 1",
		"{ \"abc\" {",
		"{ \"abc\" [",
		"{ \"abc\" \"",
		"[ ,",
		"[ :",
		"[ \"abc\" {",
		"[ \"abc\" [",
		"[ \"abc\" :",
		"[ \"abc\" 1",
		"[ \"abc\" \"",
		"[ 123 {",
		"[ 123 [",
		"[ 123 :",
		"[ 123 1",
		"[ 123 \"",
		"[ {} {",
		"[ {} [",
		"[ {} :",
		"[ {} 1",
		"[ {} \"",
		"[ [] {",
		"[ [] [",
		"[ [] :",
		"[ [] 1",
		"[ [] \"",
		"\"abc\" 1",
		"\"abc\" {",
		"\"abc\" [",
		"\"abc\" :",
		"\"abc\" \"",
		"\"abc\" ,",
		"123 1",
		"123 {",
		"123 [",
		"123 :",
		"123 \"",
		"123 ,",
		"\"\\ux",
		"\"\\u0x",
		"\"\\u00x",
		"\"\\u000x",
		"\"\\uD800x",
		"\"\\uD800\\x",
		"\"\\uD800\\ux",
		"\"\\uD800\\uDx",
		"\"\\uD800\\uDCx",
		"\"\\uD800\\uDC0x",
		"\"\\uD800\\uDBFF",
		"\"\\uD800\\uE000",
	};
	tJsonUtfType UtfTypes[] = { json_Utf8, json_Utf16be, json_Utf16le };
	size_t Type;
	size_t n;

	for (Type = 0; Type < sizeof(UtfTypes) / sizeof(UtfTypes[0]); Type++)
	{
		for (n = 0; n < sizeof(ValidContent) / sizeof(ValidContent[0]); n++)
		{
			TestResult = TestJsonParseComplete(TestResult, UtfTypes[Type], ValidContent[n], false);
			TestResult = TestJsonParseComplete(TestResult, UtfTypes[Type], ValidContent[n], true);
		}

		for (n = 0; n < sizeof(ValidContent) / sizeof(ValidContent[0]); n++)
		{
			if (!JsonCharacterIsLiteral(*ValidContent[n]) && (*ValidContent[n] != '/'))
			{
				TestResult = TestJsonParseIncomplete(TestResult, UtfTypes[Type], ValidContent[n], false);
				TestResult = TestJsonParseIncomplete(TestResult, UtfTypes[Type], ValidContent[n], true);
			}
		}

		for (n = 0; n < sizeof(InvalidContent) / sizeof(InvalidContent[0]); n++)
		{
			TestResult = TestJsonParseError(TestResult, UtfTypes[Type], InvalidContent[n]);
		}
	}

	return TestResult;
}


static tTestResult TestJsonParseKeyUtf16Valid(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonCharacter Character;
	tJsonElement Root;
	tJsonParse Parse;
	tJsonUtf16 Utf16;
	size_t Length;
	size_t n;

	JsonElementSetUp(&Root);
	JsonParseSetUp(&Parse, false, &Root);

	TEST_IS_EQ(JsonParseCharacter(&Parse, '{'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '"'), JSON_PARSE_INCOMPLETE, TestResult);

	for (Character = 0x01; Character < 0x110000; Character++)
	{
		Length = JsonUtf16beEncode(Utf16, sizeof(Utf16), 0, Character);
		for (n = 0; n < Length; n++)
		{
			if (n % JSON_UTF16_UNIT_SIZE == 0)
			{
				TEST_IS_EQ(JsonParseCharacter(&Parse, '\\'), JSON_PARSE_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonParseCharacter(&Parse, 'u'), JSON_PARSE_INCOMPLETE, TestResult);
			}
			TEST_IS_EQ(JsonParseCharacter(&Parse, JsonCharacterFromHexDigit(Utf16[n] >> 4)), JSON_PARSE_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonParseCharacter(&Parse, JsonCharacterFromHexDigit(Utf16[n] & 0x0F)), JSON_PARSE_INCOMPLETE, TestResult);
		}
	}

	TEST_IS_EQ(JsonParseCharacter(&Parse, '"'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, ':'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '{'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '}'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '}'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '\0'), JSON_PARSE_COMPLETE, TestResult);

	JsonParseCleanUp(&Parse);
	JsonElementCleanUp(&Root);

	return TestResult;
}


static tTestResult TestJsonParseKeyUtf16Invalid(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonElement Root;
	tJsonParse Parse;
	tJsonCharacter Character;
	tJsonUtf16 Utf16;
	size_t Length;
	size_t m;
	size_t n;

	for (Character = 0x01; Character < 0x110000; Character++)
	{
		Length = JsonUtf16beEncode(Utf16, sizeof(Utf16), 0, Character);

		for (m = 1; m < Length; m++)
		{
			JsonElementSetUp(&Root);
			JsonParseSetUp(&Parse, false, &Root);

			TEST_IS_EQ(JsonParseCharacter(&Parse, '{'), JSON_PARSE_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonParseCharacter(&Parse, '"'), JSON_PARSE_INCOMPLETE, TestResult);

			for (n = 0; n < Length - m; n++)
			{
				if (n % JSON_UTF16_UNIT_SIZE == 0)
				{
					TEST_IS_EQ(JsonParseCharacter(&Parse, '\\'), JSON_PARSE_INCOMPLETE, TestResult);
					TEST_IS_EQ(JsonParseCharacter(&Parse, 'u'), JSON_PARSE_INCOMPLETE, TestResult);
				}
				TEST_IS_EQ(JsonParseCharacter(&Parse, JsonCharacterFromHexDigit(Utf16[n] >> 4)), JSON_PARSE_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonParseCharacter(&Parse, JsonCharacterFromHexDigit(Utf16[n] & 0x0F)), JSON_PARSE_INCOMPLETE, TestResult);
			}
			TEST_IS_EQ(JsonParseCharacter(&Parse, '"'), JSON_PARSE_ERROR, TestResult);

			JsonParseCleanUp(&Parse);
			JsonElementCleanUp(&Root);
		}
	}

	for (n = 0xD800; n < 0xDC00; n++)
	{
		JsonElementSetUp(&Root);
		JsonParseSetUp(&Parse, false, &Root);

		TEST_IS_EQ(JsonParseCharacter(&Parse, '{'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParseCharacter(&Parse, '"'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParseCharacter(&Parse, '\\'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParseCharacter(&Parse, 'u'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParseCharacter(&Parse, JsonCharacterFromHexDigit((n >> 12) & 0x0F)), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParseCharacter(&Parse, JsonCharacterFromHexDigit((n >> 8) & 0x0F)), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParseCharacter(&Parse, JsonCharacterFromHexDigit((n >> 4) & 0x0F)), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParseCharacter(&Parse, JsonCharacterFromHexDigit(n & 0x0F)), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParseCharacter(&Parse, '"'), JSON_PARSE_ERROR, TestResult);

		JsonParseCleanUp(&Parse);
		JsonElementCleanUp(&Root);
	}

	for (n = 0xDC00; n < 0xE000; n++)
	{
		JsonElementSetUp(&Root);
		JsonParseSetUp(&Parse, false, &Root);

		TEST_IS_EQ(JsonParseCharacter(&Parse, '{'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParseCharacter(&Parse, '"'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParseCharacter(&Parse, '\\'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParseCharacter(&Parse, 'u'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParseCharacter(&Parse, JsonCharacterFromHexDigit((n >> 12) & 0x0F)), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParseCharacter(&Parse, JsonCharacterFromHexDigit((n >> 8) & 0x0F)), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParseCharacter(&Parse, JsonCharacterFromHexDigit((n >> 4) & 0x0F)), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParseCharacter(&Parse, JsonCharacterFromHexDigit(n & 0x0F)), JSON_PARSE_ERROR, TestResult);

		JsonParseCleanUp(&Parse);
		JsonElementCleanUp(&Root);
	}

	return TestResult;
}


static tTestResult TestJsonParseValueUtf16Valid(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonCharacter Character;
	tJsonElement Root;
	tJsonParse Parse;
	tJsonUtf16 Utf16;
	size_t Length;
	size_t n;

	JsonElementSetUp(&Root);
	JsonParseSetUp(&Parse, false, &Root);

	TEST_IS_EQ(JsonParseCharacter(&Parse, '"'), JSON_PARSE_INCOMPLETE, TestResult);

	for (Character = 0x01; Character < 0x110000; Character++)
	{
		Length = JsonUtf16beEncode(Utf16, sizeof(Utf16), 0, Character);
		for (n = 0; n < Length; n++)
		{
			if (n % JSON_UTF16_UNIT_SIZE == 0)
			{
				TEST_IS_EQ(JsonParseCharacter(&Parse, '\\'), JSON_PARSE_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonParseCharacter(&Parse, 'u'), JSON_PARSE_INCOMPLETE, TestResult);
			}
			TEST_IS_EQ(JsonParseCharacter(&Parse, JsonCharacterFromHexDigit(Utf16[n] >> 4)), JSON_PARSE_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonParseCharacter(&Parse, JsonCharacterFromHexDigit(Utf16[n] & 0x0F)), JSON_PARSE_INCOMPLETE, TestResult);
		}
	}

	TEST_IS_EQ(JsonParseCharacter(&Parse, '"'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '\0'), JSON_PARSE_COMPLETE, TestResult);

	JsonParseCleanUp(&Parse);
	JsonElementCleanUp(&Root);

	return TestResult;
}


static tTestResult TestJsonParseValueUtf16Invalid(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonElement Root;
	tJsonParse Parse;
	tJsonCharacter Character;
	tJsonUtf16 Utf16;
	size_t Length;
	size_t m;
	size_t n;

	for (Character = 0x01; Character < 0x110000; Character++)
	{
		Length = JsonUtf16beEncode(Utf16, sizeof(Utf16), 0, Character);

		for (m = 1; m < Length; m++)
		{
			JsonElementSetUp(&Root);
			JsonParseSetUp(&Parse, false, &Root);

			TEST_IS_EQ(JsonParseCharacter(&Parse, '"'), JSON_PARSE_INCOMPLETE, TestResult);

			for (n = 0; n < Length - m; n++)
			{
				if (n % JSON_UTF16_UNIT_SIZE == 0)
				{
					TEST_IS_EQ(JsonParseCharacter(&Parse, '\\'), JSON_PARSE_INCOMPLETE, TestResult);
					TEST_IS_EQ(JsonParseCharacter(&Parse, 'u'), JSON_PARSE_INCOMPLETE, TestResult);
				}
				TEST_IS_EQ(JsonParseCharacter(&Parse, JsonCharacterFromHexDigit(Utf16[n] >> 4)), JSON_PARSE_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonParseCharacter(&Parse, JsonCharacterFromHexDigit(Utf16[n] & 0x0F)), JSON_PARSE_INCOMPLETE, TestResult);
			}
			TEST_IS_EQ(JsonParseCharacter(&Parse, '"'), JSON_PARSE_ERROR, TestResult);

			JsonParseCleanUp(&Parse);
			JsonElementCleanUp(&Root);
		}
	}

	for (n = 0xD800; n < 0xDC00; n++)
	{
		JsonElementSetUp(&Root);
		JsonParseSetUp(&Parse, false, &Root);

		TEST_IS_EQ(JsonParseCharacter(&Parse, '"'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParseCharacter(&Parse, '\\'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParseCharacter(&Parse, 'u'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParseCharacter(&Parse, JsonCharacterFromHexDigit((n >> 12) & 0x0F)), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParseCharacter(&Parse, JsonCharacterFromHexDigit((n >> 8) & 0x0F)), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParseCharacter(&Parse, JsonCharacterFromHexDigit((n >> 4) & 0x0F)), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParseCharacter(&Parse, JsonCharacterFromHexDigit(n & 0x0F)), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParseCharacter(&Parse, '"'), JSON_PARSE_ERROR, TestResult);

		JsonParseCleanUp(&Parse);
		JsonElementCleanUp(&Root);
	}

	for (n = 0xDC00; n < 0xE000; n++)
	{
		JsonElementSetUp(&Root);
		JsonParseSetUp(&Parse, false, &Root);

		TEST_IS_EQ(JsonParseCharacter(&Parse, '"'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParseCharacter(&Parse, '\\'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParseCharacter(&Parse, 'u'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParseCharacter(&Parse, JsonCharacterFromHexDigit((n >> 12) & 0x0F)), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParseCharacter(&Parse, JsonCharacterFromHexDigit((n >> 8) & 0x0F)), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParseCharacter(&Parse, JsonCharacterFromHexDigit((n >> 4) & 0x0F)), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParseCharacter(&Parse, JsonCharacterFromHexDigit(n & 0x0F)), JSON_PARSE_ERROR, TestResult);

		JsonParseCleanUp(&Parse);
		JsonElementCleanUp(&Root);
	}

	return TestResult;
}


static tTestResult TestJsonParseLiteralValid(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonElement Root;
	tJsonParse Parse;
	static const char ValidCharacters[] = "+-.0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	size_t n;

	JsonElementSetUp(&Root);
	JsonParseSetUp(&Parse, false, &Root);

	for (n = 0; n < sizeof(ValidCharacters) - 1; n++)
	{
		TEST_IS_EQ(JsonParseCharacter(&Parse, ValidCharacters[n]), JSON_PARSE_INCOMPLETE, TestResult);
	}

	TEST_IS_EQ(JsonParseCharacter(&Parse, '\0'), JSON_PARSE_COMPLETE, TestResult);

	JsonParseCleanUp(&Parse);
	JsonElementCleanUp(&Root);

	return TestResult;
}


static tTestResult TestJsonParseLiteralInvalid(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonElement Root;
	tJsonParse Parse;
	tJsonCharacter Character;
	static const char ValidCharacters[] = "+-.0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	static const char ReservedCharacters[] = "\"{}[]/ \t\r\n";

	for (Character = 1; Character <= 0x110000; Character++)
	{
		JsonElementSetUp(&Root);
		JsonParseSetUp(&Parse, false, &Root);

		if ((strchr(ValidCharacters, Character) == NULL) && (strchr(ReservedCharacters, Character) == NULL))
		{
			TEST_IS_EQ(JsonParseCharacter(&Parse, Character), JSON_PARSE_ERROR, TestResult);
		}

		JsonParseCleanUp(&Parse);
		JsonElementCleanUp(&Root);
	}

	JsonElementCleanUp(&Root);

	return TestResult;
}


static tTestResult TestJsonParseCommentLineValid(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonElement Root;
	tJsonParse Parse;
	tJsonCharacter Character;

	JsonElementSetUp(&Root);
	JsonParseSetUp(&Parse, false, &Root);

	TEST_IS_EQ(JsonParseCharacter(&Parse, '/'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '/'), JSON_PARSE_INCOMPLETE, TestResult);

	for (Character = 0x20; Character < 0xD800; Character++)
	{
		TEST_IS_EQ(JsonParseCharacter(&Parse, Character), JSON_PARSE_INCOMPLETE, TestResult);
	}

	for (Character = 0xE000; Character < 0x110000; Character++)
	{
		TEST_IS_EQ(JsonParseCharacter(&Parse, Character), JSON_PARSE_INCOMPLETE, TestResult);
	}

	TEST_IS_EQ(JsonParseCharacter(&Parse, '\n'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '{'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '/'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '/'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '/'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '/'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '/'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '\n'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '}'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '\0'), JSON_PARSE_COMPLETE, TestResult);

	JsonParseCleanUp(&Parse);
	JsonElementCleanUp(&Root);

	return TestResult;
}


static tTestResult TestJsonParseCommentLineInvalid(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonElement Root;
	tJsonParse Parse;

	JsonElementSetUp(&Root);
	JsonParseSetUp(&Parse, false, &Root);

	TEST_IS_EQ(JsonParseCharacter(&Parse, '/'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '\0'), JSON_PARSE_ERROR, TestResult);

	JsonParseCleanUp(&Parse);
	JsonElementCleanUp(&Root);

	return TestResult;
}


static tTestResult TestJsonParseCommentBlockValid(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonElement Root;
	tJsonParse Parse;
	tJsonCharacter Character;

	JsonElementSetUp(&Root);
	JsonParseSetUp(&Parse, false, &Root);

	TEST_IS_EQ(JsonParseCharacter(&Parse, '/'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '*'), JSON_PARSE_INCOMPLETE, TestResult);

	for (Character = 0x20; Character < 0xD800; Character++)
	{
		TEST_IS_EQ(JsonParseCharacter(&Parse, Character), JSON_PARSE_INCOMPLETE, TestResult);
	}

	for (Character = 0xE000; Character < 0x110000; Character++)
	{
		TEST_IS_EQ(JsonParseCharacter(&Parse, Character), JSON_PARSE_INCOMPLETE, TestResult);
	}

	TEST_IS_EQ(JsonParseCharacter(&Parse, '*'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '/'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '{'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '/'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '*'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '/'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '*'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '/'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '}'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '\0'), JSON_PARSE_COMPLETE, TestResult);

	JsonParseCleanUp(&Parse);
	JsonElementCleanUp(&Root);

	return TestResult;
}


static tTestResult TestJsonParseCommentBlockInvalid(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonElement Root;
	tJsonParse Parse;

	JsonElementSetUp(&Root);
	JsonParseSetUp(&Parse, false, &Root);

	TEST_IS_EQ(JsonParseCharacter(&Parse, '/'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '*'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '/'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '*'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParseCharacter(&Parse, '\0'), JSON_PARSE_ERROR, TestResult);

	JsonParseCleanUp(&Parse);
	JsonElementCleanUp(&Root);

	return TestResult;
}


static const tTestCase TestCaseJsonParse[] =
{
	{ "JsonParseContent",              TestJsonParseContent              },
	{ "JsonParseKeyUtf16Valid",        TestJsonParseKeyUtf16Valid        },
	{ "JsonParseKeyUtf16Invalid",      TestJsonParseKeyUtf16Invalid      },
	{ "JsonParseValueUtf16Valid",      TestJsonParseValueUtf16Valid      },
	{ "JsonParseValueUtf16Invalid",    TestJsonParseValueUtf16Invalid    },
	{ "JsonParseCommentLineValid",     TestJsonParseCommentLineValid     },
	{ "JsonParseCommentLineInvalid",   TestJsonParseCommentLineInvalid   },
	{ "JsonParseCommentBlockValid",    TestJsonParseCommentBlockValid    },
	{ "JsonParseCommentBlockInvalid",  TestJsonParseCommentBlockInvalid  },
	{ "JsonParseLiteralValid",         TestJsonParseLiteralValid         },
	{ "JsonParseLiteralInvalid",       TestJsonParseLiteralInvalid       },
};


const tTest TestJsonParse =
{
	"JsonParse",
	TestCaseJsonParse,
	sizeof(TestCaseJsonParse) / sizeof(TestCaseJsonParse[0])
};
