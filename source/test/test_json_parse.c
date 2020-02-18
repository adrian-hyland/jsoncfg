#include <string.h>
#include "json_character.h"
#include "json_parse.h"
#include "test_json.h"


static int TestJsonParseComplete(const uint8_t *Content)
{
    tJsonElement Root;
    tJsonParse Parse;
    int ok;

    JsonElementSetUp(&Root);
    JsonParseSetUp(&Parse, &Root);

    for (ok = 1; ok && (*Content != '\0'); Content++)
    {
        ok = (JsonParse(&Parse, *Content) == JSON_PARSE_INCOMPLETE);
    }

    ok = ok && (JsonParse(&Parse, *Content) == JSON_PARSE_COMPLETE);

    JsonParseCleanUp(&Parse);
    JsonElementCleanUp(&Root);

    return ok;
}


static int TestJsonParseIncomplete(const uint8_t *Content)
{
    tJsonElement Root;
    tJsonParse Parse;
    size_t Length;
    size_t Index;
    int ok;

    ok = 1;
    Length = strlen((const char *)Content);

    while (ok && (Length > 1))
    {
        Length--;

        JsonElementSetUp(&Root);
        JsonParseSetUp(&Parse, &Root);

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


static int TestJsonParseError(const uint8_t *Content)
{
    tJsonElement Root;
    tJsonParse Parse;
    int ok;

    JsonElementSetUp(&Root);
    JsonParseSetUp(&Parse, &Root);

    for (ok = 1; ok && (Content[0] != '\0') && (Content[1] != '\0'); Content++)
    {
        ok = (JsonParse(&Parse, *Content) == JSON_PARSE_INCOMPLETE);
    }

    ok = ok && (JsonParse(&Parse, *Content) == JSON_PARSE_ERROR);

    JsonParseCleanUp(&Parse);
    JsonElementCleanUp(&Root);

    return ok;
}


static int TestJsonParseContent(void)
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
        (const uint8_t *)"1.234567890e-99"
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
    int ok;

    for (ok = 1, n = 0; ok && (n < sizeof(ValidContent) / sizeof(ValidContent[0])); n++)
    {
        ok = TestJsonParseComplete(ValidContent[n]);
    }

    for (n = 0; ok && (n < sizeof(ValidContent) / sizeof(ValidContent[0])); n++)
    {
        if (!JsonCharacterIsLiteral(*ValidContent[n]))
        {
            ok = TestJsonParseIncomplete(ValidContent[n]);
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
