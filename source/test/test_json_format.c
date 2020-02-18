#include "json_parse.h"
#include "json_format.h"
#include "test_json.h"


static int TestJsonFormatCompressContent(const uint8_t *Content)
{
    tJsonElement Root;
    tJsonParse Parse;
    tJsonFormat Format;
    size_t Index;
    uint8_t Character;
    int ok;

    JsonElementSetUp(&Root);

    JsonParseSetUp(&Parse, &Root);

    for (ok = 1, Index = 0; ok && (Content[Index] != '\0'); Index++)
    {
        ok = (JsonParse(&Parse, Content[Index]) == JSON_PARSE_INCOMPLETE);
    }

    ok = ok && (JsonParse(&Parse, Content[Index]) == JSON_PARSE_COMPLETE);

    JsonParseCleanUp(&Parse);

    JsonFormatSetUpCompress(&Format, &Root);

    for (Index = 0; ok && (Content[Index] != 0); Index++)
    {
        ok = (JsonFormat(&Format, &Character) == JSON_FORMAT_INCOMPLETE);

        ok = ok && (Character == Content[Index]);
    }

    ok = ok && (JsonFormat(&Format, &Character) == JSON_FORMAT_COMPLETE);

    ok = ok && (Character == Content[Index]);

    JsonFormatCleanUp(&Format);

    JsonElementCleanUp(&Root);

    return ok;
}


static int TestJsonFormatSpaceContent(const uint8_t *Content)
{
    tJsonElement Root;
    tJsonParse Parse;
    tJsonFormat Format;
    size_t Index;
    uint8_t Character;
    int ok;

    JsonElementSetUp(&Root);

    JsonParseSetUp(&Parse, &Root);

    for (ok = 1, Index = 0; ok && (Content[Index] != '\0'); Index++)
    {
        ok = (JsonParse(&Parse, Content[Index]) == JSON_PARSE_INCOMPLETE);
    }

    ok = ok && (JsonParse(&Parse, Content[Index]) == JSON_PARSE_COMPLETE);

    JsonParseCleanUp(&Parse);

    JsonFormatSetUpSpace(&Format, &Root);

    for (Index = 0; ok && (Content[Index] != 0); Index++)
    {
        ok = (JsonFormat(&Format, &Character) == JSON_FORMAT_INCOMPLETE);

        ok = ok && (Character == Content[Index]);
    }

    ok = ok && (JsonFormat(&Format, &Character) == JSON_FORMAT_COMPLETE);

    ok = ok && (Character == Content[Index]);

    JsonFormatCleanUp(&Format);

    JsonElementCleanUp(&Root);

    return ok;
}


static int TestJsonFormatIndentContent(const uint8_t *Content, size_t IndentSize)
{
    tJsonElement Root;
    tJsonParse Parse;
    tJsonFormat Format;
    size_t Index;
    uint8_t Character;
    int ok;

    JsonElementSetUp(&Root);

    JsonParseSetUp(&Parse, &Root);

    for (ok = 1, Index = 0; ok && (Content[Index] != '\0'); Index++)
    {
        ok = (JsonParse(&Parse, Content[Index]) == JSON_PARSE_INCOMPLETE);
    }

    ok = ok && (JsonParse(&Parse, Content[Index]) == JSON_PARSE_COMPLETE);

    JsonParseCleanUp(&Parse);

    JsonFormatSetUpIndent(&Format, IndentSize, &Root);

    for (Index = 0; ok && (Content[Index] != 0); Index++)
    {
        ok = (JsonFormat(&Format, &Character) == JSON_FORMAT_INCOMPLETE);

        ok = ok && (Character == Content[Index]);
    }

    ok = ok && (JsonFormat(&Format, &Character) == JSON_FORMAT_COMPLETE);

    ok = ok && (Character == Content[Index]);

    JsonFormatCleanUp(&Format);

    JsonElementCleanUp(&Root);

    return ok;
}


int TestJsonFormatCompress(void)
{
    static const uint8_t *Content[] =
    {
        (const uint8_t *)"{"
                            "\"key-true\":true,"
                            "\"key-false\":false,"
                            "\"key-null\":null,"
                            "\"key-int\":123,"
                            "\"key-real\":1.23e4,"
                            "\"key-real-nan\":NaN,"
                            "\"key-string\":\"hello world\","
                            "\"key-string-escape\":\"\\t\\r\\n\\b\\f\\\\\\\"\","
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
                                "\"\\t\\r\\n\\b\\f\\\\\\\"\","
                                "{},"
                                "{\"key\":value},"
                                "[],"
                                "[1,2,3]"
                            "]"
                        "}",
        (const uint8_t *)"["
                            "true,"
                            "false,"
                            "null,"
                            "123,"
                            "1.23e4,"
                            "NaN,"
                            "\"hello world\","
                            "\"\\t\\r\\n\\b\\f\\\\\\\"\","
                            "{},"
                            "{\"key\":value},"
                            "[],"
                            "[1,2,3]"
                        "]",
        (const uint8_t *)"\"string:{}[]\\t\\r\\n\\b\\f\\\\\\\"\"",
        (const uint8_t *)"true",
        (const uint8_t *)"false",
        (const uint8_t *)"null",
        (const uint8_t *)"1234567890",
        (const uint8_t *)"1.234567890e-99"
    };
    size_t n;
    int ok;

    for (ok = 1, n = 0; ok && (n < sizeof(Content) / sizeof(Content[0])); n++)
    {
        ok = TestJsonFormatCompressContent(Content[n]);
    }

    return ok;
}


int TestJsonFormatSpace(void)
{
    static const uint8_t *Content[] =
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
                            "[ 1, 2, 3 ] "
                        "]",
        (const uint8_t *)"\"string:{}[]\\t\\r\\n\\b\\f\\\\\\\"\"",
        (const uint8_t *)"true",
        (const uint8_t *)"false",
        (const uint8_t *)"null",
        (const uint8_t *)"1234567890",
        (const uint8_t *)"1.234567890e-99"
    };
    size_t n;
    int ok;

    for (ok = 1, n = 0; ok && (n < sizeof(Content) / sizeof(Content[0])); n++)
    {
        ok = TestJsonFormatSpaceContent(Content[n]);
    }

    return ok;
}


int TestJsonFormatIndent(void)
{
    static const uint8_t *Content[] =
    {
        (const uint8_t *)"{\n"
                         "   \"key-true\": true,\n"
                         "   \"key-false\": false,\n"
                         "   \"key-null\": null,\n"
                         "   \"key-int\": 123,\n"
                         "   \"key-real\": 1.23e4,\n"
                         "   \"key-real-nan\": NaN,\n"
                         "   \"key-string\": \"hello world\",\n"
                         "   \"key-string-escape\": \"\\t\\r\\n\\b\\f\\\\\\\"\",\n"
                         "   \"key-object-empty\": {},\n"
                         "   \"key-object\": {\n"
                         "      \"key\": value\n"
                         "   },\n"
                         "   \"key-array-empty\": [],\n"
                         "   \"key-array\": [ true, false, null, 123, 1.23e4, NaN, \"hello world\", \"\\t\\r\\n\\b\\f\\\\\\\"\",\n"
                         "      {},\n"
                         "      {\n"
                         "         \"key\": value\n"
                         "      },\n"
                         "      [], [ 1, 2, 3 ] ]\n"
                         "}",
        (const uint8_t *)"[ true, false, null, 123, 1.23e4, NaN, \"hello world\", \"\\t\\r\\n\\b\\f\\\\\\\"\",\n"
                         "   {},\n"
                         "   {\n"
                         "      \"key\": value\n"
                         "   },\n"
                         "   [], [ 1, 2, 3 ] ]",
        (const uint8_t *)"\"string:{}[]\\t\\r\\n\\b\\f\\\\\\\"\"",
        (const uint8_t *)"true",
        (const uint8_t *)"false",
        (const uint8_t *)"null",
        (const uint8_t *)"1234567890",
        (const uint8_t *)"1.234567890e-99"
    };
    size_t n;
    int ok;

    for (ok = 1, n = 0; ok && (n < sizeof(Content) / sizeof(Content[0])); n++)
    {
        ok = TestJsonFormatIndentContent(Content[n], 3);
    }

    return ok;
}


static const tTestCase TestCaseJsonFormat[] =
{
    { "JsonFormatCompress", TestJsonFormatCompress },
    { "JsonFormatSpace",    TestJsonFormatSpace    },
    { "JsonFormatIndent",   TestJsonFormatIndent   },
};


const tTest TestJsonFormat =
{
    "JsonFormat",
    TestCaseJsonFormat,
    sizeof(TestCaseJsonFormat) / sizeof(TestCaseJsonFormat[0])
};
