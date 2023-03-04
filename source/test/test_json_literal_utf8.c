#include "json_parse.h"
#include "json_format.h"
#include "test_json.h"


static tTestResult TestJsonLiteralUtf8Valid(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonElement Root;
	tJsonParse Parse;
	tJsonFormat Format;
	tJsonCharacter Character;
	tJsonUtf8Code Code;
	tJsonUtf8Unit CodeUnit;
	size_t Length;
	size_t n;

	JsonElementSetUp(&Root);

	JsonParseSetUp(&Parse, false, &Root);

	for (Character = 0x20; Character < 0x80; Character++)
	{
		if (JsonCharacterIsLiteral(Character))
		{
			Code = JsonUtf8Code(Character);
			Length = JsonUtf8CodeGetUnitLength(Code);
			for (n = 0; n < Length; n++)
			{
				TEST_IS_EQ(JsonParse(&Parse, JsonUtf8CodeGetUnit(Code, n)), JSON_PARSE_INCOMPLETE, TestResult);
			}
		}
	}

	TEST_IS_EQ(JsonParse(&Parse, '\0'), JSON_PARSE_COMPLETE, TestResult);

	JsonParseCleanUp(&Parse);

	JsonFormatSetUpSpace(&Format, &Root);

	for (Character = 0x20; Character < 0x80; Character++)
	{
		if (JsonCharacterIsLiteral(Character))
		{
			Code = JsonUtf8Code(Character);
			Length = JsonUtf8CodeGetUnitLength(Code);
			for (n = 0; n < Length; n++)
			{
				TEST_IS_EQ(JsonFormat(&Format, &CodeUnit), JSON_PARSE_INCOMPLETE, TestResult);
				TEST_IS_EQ(CodeUnit, JsonUtf8CodeGetUnit(Code, n), TestResult);
			}
		}
	}

	TEST_IS_EQ(JsonFormat(&Format, &CodeUnit), JSON_PARSE_COMPLETE, TestResult);
	TEST_IS_EQ(CodeUnit, '\0', TestResult);

	JsonFormatCleanUp(&Format);

	JsonElementCleanUp(&Root);

	for (Character = 0x20; Character < 0x110000; Character++)
	{
		if (!JsonCharacterIsLiteral(Character) && (Character != ' ') && (Character != '"') && (Character != '[') && (Character != '{') && (Character != '/'))
		{
			JsonElementSetUp(&Root);
			JsonParseSetUp(&Parse, false, &Root);

			Code = JsonUtf8Code(Character);
			Length = JsonUtf8CodeGetUnitLength(Code);

			for (n = 0; n < Length - 1; n++)
			{
				TEST_IS_EQ(JsonParse(&Parse, JsonUtf8CodeGetUnit(Code, n)), JSON_PARSE_INCOMPLETE, TestResult);
			}

			TEST_IS_EQ(JsonParse(&Parse, JsonUtf8CodeGetUnit(Code, n)), JSON_PARSE_ERROR, TestResult);

			JsonParseCleanUp(&Parse);
			JsonElementCleanUp(&Root);
		}
	}

	return TestResult;
}


static tTestResult TestJsonLiteralUtf8Invalid(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonElement Root;
	tJsonParse Parse;
	tJsonCharacter Character;
	tJsonUtf8Code Code;
	tJsonUtf8Unit InvalidUnit;
	size_t Length;
	size_t n;

	for (Character = 0x80; Character < 0x110000; Character++)
	{
		Code = JsonUtf8Code(Character);
		Length = JsonUtf8CodeGetUnitLength(Code);

		for (InvalidUnit = 0x00; InvalidUnit < 0x80; InvalidUnit++)
		{
			JsonElementSetUp(&Root);
			JsonParseSetUp(&Parse, false, &Root);

			for (n = 0; n < Length - 1; n++)
			{
				TEST_IS_EQ(JsonParse(&Parse, JsonUtf8CodeGetUnit(Code, n)), JSON_PARSE_INCOMPLETE, TestResult);
			}

			TEST_IS_EQ(JsonParse(&Parse, InvalidUnit), JSON_PARSE_ERROR, TestResult);

			JsonParseCleanUp(&Parse);
			JsonElementCleanUp(&Root);
		}

		for (InvalidUnit = 0xC0; InvalidUnit != 0x00; InvalidUnit++)
		{
			JsonElementSetUp(&Root);
			JsonParseSetUp(&Parse, false, &Root);

			for (n = 0; n < Length - 1; n++)
			{
				TEST_IS_EQ(JsonParse(&Parse, JsonUtf8CodeGetUnit(Code, n)), JSON_PARSE_INCOMPLETE, TestResult);
			}

			TEST_IS_EQ(JsonParse(&Parse, InvalidUnit), JSON_PARSE_ERROR, TestResult);

			JsonParseCleanUp(&Parse);
			JsonElementCleanUp(&Root);
		}
	}

	return TestResult;
}


static tTestResult TestJsonLiteralUtf8OutOfRange(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonElement Root;
	tJsonParse Parse;
	tJsonCharacter Character;

	for (Character = 0xD800; Character < 0xE000; Character = Character + 0x40)
	{
		JsonElementSetUp(&Root);
		JsonParseSetUp(&Parse, false, &Root);

		TEST_IS_EQ(JsonParse(&Parse, 0xE0 + (Character >> 12)), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParse(&Parse, 0x80 + ((Character >> 6) & 0x3F)), JSON_PARSE_ERROR, TestResult);

		JsonParseCleanUp(&Parse);
		JsonElementCleanUp(&Root);
	}

	for (Character = 0x110000; Character < 0x140000; Character = Character + 0x1000)
	{
		JsonElementSetUp(&Root);
		JsonParseSetUp(&Parse, false, &Root);

		TEST_IS_EQ(JsonParse(&Parse, 0xF0 + (Character >> 18)), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParse(&Parse, 0x80 + ((Character >> 12) & 0x3F)), JSON_PARSE_ERROR, TestResult);

		JsonParseCleanUp(&Parse);
		JsonElementCleanUp(&Root);
	}

	for (Character = 0x140000; Character < 0x200000; Character = Character + 0x40000)
	{
		JsonElementSetUp(&Root);
		JsonParseSetUp(&Parse, false, &Root);

		TEST_IS_EQ(JsonParse(&Parse, 0xF0 + (Character >> 18)), JSON_PARSE_ERROR, TestResult);

		JsonParseCleanUp(&Parse);
		JsonElementCleanUp(&Root);
	}

	return TestResult;
}


static tTestResult TestJsonLiteralUtf8Overlong(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonElement Root;
	tJsonParse Parse;
	tJsonCharacter Character;

	for (Character = 0; Character < 0x80; Character = Character + 0x40)
	{
		JsonElementSetUp(&Root);
		JsonParseSetUp(&Parse, false, &Root);

		TEST_IS_EQ(JsonParse(&Parse, 0xC0 + (Character >> 6)), JSON_PARSE_ERROR, TestResult);

		JsonParseCleanUp(&Parse);
		JsonElementCleanUp(&Root);
	}

	for (Character = 0; Character < 0x800; Character = Character + 0x40)
	{
		JsonElementSetUp(&Root);
		JsonParseSetUp(&Parse, false, &Root);

		TEST_IS_EQ(JsonParse(&Parse, 0xE0 + (Character >> 12)), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParse(&Parse, 0x80 + ((Character >> 6) & 0x3F)), JSON_PARSE_ERROR, TestResult);

		JsonParseCleanUp(&Parse);
		JsonElementCleanUp(&Root);
	}

	for (Character = 0; Character < 0x10000; Character = Character + 0x1000)
	{
		JsonElementSetUp(&Root);
		JsonParseSetUp(&Parse, false, &Root);

		TEST_IS_EQ(JsonParse(&Parse, 0xF0 + (Character >> 18)), JSON_PARSE_INCOMPLETE, TestResult);
		TEST_IS_EQ(JsonParse(&Parse, 0x80 + ((Character >> 12) & 0x3F)), JSON_PARSE_ERROR, TestResult);

		JsonParseCleanUp(&Parse);
		JsonElementCleanUp(&Root);
	}

	return TestResult;
}


static const tTestCase TestCaseJsonLiteralUtf8[] =
{
   { "Valid",      TestJsonLiteralUtf8Valid      },
	{ "Invalid",    TestJsonLiteralUtf8Invalid    },
	{ "OutOfRange", TestJsonLiteralUtf8OutOfRange },
	{ "Overlong",   TestJsonLiteralUtf8Overlong   }
};


const tTest TestJsonLiteralUtf8 =
{
    "JsonLiteralUtf8",
    TestCaseJsonLiteralUtf8,
    sizeof(TestCaseJsonLiteralUtf8) / sizeof(TestCaseJsonLiteralUtf8[0])
};
