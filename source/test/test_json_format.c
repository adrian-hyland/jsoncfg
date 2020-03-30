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

    JsonParseSetUp(&Parse, 0, &Root);

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

    JsonParseSetUp(&Parse, 0, &Root);

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


static int TestJsonFormatIndentContent(const uint8_t *Content, size_t IndentSize, tJsonCommentType CommentType)
{
    tJsonElement Root;
    tJsonParse Parse;
    tJsonFormat Format;
    size_t Index;
    uint8_t Character;
    int ok;

    JsonElementSetUp(&Root);

    JsonParseSetUp(&Parse, 0, &Root);

    for (ok = 1, Index = 0; ok && (Content[Index] != '\0'); Index++)
    {
        ok = (JsonParse(&Parse, Content[Index]) == JSON_PARSE_INCOMPLETE);
    }

    ok = ok && (JsonParse(&Parse, Content[Index]) == JSON_PARSE_COMPLETE);

    JsonParseCleanUp(&Parse);

    JsonFormatSetUpIndent(&Format, IndentSize, CommentType, &Root);

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


static int TestJsonFormatCompress(void)
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


static int TestJsonFormatSpace(void)
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
                                    "[ 1, 2, 3 ] "
                                "] "
                            "}",
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


static int TestJsonFormatIndent(void)
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
                         "   \"key-array\": [\n"
                         "      true,\n"
                         "      false,\n"
                         "      null,\n"
                         "      123,\n"
                         "      1.23e4,\n"
                         "      NaN,\n"
                         "      \"hello world\",\n"
                         "      \"\\t\\r\\n\\b\\f\\\\\\\"\",\n"
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
        (const uint8_t *)"[]",
        (const uint8_t *)"[\n"
                         "   1\n"
                         "]",
        (const uint8_t *)"[\n"
                         "   {}\n"
                         "]",
        (const uint8_t *)"[\n"
                         "   {\n"
                         "      \"key\": \"value\"\n"
                         "   }\n"
                         "]",
        (const uint8_t *)"[\n"
                         "   []\n"
                         "]",
        (const uint8_t *)"[\n"
                         "   [\n"
                         "      1\n"
                         "   ]\n"
                         "]",
        (const uint8_t *)"[\n"
                         "   [\n"
                         "      1,\n"
                         "      2\n"
                         "   ]\n"
                         "]",
        (const uint8_t *)"[\n"
                         "   true,\n"
                         "   false,\n"
                         "   null,\n"
                         "   123,\n"
                         "   1.23e4,\n"
                         "   NaN,\n"
                         "   \"hello world\",\n"
                         "   \"\\t\\r\\n\\b\\f\\\\\\\"\",\n"
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
        ok = TestJsonFormatIndentContent(Content[n], 3, json_CommentNone);
        ok = ok && TestJsonFormatIndentContent(Content[n], 3, json_CommentLine);
        ok = ok && TestJsonFormatIndentContent(Content[n], 3, json_CommentBlock);
    }

    return ok;
}


static int TestJsonFormatCommentLine(void)
{
    static const uint8_t Content[] =
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
    static const uint8_t ContentIndentCommentNone[] =
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
    static const uint8_t ContentIndentCommentLine[] =
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
    static const uint8_t ContentIndentCommentBlock[] =
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
    static const uint8_t ContentSpace[] =
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
    static const uint8_t ContentCompress[] =
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
    size_t Index;
    uint8_t Character;
    int ok;

    JsonElementSetUp(&Root);

    JsonParseSetUp(&Parse, 0, &Root);

    for (ok = 1, Index = 0; ok && (Content[Index] != '\0'); Index++)
    {
        ok = (JsonParse(&Parse, Content[Index]) == JSON_PARSE_INCOMPLETE);
    }

    ok = ok && (JsonParse(&Parse, Content[Index]) == JSON_PARSE_COMPLETE);

    JsonParseCleanUp(&Parse);

    JsonFormatSetUpIndent(&Format, 3, json_CommentNone, &Root);

    for (Index = 0; ok && (ContentIndentCommentNone[Index] != 0); Index++)
    {
        ok = (JsonFormat(&Format, &Character) == JSON_FORMAT_INCOMPLETE);

        ok = ok && (Character == ContentIndentCommentNone[Index]);
    }

    ok = ok && (JsonFormat(&Format, &Character) == JSON_FORMAT_COMPLETE);

    ok = ok && (Character == ContentIndentCommentNone[Index]);

    JsonFormatCleanUp(&Format);

    JsonFormatSetUpIndent(&Format, 3, json_CommentLine, &Root);

    for (Index = 0; ok && (ContentIndentCommentLine[Index] != 0); Index++)
    {
        ok = (JsonFormat(&Format, &Character) == JSON_FORMAT_INCOMPLETE);

        ok = ok && (Character == ContentIndentCommentLine[Index]);
    }

    ok = ok && (JsonFormat(&Format, &Character) == JSON_FORMAT_COMPLETE);

    ok = ok && (Character == ContentIndentCommentLine[Index]);

    JsonFormatCleanUp(&Format);

    JsonFormatSetUpIndent(&Format, 3, json_CommentBlock, &Root);

    for (Index = 0; ok && (ContentIndentCommentBlock[Index] != 0); Index++)
    {
        ok = (JsonFormat(&Format, &Character) == JSON_FORMAT_INCOMPLETE);

        ok = ok && (Character == ContentIndentCommentBlock[Index]);
    }

    ok = ok && (JsonFormat(&Format, &Character) == JSON_FORMAT_COMPLETE);

    ok = ok && (Character == ContentIndentCommentBlock[Index]);

    JsonFormatCleanUp(&Format);

    JsonFormatSetUpSpace(&Format, &Root);

    for (Index = 0; ok && (ContentSpace[Index] != 0); Index++)
    {
        ok = (JsonFormat(&Format, &Character) == JSON_FORMAT_INCOMPLETE);

        ok = ok && (Character == ContentSpace[Index]);
    }

    ok = ok && (JsonFormat(&Format, &Character) == JSON_FORMAT_COMPLETE);

    ok = ok && (Character == ContentSpace[Index]);

    JsonFormatCleanUp(&Format);

    JsonFormatSetUpCompress(&Format, &Root);

    for (Index = 0; ok && (ContentCompress[Index] != 0); Index++)
    {
        ok = (JsonFormat(&Format, &Character) == JSON_FORMAT_INCOMPLETE);

        ok = ok && (Character == ContentCompress[Index]);
    }

    ok = ok && (JsonFormat(&Format, &Character) == JSON_FORMAT_COMPLETE);

    ok = ok && (Character == ContentCompress[Index]);

    JsonFormatCleanUp(&Format);

    JsonElementCleanUp(&Root);

    return ok;
}


static int TestJsonFormatCommentBlock(void)
{
    static const uint8_t Content[] =
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
    static const uint8_t ContentIndentCommentNone[] =
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
    static const uint8_t ContentIndentCommentLine[] =
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
    static const uint8_t ContentIndentCommentBlock[] =
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
    static const uint8_t ContentSpace[] =
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
    static const uint8_t ContentCompress[] =
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
    size_t Index;
    uint8_t Character;
    int ok;

    JsonElementSetUp(&Root);

    JsonParseSetUp(&Parse, 0, &Root);

    for (ok = 1, Index = 0; ok && (Content[Index] != '\0'); Index++)
    {
        ok = (JsonParse(&Parse, Content[Index]) == JSON_PARSE_INCOMPLETE);
    }

    ok = ok && (JsonParse(&Parse, Content[Index]) == JSON_PARSE_COMPLETE);

    JsonParseCleanUp(&Parse);

    JsonFormatSetUpIndent(&Format, 3, json_CommentNone, &Root);

    for (Index = 0; ok && (ContentIndentCommentNone[Index] != 0); Index++)
    {
        ok = (JsonFormat(&Format, &Character) == JSON_FORMAT_INCOMPLETE);

        ok = ok && (Character == ContentIndentCommentNone[Index]);
    }

    ok = ok && (JsonFormat(&Format, &Character) == JSON_FORMAT_COMPLETE);

    ok = ok && (Character == ContentIndentCommentNone[Index]);

    JsonFormatCleanUp(&Format);

    JsonFormatSetUpIndent(&Format, 3, json_CommentLine, &Root);

    for (Index = 0; ok && (ContentIndentCommentLine[Index] != 0); Index++)
    {
        ok = (JsonFormat(&Format, &Character) == JSON_FORMAT_INCOMPLETE);

        ok = ok && (Character == ContentIndentCommentLine[Index]);
    }

    ok = ok && (JsonFormat(&Format, &Character) == JSON_FORMAT_COMPLETE);

    ok = ok && (Character == ContentIndentCommentLine[Index]);

    JsonFormatCleanUp(&Format);

    JsonFormatSetUpIndent(&Format, 3, json_CommentBlock, &Root);

    for (Index = 0; ok && (ContentIndentCommentBlock[Index] != 0); Index++)
    {
        ok = (JsonFormat(&Format, &Character) == JSON_FORMAT_INCOMPLETE);

        ok = ok && (Character == ContentIndentCommentBlock[Index]);
    }

    ok = ok && (JsonFormat(&Format, &Character) == JSON_FORMAT_COMPLETE);

    ok = ok && (Character == ContentIndentCommentBlock[Index]);

    JsonFormatCleanUp(&Format);

    JsonFormatSetUpSpace(&Format, &Root);

    for (Index = 0; ok && (ContentSpace[Index] != 0); Index++)
    {
        ok = (JsonFormat(&Format, &Character) == JSON_FORMAT_INCOMPLETE);

        ok = ok && (Character == ContentSpace[Index]);
    }

    ok = ok && (JsonFormat(&Format, &Character) == JSON_FORMAT_COMPLETE);

    ok = ok && (Character == ContentSpace[Index]);

    JsonFormatCleanUp(&Format);

    JsonFormatSetUpCompress(&Format, &Root);

    for (Index = 0; ok && (ContentCompress[Index] != 0); Index++)
    {
        ok = (JsonFormat(&Format, &Character) == JSON_FORMAT_INCOMPLETE);

        ok = ok && (Character == ContentCompress[Index]);
    }

    ok = ok && (JsonFormat(&Format, &Character) == JSON_FORMAT_COMPLETE);

    ok = ok && (Character == ContentCompress[Index]);

    JsonFormatCleanUp(&Format);

    JsonElementCleanUp(&Root);

    return ok;
}


static const tTestCase TestCaseJsonFormat[] =
{
    { "JsonFormatCompress",     TestJsonFormatCompress     },
    { "JsonFormatSpace",        TestJsonFormatSpace        },
    { "JsonFormatIndent",       TestJsonFormatIndent       },
    { "JsonFormatCommentLine",  TestJsonFormatCommentLine  },
    { "JsonFormatCommentBlock", TestJsonFormatCommentBlock }
};


const tTest TestJsonFormat =
{
    "JsonFormat",
    TestCaseJsonFormat,
    sizeof(TestCaseJsonFormat) / sizeof(TestCaseJsonFormat[0])
};
