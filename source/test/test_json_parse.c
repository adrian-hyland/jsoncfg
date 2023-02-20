#include <string.h>
#include "json_character.h"
#include "json_parse.h"
#include "test_json.h"


static bool TestJsonParseComplete(const uint8_t *Content, bool StripComments)
{
	tJsonElement Root;
	tJsonParse Parse;
	bool ok;

	JsonElementSetUp(&Root);
	JsonParseSetUp(&Parse, StripComments, &Root);

	for (ok = true; ok && (*Content != '\0'); Content++)
	{
		ok = (JsonParse(&Parse, *Content) == JSON_PARSE_INCOMPLETE);
	}

	ok = ok && (JsonParse(&Parse, *Content) == JSON_PARSE_COMPLETE);

	JsonParseCleanUp(&Parse);
	JsonElementCleanUp(&Root);

	return ok;
}


static bool TestJsonParseIncomplete(const uint8_t *Content, bool StripComments)
{
	tJsonElement Root;
	tJsonParse Parse;
	size_t Length;
	size_t Index;
	bool ok;

	ok = true;
	Length = strlen((const char *)Content);

	while (ok && (Length > 1))
	{
		Length--;

		JsonElementSetUp(&Root);
		JsonParseSetUp(&Parse, StripComments, &Root);

		for (Index = 0; ok && (Index < Length); Index++)
		{
			ok = (JsonParse(&Parse, Content[Index]) == JSON_PARSE_INCOMPLETE);
		}

		ok = ok && (JsonParse(&Parse, '\0') == JSON_PARSE_ERROR);

		JsonParseCleanUp(&Parse);
		JsonElementCleanUp(&Root);
	}

	return ok;
}


static bool TestJsonParseError(const uint8_t *Content)
{
	tJsonElement Root;
	tJsonParse Parse;
	bool ok;

	JsonElementSetUp(&Root);
	JsonParseSetUp(&Parse, true, &Root);

	for (ok = true; ok && (Content[0] != '\0') && (Content[1] != '\0'); Content++)
	{
		ok = (JsonParse(&Parse, *Content) == JSON_PARSE_INCOMPLETE);
	}

	ok = ok && (JsonParse(&Parse, *Content) == JSON_PARSE_ERROR);

	JsonParseCleanUp(&Parse);
	JsonElementCleanUp(&Root);

	return ok;
}


static bool TestJsonParseContent(void)
{
	static const uint8_t *ValidContent[] =
	{
		(const uint8_t *)"{ "
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
		(const uint8_t *)"[ "
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
		(const uint8_t *)"\"string:{}[]\\t\\r\\n\\b\\f\\\\\\\"\"",
		(const uint8_t *)"true",
		(const uint8_t *)"false",
		(const uint8_t *)"null",
		(const uint8_t *)"1234567890",
		(const uint8_t *)"1.234567890e-99",
		(const uint8_t *)"// Comment 1\n"
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
	static const uint8_t *InvalidContent[] =
	{
		(const uint8_t *)"{ ,",
		(const uint8_t *)"{ 1",
		(const uint8_t *)"{ {",
		(const uint8_t *)"{ [",
		(const uint8_t *)"{ :",
		(const uint8_t *)"{ \"abc\" ,",
		(const uint8_t *)"{ \"abc\" 1",
		(const uint8_t *)"{ \"abc\" {",
		(const uint8_t *)"{ \"abc\" [",
		(const uint8_t *)"{ \"abc\" \"",
		(const uint8_t *)"[ ,",
		(const uint8_t *)"[ :",
		(const uint8_t *)"[ \"abc\" {",
		(const uint8_t *)"[ \"abc\" [",
		(const uint8_t *)"[ \"abc\" :",
		(const uint8_t *)"[ \"abc\" 1",
		(const uint8_t *)"[ \"abc\" \"",
		(const uint8_t *)"[ 123 {",
		(const uint8_t *)"[ 123 [",
		(const uint8_t *)"[ 123 :",
		(const uint8_t *)"[ 123 1",
		(const uint8_t *)"[ 123 \"",
		(const uint8_t *)"[ {} {",
		(const uint8_t *)"[ {} [",
		(const uint8_t *)"[ {} :",
		(const uint8_t *)"[ {} 1",
		(const uint8_t *)"[ {} \"",
		(const uint8_t *)"[ [] {",
		(const uint8_t *)"[ [] [",
		(const uint8_t *)"[ [] :",
		(const uint8_t *)"[ [] 1",
		(const uint8_t *)"[ [] \"",
		(const uint8_t *)"\"abc\" 1",
		(const uint8_t *)"\"abc\" {",
		(const uint8_t *)"\"abc\" [",
		(const uint8_t *)"\"abc\" :",
		(const uint8_t *)"\"abc\" \"",
		(const uint8_t *)"\"abc\" ,",
		(const uint8_t *)"123 1",
		(const uint8_t *)"123 {",
		(const uint8_t *)"123 [",
		(const uint8_t *)"123 :",
		(const uint8_t *)"123 \"",
		(const uint8_t *)"123 ,",
	};
	size_t n;
	bool ok;

	for (ok = true, n = 0; ok && (n < sizeof(ValidContent) / sizeof(ValidContent[0])); n++)
	{
		ok = TestJsonParseComplete(ValidContent[n], false);
		ok = ok && TestJsonParseComplete(ValidContent[n], true);
	}

	for (n = 0; ok && (n < sizeof(ValidContent) / sizeof(ValidContent[0])); n++)
	{
		if (!JsonCharacterIsLiteral(*ValidContent[n]) && (*ValidContent[n] != '/'))
		{
			ok = TestJsonParseIncomplete(ValidContent[n], false);
			ok = ok && TestJsonParseIncomplete(ValidContent[n], true);
		}
	}

	for (n = 0; ok && (n < sizeof(InvalidContent) / sizeof(InvalidContent[0])); n++)
	{
		ok = TestJsonParseError(InvalidContent[n]);
	}

	return ok;
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
