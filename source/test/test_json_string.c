#include "json_string.h"
#include "test_json.h"


static tTestResult TestJsonStringSetUp(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonString String;
	tJsonUtf8Code Code;

	JsonStringSetUp(&String);

	TEST_IS_EQ(JsonStringGetLength(&String), 0, TestResult);

	TEST_IS_EQ(JsonStringGetNextUtf8Code(&String, 0, &Code), 0, TestResult);
	
	TEST_IS_EQ(Code, '\0', TestResult);

	JsonStringCleanUp(&String);

	return TestResult;
}


static tTestResult TestJsonStringCleanUp(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonString String;
	tJsonUtf8Code Code;

	JsonStringSetUp(&String);

	TEST_IS_TRUE(JsonStringAddUtf8Code(&String, 'a'), TestResult);

	TEST_IS_EQ(JsonStringGetLength(&String), 1, TestResult);

	TEST_IS_EQ(JsonStringGetNextUtf8Code(&String, 0, &Code), 1, TestResult);
	
	TEST_IS_EQ(Code, 'a', TestResult);

	JsonStringCleanUp(&String);

	TEST_IS_EQ(JsonStringGetLength(&String), 0, TestResult);

	TEST_IS_EQ(JsonStringGetNextUtf8Code(&String, 0, &Code), 0, TestResult);
	
	TEST_IS_EQ(Code, '\0', TestResult);

	return TestResult;
}


static tTestResult TestJsonStringClear(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonString String;
	tJsonUtf8Code Code;

	JsonStringSetUp(&String);

	TEST_IS_TRUE(JsonStringAddCharacter(&String, 'a'), TestResult);

	TEST_IS_EQ(JsonStringGetLength(&String), 1, TestResult);

	TEST_IS_EQ(JsonStringGetNextUtf8Code(&String, 0, &Code), 1, TestResult);
	
	TEST_IS_EQ(Code, 'a', TestResult);

	JsonStringClear(&String);

	TEST_IS_EQ(JsonStringGetLength(&String), 0, TestResult);

	TEST_IS_EQ(JsonStringGetNextUtf8Code(&String, 0, &Code), 0, TestResult);
	
	TEST_IS_EQ(Code, '\0', TestResult);

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


static tTestResult TestJsonStringAddUtf8Code(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonString String;
	tJsonCharacter Character;
	tJsonUtf8Code NextCode;
	size_t Offset;
	size_t Length;

	JsonStringSetUp(&String);

	for (Character = 1; Character < 0xD800; Character++)
	{
		TEST_IS_TRUE(JsonStringAddUtf8Code(&String, JsonUtf8Code(Character)), TestResult);
	}

	for (Character = 0xE000; Character < 0x110000; Character++)
	{
		TEST_IS_TRUE(JsonStringAddUtf8Code(&String, JsonUtf8Code(Character)), TestResult);
	}

	for (Character = 1, Offset = 0; Character < 0xD800; Character++, Offset = Offset + Length)
	{
		Length = JsonStringGetNextUtf8Code(&String, Offset, &NextCode);
		
		TEST_IS_EQ(Length, JsonUtf8CodeGetUnitLength(NextCode), TestResult);
		TEST_IS_EQ(Character, JsonUtf8CodeGetCharacter(NextCode), TestResult);
	}

	for (Character = 0xE000; Character < 0x110000; Character++, Offset = Offset + Length)
	{
		Length = JsonStringGetNextUtf8Code(&String, Offset, &NextCode);
		
		TEST_IS_EQ(Length, JsonUtf8CodeGetUnitLength(NextCode), TestResult);
		TEST_IS_EQ(Character, JsonUtf8CodeGetCharacter(NextCode), TestResult);
	}

	TEST_IS_EQ(Offset, JsonStringGetLength(&String), TestResult);

	JsonStringCleanUp(&String);

	return TestResult;
}


static tTestResult TestJsonStringAddCharacter(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonString String;
	tJsonCharacter Character;
	tJsonCharacter NextCharacter;
	size_t Offset;
	size_t Length;

	JsonStringSetUp(&String);

	for (Character = 1; Character < 0xD800; Character++)
	{
		TEST_IS_TRUE(JsonStringAddCharacter(&String, Character), TestResult);
	}

	for (Character = 0xE000; Character < 0x110000; Character++)
	{
		TEST_IS_TRUE(JsonStringAddCharacter(&String, Character), TestResult);
	}

	for (Character = 1, Offset = 0; Character < 0xD800; Character++, Offset = Offset + Length)
	{
		Length = JsonStringGetNextCharacter(&String, Offset, &NextCharacter);
		
		TEST_IS_EQ(Length, JsonUtf8CodeGetUnitLength(JsonUtf8Code(Character)), TestResult);
		TEST_IS_EQ(Character, NextCharacter, TestResult);
	}

	for (Character = 0xE000; Character < 0x110000; Character++, Offset = Offset + Length)
	{
		Length = JsonStringGetNextCharacter(&String, Offset, &NextCharacter);
		
		TEST_IS_EQ(Length, JsonUtf8CodeGetUnitLength(JsonUtf8Code(Character)), TestResult);
		TEST_IS_EQ(Character, NextCharacter, TestResult);
	}

	TEST_IS_EQ(Offset, JsonStringGetLength(&String), TestResult);

	JsonStringCleanUp(&String);

	return TestResult;
}


static tTestResult TestJsonStringGetNextUtf8Code(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonString String;
	tJsonCharacter Character;
	tJsonUtf8Code NextCode;
	size_t Offset;
	size_t Length;

	JsonStringSetUp(&String);

	for (Character = 1, Offset = 0; Character < 0xD800; Character++, Offset = Offset + Length)
	{
		Length = JsonUtf8CodeGetUnitLength(JsonUtf8Code(Character));

		TEST_IS_EQ(JsonStringGetNextUtf8Code(&String, Offset, &NextCode), 0, TestResult);
		
		TEST_IS_EQ(NextCode, '\0', TestResult);

		TEST_IS_TRUE(JsonStringAddCharacter(&String, Character), TestResult);

		TEST_IS_EQ(JsonStringGetNextUtf8Code(&String, Offset, &NextCode), Length, TestResult);
		
		TEST_IS_EQ(Character, JsonUtf8CodeGetCharacter(NextCode), TestResult);
	}

	for (Character = 0xE000; Character < 0x110000; Character++, Offset = Offset + Length)
	{
		Length = JsonUtf8CodeGetUnitLength(JsonUtf8Code(Character));

		TEST_IS_EQ(JsonStringGetNextUtf8Code(&String, Offset, &NextCode), 0, TestResult);
		
		TEST_IS_EQ(NextCode, '\0', TestResult);

		TEST_IS_TRUE(JsonStringAddCharacter(&String, Character), TestResult);

		TEST_IS_EQ(JsonStringGetNextUtf8Code(&String, Offset, &NextCode), Length, TestResult);
		
		TEST_IS_EQ(Character, JsonUtf8CodeGetCharacter(NextCode), TestResult);
	}

	JsonStringCleanUp(&String);

	return TestResult;
}


static tTestResult TestJsonStringGetNextCharacter(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonString String;
	tJsonCharacter Character;
	tJsonCharacter NextCharacter;
	size_t Offset;
	size_t Length;

	JsonStringSetUp(&String);

	for (Character = 1, Offset = 0; Character < 0xD800; Character++, Offset = Offset + Length)
	{
		Length = JsonUtf8CodeGetUnitLength(JsonUtf8Code(Character));

		TEST_IS_EQ(JsonStringGetNextCharacter(&String, Offset, &NextCharacter), 0, TestResult);
		
		TEST_IS_EQ(NextCharacter, '\0', TestResult);

		TEST_IS_TRUE(JsonStringAddCharacter(&String, Character), TestResult);

		TEST_IS_EQ(JsonStringGetNextCharacter(&String, Offset, &NextCharacter), Length, TestResult);
		
		TEST_IS_EQ(Character, NextCharacter, TestResult);
	}

	for (Character = 0xE000; Character < 0x110000; Character++, Offset = Offset + Length)
	{
		Length = JsonUtf8CodeGetUnitLength(JsonUtf8Code(Character));

		TEST_IS_EQ(JsonStringGetNextCharacter(&String, Offset, &NextCharacter), 0, TestResult);
		
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
