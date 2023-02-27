#include "json_string.h"
#include "test_json.h"


static bool TestJsonStringSetUp(void)
{
	tJsonString String;
	tJsonUtf8Code Code;
	bool ok;

	JsonStringSetUp(&String);

	ok = (JsonStringGetLength(&String) == 0);

	ok = ok && (JsonStringGetNextUtf8Code(&String, 0, &Code) == 0);
	
	ok = ok && (Code == '\0');

	JsonStringCleanUp(&String);

	return ok;
}


static bool TestJsonStringCleanUp(void)
{
	tJsonString String;
	tJsonUtf8Code Code;
	bool ok;

	JsonStringSetUp(&String);

	ok = JsonStringAddUtf8Code(&String, 'a');

	ok = ok && (JsonStringGetLength(&String) == 1);

	ok = ok && (JsonStringGetNextUtf8Code(&String, 0, &Code) == 1);
	
	ok = ok && (Code == 'a');

	JsonStringCleanUp(&String);

	ok = ok && (JsonStringGetLength(&String) == 0);

	ok = ok && (JsonStringGetNextUtf8Code(&String, 0, &Code) == 0);
	
	ok = ok && (Code == '\0');

	return ok;
}


static bool TestJsonStringClear(void)
{
	tJsonString String;
	tJsonUtf8Code Code;
	bool ok;

	JsonStringSetUp(&String);

	ok = JsonStringAddCharacter(&String, 'a');

	ok = ok && (JsonStringGetLength(&String) == 1);

	ok = ok && (JsonStringGetNextUtf8Code(&String, 0, &Code) == 1);
	
	ok = ok && (Code == 'a');

	JsonStringClear(&String);

	ok = ok && (JsonStringGetLength(&String) == 0);

	ok = ok && (JsonStringGetNextUtf8Code(&String, 0, &Code) == 0);
	
	ok = ok && (Code == '\0');

	JsonStringCleanUp(&String);

	return ok;
}


static bool TestJsonStringGetLength(void)
{
	tJsonString String;
	bool ok;

	JsonStringSetUp(&String);

	ok = JsonStringAddCharacter(&String, 0x7F);

	ok = ok && (JsonStringGetLength(&String) == 1);

	ok = ok && JsonStringAddCharacter(&String, 0x7FF);

	ok = ok && (JsonStringGetLength(&String) == 3);

	ok = ok && JsonStringAddCharacter(&String, 0xFFFF);

	ok = ok && (JsonStringGetLength(&String) == 6);

	ok = ok && JsonStringAddCharacter(&String, 0x10FFFF);

	ok = ok && (JsonStringGetLength(&String) == 10);

	JsonStringCleanUp(&String);

	return ok;
}


static bool TestJsonStringAddUtf8Code(void)
{
	tJsonString String;
	tJsonCharacter Character;
	tJsonUtf8Code NextCode;
	size_t Offset;
	size_t Length;
	bool ok = true;

	JsonStringSetUp(&String);

	for (Character = 1; ok && (Character < 0xD800); Character++)
	{
		ok = JsonStringAddUtf8Code(&String, JsonUtf8Code(Character));
	}

	for (Character = 0xE000; ok && (Character < 0x110000); Character++)
	{
		ok = JsonStringAddUtf8Code(&String, JsonUtf8Code(Character));
	}

	for (Character = 1, Offset = 0; ok && (Character < 0xD800); Character++, Offset = Offset + Length)
	{
		Length = JsonStringGetNextUtf8Code(&String, Offset, &NextCode);
		
		ok = (Length == JsonUtf8CodeGetUnitLength(NextCode));
		
		ok = ok && (Character == JsonUtf8CodeGetCharacter(NextCode));
	}

	for (Character = 0xE000; ok && (Character < 0x110000); Character++, Offset = Offset + Length)
	{
		Length = JsonStringGetNextUtf8Code(&String, Offset, &NextCode);
		
		ok = (Length == JsonUtf8CodeGetUnitLength(NextCode));
		
		ok = ok && (Character == JsonUtf8CodeGetCharacter(NextCode));
	}

	ok = ok && (Offset == JsonStringGetLength(&String));

	JsonStringCleanUp(&String);

	return ok;
}


static bool TestJsonStringAddCharacter(void)
{
	tJsonString String;
	tJsonCharacter Character;
	tJsonCharacter NextCharacter;
	size_t Offset;
	size_t Length;
	bool ok = true;

	JsonStringSetUp(&String);

	for (Character = 1; ok && (Character < 0xD800); Character++)
	{
		ok = JsonStringAddCharacter(&String, Character);
	}

	for (Character = 0xE000; ok && (Character < 0x110000); Character++)
	{
		ok = JsonStringAddCharacter(&String, Character);
	}

	for (Character = 1, Offset = 0; ok && (Character < 0xD800); Character++, Offset = Offset + Length)
	{
		Length = JsonStringGetNextCharacter(&String, Offset, &NextCharacter);
		
		ok = (Length == JsonUtf8CodeGetUnitLength(JsonUtf8Code(Character)));
		
		ok = ok && (Character == NextCharacter);
	}

	for (Character = 0xE000; ok && (Character < 0x110000); Character++, Offset = Offset + Length)
	{
		Length = JsonStringGetNextCharacter(&String, Offset, &NextCharacter);
		
		ok = (Length == JsonUtf8CodeGetUnitLength(JsonUtf8Code(Character)));
		
		ok = ok && (Character == NextCharacter);
	}

	ok = ok && (Offset == JsonStringGetLength(&String));

	JsonStringCleanUp(&String);

	return ok;
}


static bool TestJsonStringGetNextUtf8Code(void)
{
	tJsonString String;
	tJsonCharacter Character;
	tJsonUtf8Code NextCode;
	size_t Offset;
	size_t Length;
	bool ok = true;

	JsonStringSetUp(&String);

	for (Character = 1, Offset = 0; ok && (Character < 0xD800); Character++, Offset = Offset + Length)
	{
		Length = JsonUtf8CodeGetUnitLength(JsonUtf8Code(Character));

		ok = (JsonStringGetNextUtf8Code(&String, Offset, &NextCode) == 0);
		
		ok = ok && (NextCode == '\0');

		ok = ok && JsonStringAddCharacter(&String, Character);

		ok = ok && (JsonStringGetNextUtf8Code(&String, Offset, &NextCode) == Length);
		
		ok = ok && (Character == JsonUtf8CodeGetCharacter(NextCode));
	}

	for (Character = 0xE000; ok && (Character < 0x110000); Character++, Offset = Offset + Length)
	{
		Length = JsonUtf8CodeGetUnitLength(JsonUtf8Code(Character));

		ok = (JsonStringGetNextUtf8Code(&String, Offset, &NextCode) == 0);
		
		ok = ok && (NextCode == '\0');

		ok = ok && JsonStringAddCharacter(&String, Character);

		ok = ok && (JsonStringGetNextUtf8Code(&String, Offset, &NextCode) == Length);
		
		ok = ok && (Character == JsonUtf8CodeGetCharacter(NextCode));
	}

	JsonStringCleanUp(&String);

	return ok;
}


static bool TestJsonStringGetNextCharacter(void)
{
	tJsonString String;
	tJsonCharacter Character;
	tJsonCharacter NextCharacter;
	size_t Offset;
	size_t Length;
	bool ok = true;

	JsonStringSetUp(&String);

	for (Character = 1, Offset = 0; ok && (Character < 0xD800); Character++, Offset = Offset + Length)
	{
		Length = JsonUtf8CodeGetUnitLength(JsonUtf8Code(Character));

		ok = (JsonStringGetNextCharacter(&String, Offset, &NextCharacter) == 0);
		
		ok = ok && (NextCharacter == '\0');

		ok = ok && JsonStringAddCharacter(&String, Character);

		ok = ok && (JsonStringGetNextCharacter(&String, Offset, &NextCharacter) == Length);
		
		ok = ok && (Character == NextCharacter);
	}

	for (Character = 0xE000; ok && (Character < 0x110000); Character++, Offset = Offset + Length)
	{
		Length = JsonUtf8CodeGetUnitLength(JsonUtf8Code(Character));

		ok = (JsonStringGetNextCharacter(&String, Offset, &NextCharacter) == 0);
		
		ok = ok && (NextCharacter == '\0');

		ok = ok && JsonStringAddCharacter(&String, Character);

		ok = ok && (JsonStringGetNextCharacter(&String, Offset, &NextCharacter) == Length);
		
		ok = ok && (Character == NextCharacter);
	}

	JsonStringCleanUp(&String);

	return ok;
}


static const tTestCase TestCaseJsonString[] =
{
	{ "JsonStringSetUp",            TestJsonStringSetUp            },
	{ "JsonStringCleanUp",          TestJsonStringCleanUp          },
	{ "JsonStringClear",            TestJsonStringClear            },
	{ "JsonStringGetLength",        TestJsonStringGetLength        },
	{ "JsonStringAddUtfCode",       TestJsonStringAddUtf8Code      },
	{ "JsonStringAddCharacter",     TestJsonStringAddCharacter     },
	{ "JsonStringGetNextUtf8Code",  TestJsonStringGetNextUtf8Code  },
	{ "JsonStringGetNextCharacter", TestJsonStringGetNextCharacter }
};


const tTest TestJsonString =
{
	"JsonString",
	TestCaseJsonString,
	sizeof(TestCaseJsonString) / sizeof(TestCaseJsonString[0])
};
