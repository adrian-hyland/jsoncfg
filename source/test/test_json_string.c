#include "json_string.h"
#include "test_json.h"


static tTestResult TestJsonStringSetUp(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonString String;
	tJsonCharacter Character;

	JsonStringSetUp(&String);

	TEST_IS_ZERO(JsonStringGetLength(&String), TestResult);

	TEST_IS_ZERO(JsonStringGetNextCharacter(&String, 0, &Character), TestResult);
	
	TEST_IS_EQ(Character, '\0', TestResult);

	JsonStringCleanUp(&String);

	return TestResult;
}


static tTestResult TestJsonStringCleanUp(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonString String;
	tJsonCharacter Character;

	JsonStringSetUp(&String);

	TEST_IS_TRUE(JsonStringAddCharacter(&String, 'a'), TestResult);

	TEST_IS_EQ(JsonStringGetLength(&String), 1, TestResult);

	TEST_IS_EQ(JsonStringGetNextCharacter(&String, 0, &Character), 1, TestResult);
	
	TEST_IS_EQ(Character, 'a', TestResult);

	JsonStringCleanUp(&String);

	TEST_IS_ZERO(JsonStringGetLength(&String), TestResult);

	TEST_IS_ZERO(JsonStringGetNextCharacter(&String, 0, &Character), TestResult);
	
	TEST_IS_EQ(Character, '\0', TestResult);

	return TestResult;
}


static tTestResult TestJsonStringClear(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonString String;
	tJsonCharacter Character;

	JsonStringSetUp(&String);

	TEST_IS_TRUE(JsonStringAddCharacter(&String, 'a'), TestResult);

	TEST_IS_EQ(JsonStringGetLength(&String), 1, TestResult);

	TEST_IS_EQ(JsonStringGetNextCharacter(&String, 0, &Character), 1, TestResult);
	
	TEST_IS_EQ(Character, 'a', TestResult);

	JsonStringClear(&String);

	TEST_IS_ZERO(JsonStringGetLength(&String), TestResult);

	TEST_IS_ZERO(JsonStringGetNextCharacter(&String, 0, &Character), TestResult);
	
	TEST_IS_EQ(Character, '\0', TestResult);

	JsonStringCleanUp(&String);

	return TestResult;
}


static tTestResult TestJsonStringGetLength(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonString String;

	JsonStringSetUp(&String);

	TEST_IS_TRUE(JsonStringAddCharacter(&String, 0x7F), TestResult);

	TEST_IS_EQ(JsonStringGetLength(&String), 1, TestResult);

	TEST_IS_TRUE(JsonStringAddCharacter(&String, 0x7FF), TestResult);

	TEST_IS_EQ(JsonStringGetLength(&String), 3, TestResult);

	TEST_IS_TRUE(JsonStringAddCharacter(&String, 0xFFFF), TestResult);

	TEST_IS_EQ(JsonStringGetLength(&String), 6, TestResult);

	TEST_IS_TRUE(JsonStringAddCharacter(&String, 0x10FFFF), TestResult);

	TEST_IS_EQ(JsonStringGetLength(&String), 10, TestResult);

	JsonStringCleanUp(&String);

	return TestResult;
}


static tTestResult TestJsonStringAddCharacter(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonString String;
	tJsonCharacter Character;
	tJsonCharacter NextCharacter;
	tJsonUtf8 Utf8;
	size_t Offset;
	size_t Length;

	JsonStringSetUp(&String);

	for (Character = 1; Character < 0xD800; Character++)
	{
		TEST_IS_TRUE(JsonStringAddCharacter(&String, Character), TestResult);
	}

	for (Character = 0xD800; Character < 0xE000; Character++)
	{
		TEST_IS_FALSE(JsonStringAddCharacter(&String, Character), TestResult);
	}

	for (Character = 0xE000; Character < 0x110000; Character++)
	{
		TEST_IS_TRUE(JsonStringAddCharacter(&String, Character), TestResult);
	}

	for (Character = 0x110000; Character != 0; Character++)
	{
		TEST_IS_FALSE(JsonStringAddCharacter(&String, Character), TestResult);
	}

	for (Character = 1, Offset = 0; Character < 0xD800; Character++, Offset = Offset + Length)
	{
		Length = JsonStringGetNextCharacter(&String, Offset, &NextCharacter);
		
		TEST_IS_EQ(Length, JsonUtf8Encode(Utf8, sizeof(Utf8), 0, Character), TestResult);
		TEST_IS_EQ(Character, NextCharacter, TestResult);
	}

	for (Character = 0xE000; Character < 0x110000; Character++, Offset = Offset + Length)
	{
		Length = JsonStringGetNextCharacter(&String, Offset, &NextCharacter);
		
		TEST_IS_EQ(Length, JsonUtf8Encode(Utf8, sizeof(Utf8), 0, Character), TestResult);
		TEST_IS_EQ(Character, NextCharacter, TestResult);
	}

	TEST_IS_EQ(Offset, JsonStringGetLength(&String), TestResult);

	JsonStringCleanUp(&String);

	return TestResult;
}


static tTestResult TestJsonStringGetNextCharacter(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonString String;
	tJsonCharacter Character;
	tJsonCharacter NextCharacter;
	tJsonUtf8 Utf8;
	size_t Offset;
	size_t Length;

	JsonStringSetUp(&String);

	for (Character = 1, Offset = 0; Character < 0xD800; Character++, Offset = Offset + Length)
	{
		Length = JsonUtf8Encode(Utf8, sizeof(Utf8), 0, Character);

		TEST_IS_ZERO(JsonStringGetNextCharacter(&String, Offset, &NextCharacter), TestResult);
		
		TEST_IS_EQ(NextCharacter, '\0', TestResult);

		TEST_IS_TRUE(JsonStringAddCharacter(&String, Character), TestResult);

		TEST_IS_EQ(JsonStringGetNextCharacter(&String, Offset, &NextCharacter), Length, TestResult);
		
		TEST_IS_EQ(Character, NextCharacter, TestResult);
	}

	for (Character = 0xE000; Character < 0x110000; Character++, Offset = Offset + Length)
	{
		Length = JsonUtf8Encode(Utf8, sizeof(Utf8), 0, Character);

		TEST_IS_ZERO(JsonStringGetNextCharacter(&String, Offset, &NextCharacter), TestResult);
		
		TEST_IS_EQ(NextCharacter, '\0', TestResult);

		TEST_IS_TRUE(JsonStringAddCharacter(&String, Character), TestResult);

		TEST_IS_EQ(JsonStringGetNextCharacter(&String, Offset, &NextCharacter), Length, TestResult);
		
		TEST_IS_EQ(Character, NextCharacter, TestResult);
	}

	JsonStringCleanUp(&String);

	return TestResult;
}


static const tTestCase TestCaseJsonString[] =
{
	{ "JsonStringSetUp",            TestJsonStringSetUp            },
	{ "JsonStringCleanUp",          TestJsonStringCleanUp          },
	{ "JsonStringClear",            TestJsonStringClear            },
	{ "JsonStringGetLength",        TestJsonStringGetLength        },
	{ "JsonStringAddCharacter",     TestJsonStringAddCharacter     },
	{ "JsonStringGetNextCharacter", TestJsonStringGetNextCharacter }
};


const tTest TestJsonString =
{
	"JsonString",
	TestCaseJsonString,
	sizeof(TestCaseJsonString) / sizeof(TestCaseJsonString[0])
};
