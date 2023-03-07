#include "json_parse.h"
#include "json_format.h"
#include "test_json.h"


static tTestResult TestJsonValueUtf16Valid(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonElement Root;
	tJsonParse Parse;
	tJsonFormat Format;
	tJsonCharacter Character;
	tJsonUtf16Code Utf16Code;
	tJsonUtf8Code Utf8Code;
	tJsonUtf8Unit CodeUnit;
	size_t Length;
	size_t n;

	JsonElementSetUp(&Root);

	JsonParseSetUp(&Parse, false, &Root);

	TEST_IS_EQ(JsonParse(&Parse, '"'), JSON_PARSE_INCOMPLETE, TestResult);

	for (Character = 0x01; Character < 0x110000; Character++)
	{
		Utf16Code = JsonUtf16Code(Character);
		Length = JsonUtf16CodeGetNibbleLength(Utf16Code);
		for (n = 0; n < Length; n++)
		{
			if (n % 4 == 0)
			{
				TEST_IS_EQ(JsonParse(&Parse, '\\'), JSON_PARSE_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonParse(&Parse, 'u'), JSON_PARSE_INCOMPLETE, TestResult);
			}
			TEST_IS_EQ(JsonParse(&Parse, JsonCharacterFromHexDigit(JsonUtf16CodeGetNibble(Utf16Code, n))), JSON_PARSE_INCOMPLETE, TestResult);
		}
	}

	TEST_IS_EQ(JsonParse(&Parse, '"'), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(JsonParse(&Parse, '\0'), JSON_PARSE_COMPLETE, TestResult);

	JsonParseCleanUp(&Parse);

	JsonFormatSetUpSpace(&Format, &Root);

	TEST_IS_EQ(JsonFormat(&Format, &CodeUnit), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(CodeUnit, '"', TestResult);

	for (Character = 0x01; Character < 0x110000; Character++)
	{
		if (JsonCharacterIsEscapable(Character) || (Character == '\\') || (Character == '"'))
		{
			TEST_IS_EQ(JsonFormat(&Format, &CodeUnit), JSON_PARSE_INCOMPLETE, TestResult);
			TEST_IS_EQ(CodeUnit, '\\', TestResult);
			TEST_IS_EQ(JsonFormat(&Format, &CodeUnit), JSON_PARSE_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonCharacterFromEscape(CodeUnit), Character, TestResult);
		}
		else if (JsonCharacterIsControl(Character))
		{
			Utf16Code = JsonUtf16Code(Character);
			Length = JsonUtf16CodeGetNibbleLength(Utf16Code);
			for (n = 0; n < Length; n++)
			{
				if (n % 4 == 0)
				{
					TEST_IS_EQ(JsonFormat(&Format, &CodeUnit), JSON_PARSE_INCOMPLETE, TestResult);
					TEST_IS_EQ(CodeUnit, '\\', TestResult);
					TEST_IS_EQ(JsonFormat(&Format, &CodeUnit), JSON_PARSE_INCOMPLETE, TestResult);
					TEST_IS_EQ(CodeUnit, 'u', TestResult);
				}
				TEST_IS_EQ(JsonFormat(&Format, &CodeUnit), JSON_PARSE_INCOMPLETE, TestResult);
				TEST_IS_TRUE(JsonCharacterIsHexDigit(CodeUnit), TestResult)
				TEST_IS_EQ(JsonCharacterToHexDigit(CodeUnit), JsonUtf16CodeGetNibble(Utf16Code, n), TestResult);
			}
		}
		else
		{
			Utf8Code = JsonUtf8Code(Character);
			Length = JsonUtf8CodeGetUnitLength(Utf8Code);
			for (n = 0; n < Length; n++)
			{
				TEST_IS_EQ(JsonFormat(&Format, &CodeUnit), JSON_PARSE_INCOMPLETE, TestResult);
				TEST_IS_EQ(CodeUnit, JsonUtf8CodeGetUnit(Utf8Code, n), TestResult);
			}
		}
	}

	TEST_IS_EQ(JsonFormat(&Format, &CodeUnit), JSON_PARSE_INCOMPLETE, TestResult);
	TEST_IS_EQ(CodeUnit, '"', TestResult);
	TEST_IS_EQ(JsonFormat(&Format, &CodeUnit), JSON_PARSE_COMPLETE, TestResult);
	TEST_IS_EQ(CodeUnit, '\0', TestResult);

	JsonFormatCleanUp(&Format);

	JsonElementCleanUp(&Root);

	return TestResult;
}


static tTestResult TestJsonValueUtf8Invalid(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonElement Root;
	tJsonParse Parse;
	tJsonCharacter Character;
	tJsonUtf16Code Utf16Code;
	size_t Length;
	size_t m;
	size_t n;

	JsonElementSetUp(&Root);

	for (Character = 0x01; Character < 0x110000; Character++)
	{
		Utf16Code = JsonUtf16Code(Character);
		Length = JsonUtf16CodeGetNibbleLength(Utf16Code);

		for (m = 1; m < Length; m++)
		{
			JsonParseSetUp(&Parse, false, &Root);

			TEST_IS_EQ(JsonParse(&Parse, '"'), JSON_PARSE_INCOMPLETE, TestResult);

			for (n = 0; n < Length - m; n++)
			{
				if (n % 4 == 0)
				{
					TEST_IS_EQ(JsonParse(&Parse, '\\'), JSON_PARSE_INCOMPLETE, TestResult);
					TEST_IS_EQ(JsonParse(&Parse, 'u'), JSON_PARSE_INCOMPLETE, TestResult);
				}
				TEST_IS_EQ(JsonParse(&Parse, JsonCharacterFromHexDigit(JsonUtf16CodeGetNibble(Utf16Code, n))), JSON_PARSE_INCOMPLETE, TestResult);
			}
			TEST_IS_EQ(JsonParse(&Parse, '"'), JSON_PARSE_ERROR, TestResult);

			JsonParseCleanUp(&Parse);
		}
	}

	for (n = 0xD800; n < 0xDC00; n++)
	{
		JsonParseSetUp(&Parse, false, &Root);

		TEST_IS_EQ(JsonParse(&Parse, '"'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParse(&Parse, '\\'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParse(&Parse, 'u'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParse(&Parse, JsonCharacterFromHexDigit((n >> 12) & 0x0F)), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParse(&Parse, JsonCharacterFromHexDigit((n >> 8) & 0x0F)), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParse(&Parse, JsonCharacterFromHexDigit((n >> 4) & 0x0F)), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParse(&Parse, JsonCharacterFromHexDigit(n & 0x0F)), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParse(&Parse, '"'), JSON_PARSE_ERROR, TestResult);

		JsonParseCleanUp(&Parse);
	}

	for (n = 0xDC00; n < 0xE000; n++)
	{
		JsonParseSetUp(&Parse, false, &Root);

		TEST_IS_EQ(JsonParse(&Parse, '"'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParse(&Parse, '\\'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParse(&Parse, 'u'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParse(&Parse, JsonCharacterFromHexDigit((n >> 12) & 0x0F)), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParse(&Parse, JsonCharacterFromHexDigit((n >> 8) & 0x0F)), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParse(&Parse, JsonCharacterFromHexDigit((n >> 4) & 0x0F)), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParse(&Parse, JsonCharacterFromHexDigit(n & 0x0F)), JSON_PARSE_ERROR, TestResult);

		JsonParseCleanUp(&Parse);
	}

	for (n = 0; n < 0x10; n++)
	{
		if (n != 0x0D)
		{
			JsonParseSetUp(&Parse, false, &Root);

			TEST_IS_EQ(JsonParse(&Parse, '"'), JSON_PARSE_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonParse(&Parse, '\\'), JSON_PARSE_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonParse(&Parse, 'u'), JSON_PARSE_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonParse(&Parse, 'D'), JSON_PARSE_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonParse(&Parse, '8'), JSON_PARSE_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonParse(&Parse, '0'), JSON_PARSE_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonParse(&Parse, '0'), JSON_PARSE_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonParse(&Parse, '\\'), JSON_PARSE_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonParse(&Parse, 'u'), JSON_PARSE_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonParse(&Parse, JsonCharacterFromHexDigit(n)), JSON_PARSE_ERROR, TestResult);

			JsonParseCleanUp(&Parse);
		}
	}

	for (n = 0; n < 0x0C; n++)
	{
		JsonParseSetUp(&Parse, false, &Root);

		TEST_IS_EQ(JsonParse(&Parse, '"'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParse(&Parse, '\\'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParse(&Parse, 'u'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParse(&Parse, 'D'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParse(&Parse, '8'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParse(&Parse, '0'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParse(&Parse, '0'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParse(&Parse, '\\'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParse(&Parse, 'u'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParse(&Parse, 'D'), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParse(&Parse, JsonCharacterFromHexDigit(n)), JSON_PARSE_ERROR, TestResult);

		JsonParseCleanUp(&Parse);
	}

	return TestResult;
}


static const tTestCase TestCaseJsonValueUtf16[] =
{
   { "Valid",   TestJsonValueUtf16Valid  },
	{ "Invalid", TestJsonValueUtf8Invalid }
};


const tTest TestJsonValueUtf16 =
{
    "JsonValueUtf16",
    TestCaseJsonValueUtf16,
    sizeof(TestCaseJsonValueUtf16) / sizeof(TestCaseJsonValueUtf16[0])
};
