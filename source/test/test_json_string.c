#include "json_string.h"
#include "test_json.h"


static bool TestJsonStringSetUp(void)
{
	tJsonString String;
	uint8_t Character;
	bool ok;

	JsonStringSetUp(&String);

	ok = (JsonStringGetLength(&String) == 0);

	ok = ok && (JsonStringGetCharacter(&String, 0, &Character) == 0);
	
	ok = ok && (Character == '\0');

	JsonStringCleanUp(&String);

	return ok;
}


static bool TestJsonStringCleanUp(void)
{
	tJsonString String;
	uint8_t Character;
	bool ok;

	JsonStringSetUp(&String);

	ok = JsonStringAddCharacter(&String, 'a');

	ok = ok && (JsonStringGetLength(&String) == 1);

	ok = ok && (JsonStringGetCharacter(&String, 0, &Character) == 1);
	
	ok = ok && (Character == 'a');

	JsonStringCleanUp(&String);

	ok = ok && (JsonStringGetLength(&String) == 0);

	ok = ok && (JsonStringGetCharacter(&String, 0, &Character) == 0);
	
	ok = ok && (Character == '\0');

	return ok;
}


static bool TestJsonStringClear(void)
{
	tJsonString String;
	uint8_t Character;
	bool ok;

	JsonStringSetUp(&String);

	ok = JsonStringAddCharacter(&String, 'a');

	ok = ok && (JsonStringGetLength(&String) == 1);

	ok = ok && (JsonStringGetCharacter(&String, 0, &Character) == 1);
	
	ok = ok && (Character == 'a');

	JsonStringClear(&String);

	ok = ok && (JsonStringGetLength(&String) == 0);

	ok = ok && (JsonStringGetCharacter(&String, 0, &Character) == 0);
	
	ok = ok && (Character == '\0');

	JsonStringCleanUp(&String);

	return ok;
}


static bool TestJsonStringGetLength(void)
{
	tJsonString String;
	uint8_t Character;
	bool ok;

	JsonStringSetUp(&String);

	for (ok = true, Character = 0; ok && (Character < UINT8_MAX); Character++)
	{
		ok = JsonStringAddCharacter(&String, Character + 1);

		ok = ok && (JsonStringGetLength(&String) == Character + 1);
	}

	JsonStringCleanUp(&String);

	return ok;
}


static bool TestJsonStringAddCharacter(void)
{
	tJsonString String;
	uint8_t Character;
	uint8_t n;
	bool ok;

	JsonStringSetUp(&String);

	for (ok = true, n = 0; ok && (n < UINT8_MAX); n++)
	{
		ok = JsonStringAddCharacter(&String, n + 1);
	}

	ok = ok && (JsonStringGetLength(&String) == n);

	for (n = 0; ok && (n < UINT8_MAX); n++)
	{
		ok = ok && (JsonStringGetCharacter(&String, n, &Character) == 1);
		
		ok = ok && (Character == n + 1);
	}

	JsonStringCleanUp(&String);

	return ok;
}


static bool TestJsonStringGetCharacter(void)
{
	tJsonString String;
	uint8_t Character;
	uint8_t n;
	bool ok;

	JsonStringSetUp(&String);

	for (ok = true, n = 0; ok && (n < UINT8_MAX); n++)
	{
		ok = (JsonStringGetCharacter(&String, n, &Character) == 0);
		
		ok = ok && (Character == '\0');

		ok = ok && JsonStringAddCharacter(&String, n + 1);

		ok = ok && (JsonStringGetCharacter(&String, n, &Character) == 1);
		
		ok = ok && (Character == n + 1);
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
