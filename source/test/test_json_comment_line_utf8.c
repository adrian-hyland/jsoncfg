#include "json_parse.h"
#include "json_format.h"
#include "test_json.h"


static bool TestJsonCommentLineUtf8Valid(void)
{
	tJsonElement Root;
	tJsonParse Parse;
	tJsonFormat Format;
	tJsonCharacter Character;
	tJsonUtf8Code Code;
	tJsonUtf8Unit CodeUnit;
	size_t Length;
	size_t n;
	bool ok;

	JsonElementSetUp(&Root);

	JsonParseSetUp(&Parse, false, &Root);

	ok = (JsonParse(&Parse, '/') == JSON_PARSE_INCOMPLETE) && (JsonParse(&Parse, '/') == JSON_PARSE_INCOMPLETE);

	for (Character = 0x20; ok && (Character < 0x110000); Character++)
	{
		Code = JsonUtf8Code(Character);
		Length = JsonUtf8CodeGetUnitLength(Code);
		for (n = 0; ok && (n < Length); n++)
		{
			ok = (JsonParse(&Parse, JsonUtf8CodeGetUnit(Code, n)) == JSON_PARSE_INCOMPLETE);
		}
	}

	ok = ok && (JsonParse(&Parse, '\n') == JSON_PARSE_INCOMPLETE);
	ok = ok && (JsonParse(&Parse, '{') == JSON_PARSE_INCOMPLETE) && (JsonParse(&Parse, '}') == JSON_PARSE_INCOMPLETE);
	ok = ok && (JsonParse(&Parse, '\0') == JSON_PARSE_COMPLETE);

	JsonParseCleanUp(&Parse);

	JsonFormatSetUpIndent(&Format, 0, json_CommentLine, &Root);

	ok = ok && (JsonFormat(&Format, &CodeUnit) == JSON_PARSE_INCOMPLETE) && (CodeUnit == '/');
	ok = ok && (JsonFormat(&Format, &CodeUnit) == JSON_PARSE_INCOMPLETE) && (CodeUnit == '/');

	for (Character = 0x20; ok && (Character < 0x110000); Character++)
	{
		Code = JsonUtf8Code(Character);
		Length = JsonUtf8CodeGetUnitLength(Code);
		for (n = 0; ok && (n < Length); n++)
		{
			ok = (JsonFormat(&Format, &CodeUnit) == JSON_PARSE_INCOMPLETE) && (CodeUnit == JsonUtf8CodeGetUnit(Code, n));
		}
	}

	ok = ok && (JsonFormat(&Format, &CodeUnit) == JSON_PARSE_INCOMPLETE) && (CodeUnit == '\n');
	ok = ok && (JsonFormat(&Format, &CodeUnit) == JSON_PARSE_INCOMPLETE) && (CodeUnit == '{');
	ok = ok && (JsonFormat(&Format, &CodeUnit) == JSON_PARSE_INCOMPLETE) && (CodeUnit == '}');
	ok = ok && (JsonFormat(&Format, &CodeUnit) == JSON_PARSE_COMPLETE) && (CodeUnit == '\0');

	JsonFormatCleanUp(&Format);
	
	JsonElementCleanUp(&Root);

	return ok;
}


static bool TestJsonCommentLineUtf8Invalid(void)
{
	tJsonElement Root;
	tJsonParse Parse;
	tJsonCharacter Character;
	tJsonUtf8Code Code;
	tJsonUtf8Unit InvalidUnit;
	size_t Length;
	size_t n;
	bool ok = true;

	for (Character = 0x80; ok && (Character < 0x110000); Character++)
	{
		Code = JsonUtf8Code(Character);
		Length = JsonUtf8CodeGetUnitLength(Code);

		for (InvalidUnit = 0x00; ok && (InvalidUnit < 0x80); InvalidUnit++)
		{
			JsonElementSetUp(&Root);
			JsonParseSetUp(&Parse, false, &Root);

			ok = (JsonParse(&Parse, '/') == JSON_PARSE_INCOMPLETE) && (JsonParse(&Parse, '/') == JSON_PARSE_INCOMPLETE);
			
			for (n = 0; ok && (n < Length - 1); n++)
			{
				ok = (JsonParse(&Parse, JsonUtf8CodeGetUnit(Code, n)) == JSON_PARSE_INCOMPLETE);
			}

			ok = ok && (JsonParse(&Parse, InvalidUnit) == JSON_PARSE_ERROR);

			JsonParseCleanUp(&Parse);
			JsonElementCleanUp(&Root);
		}

		for (InvalidUnit = 0xC0; ok && (InvalidUnit != 0x00); InvalidUnit++)
		{
			JsonElementSetUp(&Root);
			JsonParseSetUp(&Parse, false, &Root);

			ok = (JsonParse(&Parse, '/') == JSON_PARSE_INCOMPLETE) && (JsonParse(&Parse, '/') == JSON_PARSE_INCOMPLETE);

			for (n = 0; ok && (n < Length - 1); n++)
			{
				ok = (JsonParse(&Parse, JsonUtf8CodeGetUnit(Code, n)) == JSON_PARSE_INCOMPLETE);
			}

			ok = ok && (JsonParse(&Parse, InvalidUnit) == JSON_PARSE_ERROR);

			JsonParseCleanUp(&Parse);
			JsonElementCleanUp(&Root);
		}
	}

	return ok;
}


static bool TestJsonCommentLineUtf8OutOfRange(void)
{
	tJsonElement Root;
	tJsonParse Parse;
	tJsonCharacter Character;
	bool ok = true;

	for (Character = 0xD800; ok && (Character < 0xE000); Character = Character + 0x40)
	{
		JsonElementSetUp(&Root);
		JsonParseSetUp(&Parse, false, &Root);

		ok = (JsonParse(&Parse, '/') == JSON_PARSE_INCOMPLETE) && (JsonParse(&Parse, '/') == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0xE0 + (Character >> 12)) == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0x80 + ((Character >> 6) & 0x3F)) == JSON_PARSE_ERROR);

		JsonParseCleanUp(&Parse);
		JsonElementCleanUp(&Root);
	}

	for (Character = 0x110000; ok && (Character < 0x140000); Character = Character + 0x1000)
	{
		JsonElementSetUp(&Root);
		JsonParseSetUp(&Parse, false, &Root);

		ok = (JsonParse(&Parse, '/') == JSON_PARSE_INCOMPLETE) && (JsonParse(&Parse, '/') == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0xF0 + (Character >> 18)) == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0x80 + ((Character >> 12) & 0x3F)) == JSON_PARSE_ERROR);

		JsonParseCleanUp(&Parse);
		JsonElementCleanUp(&Root);
	}

	for (Character = 0x140000; ok && (Character < 0x200000); Character = Character + 0x40000)
	{
		JsonElementSetUp(&Root);
		JsonParseSetUp(&Parse, false, &Root);

		ok = (JsonParse(&Parse, '/') == JSON_PARSE_INCOMPLETE) && (JsonParse(&Parse, '/') == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0xF0 + (Character >> 18)) == JSON_PARSE_ERROR);

		JsonParseCleanUp(&Parse);
		JsonElementCleanUp(&Root);
	}

	return ok;
}


static bool TestJsonCommentLineUtf8Overlong(void)
{
	tJsonElement Root;
	tJsonParse Parse;
	tJsonCharacter Character;
	bool ok = true;

	for (Character = 0; ok && (Character < 0x80); Character = Character + 0x40)
	{
		JsonElementSetUp(&Root);
		JsonParseSetUp(&Parse, false, &Root);

		ok = (JsonParse(&Parse, '/') == JSON_PARSE_INCOMPLETE) && (JsonParse(&Parse, '/') == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0xC0 + (Character >> 6)) == JSON_PARSE_ERROR);

		JsonParseCleanUp(&Parse);
		JsonElementCleanUp(&Root);
	}

	for (Character = 0; ok && (Character < 0x800); Character = Character + 0x40)
	{
		JsonElementSetUp(&Root);
		JsonParseSetUp(&Parse, false, &Root);

		ok = (JsonParse(&Parse, '/') == JSON_PARSE_INCOMPLETE) && (JsonParse(&Parse, '/') == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0xE0 + (Character >> 12)) == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0x80 + ((Character >> 6) & 0x3F)) == JSON_PARSE_ERROR);

		JsonParseCleanUp(&Parse);
		JsonElementCleanUp(&Root);
	}

	for (Character = 0; ok && (Character < 0x10000); Character = Character + 0x1000)
	{
		JsonElementSetUp(&Root);
		JsonParseSetUp(&Parse, false, &Root);

		ok = (JsonParse(&Parse, '/') == JSON_PARSE_INCOMPLETE) && (JsonParse(&Parse, '/') == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0xF0 + (Character >> 18)) == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0x80 + ((Character >> 12) & 0x3F)) == JSON_PARSE_ERROR);

		JsonParseCleanUp(&Parse);
		JsonElementCleanUp(&Root);
	}

	return ok;
}


static const tTestCase TestCaseJsonCommentLineUtf8[] =
{
   { "Valid",      TestJsonCommentLineUtf8Valid      },
	{ "Invalid",    TestJsonCommentLineUtf8Invalid    },
	{ "OutOfRange", TestJsonCommentLineUtf8OutOfRange },
	{ "Overlong",   TestJsonCommentLineUtf8Overlong   }
};


const tTest TestJsonCommentLineUtf8 =
{
    "JsonCommentLineUtf8",
    TestCaseJsonCommentLineUtf8,
    sizeof(TestCaseJsonCommentLineUtf8) / sizeof(TestCaseJsonCommentLineUtf8[0])
};
