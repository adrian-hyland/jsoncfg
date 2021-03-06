#include "json_string.h"
#include "test_json.h"


static int TestJsonStringSetUp(void)
{
    tJsonString String;
    int ok;

    JsonStringSetUp(&String);

    ok = (JsonStringGetLength(&String) == 0);

    ok = ok && (JsonStringGetCharacter(&String, 0) == '\0');

    JsonStringCleanUp(&String);

    return ok;
}


static int TestJsonStringCleanUp(void)
{
    tJsonString String;
    int ok;

    JsonStringSetUp(&String);

    ok = JsonStringAddCharacter(&String, 'a');

    ok = ok && (JsonStringGetLength(&String) == 1);

    ok = ok && (JsonStringGetCharacter(&String, 0) == 'a');

    JsonStringCleanUp(&String);

    ok = ok && (JsonStringGetLength(&String) == 0);

    ok = ok && (JsonStringGetCharacter(&String, 0) == '\0');

    return ok;
}


static int TestJsonStringClear(void)
{
    tJsonString String;
    int ok;

    JsonStringSetUp(&String);

    ok = JsonStringAddCharacter(&String, 'a');

    ok = ok && (JsonStringGetLength(&String) == 1);

    ok = ok && (JsonStringGetCharacter(&String, 0) == 'a');

    JsonStringClear(&String);

    ok = ok && (JsonStringGetLength(&String) == 0);

    ok = ok && (JsonStringGetCharacter(&String, 0) == '\0');

    JsonStringCleanUp(&String);

    return ok;
}


static int TestJsonStringGetLength(void)
{
    tJsonString String;
    uint8_t Character;
    int ok;

    JsonStringSetUp(&String);

    for (ok = 1, Character = 0; ok && (Character < UINT8_MAX); Character++)
    {
        ok = JsonStringAddCharacter(&String, Character + 1);

        ok = ok && (JsonStringGetLength(&String) == Character + 1);
    }

    JsonStringCleanUp(&String);

    return ok;
}


static int TestJsonStringAddCharacter(void)
{
    tJsonString String;
    uint8_t Character;
    int ok;

    JsonStringSetUp(&String);

    for (ok = 1, Character = 0; ok && (Character < UINT8_MAX); Character++)
    {
        ok = JsonStringAddCharacter(&String, Character + 1);
    }

    ok = ok && (JsonStringGetLength(&String) == Character);

    for (Character = 0; ok && (Character < UINT8_MAX); Character++)
    {
        ok = ok && (JsonStringGetCharacter(&String, Character) == Character + 1);
    }

    JsonStringCleanUp(&String);

    return ok;
}


static int TestJsonStringGetCharacter(void)
{
    tJsonString String;
    uint8_t Character;
    int ok;

    JsonStringSetUp(&String);

    for (ok = 1, Character = 0; ok && (Character < UINT8_MAX); Character++)
    {
        ok = (JsonStringGetCharacter(&String, Character) == '\0');

        ok = ok && JsonStringAddCharacter(&String, Character + 1);

        ok = ok && (JsonStringGetCharacter(&String, Character) == Character + 1);
    }

    JsonStringCleanUp(&String);

    return ok;
}


static const tTestCase TestCaseJsonString[] =
{
    { "JsonStringSetUp",        TestJsonStringSetUp        },
    { "JsonStringCleanUp",      TestJsonStringCleanUp      },
    { "JsonStringClear",        TestJsonStringClear        },
    { "JsonStringGetLength",    TestJsonStringGetLength    },
    { "JsonStringAddCharacter", TestJsonStringAddCharacter },
    { "JsonStringGetCharacter", TestJsonStringGetCharacter }
};


const tTest TestJsonString =
{
    "JsonString",
    TestCaseJsonString,
    sizeof(TestCaseJsonString) / sizeof(TestCaseJsonString[0])
};
