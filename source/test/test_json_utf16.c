#include "json_utf16.h"
#include "test_json.h"


static tTestResult TestJsonUtf16beEncode(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonCharacter Character;
	tJsonUtf16 Utf16;
	size_t Length;

	for (Character = 0; Character < 0xD800; Character++)
	{
		Length = JsonUtf16beEncode(Utf16, sizeof(Utf16), 0, Character);
		TEST_IS_EQ(Length, 2, TestResult);
		TEST_IS_EQ(Utf16[0], Character >> 8, TestResult);
		TEST_IS_EQ(Utf16[1], Character & 0xFF, TestResult);
	}

	for (Character = 0xD800; Character < 0xE000; Character++)
	{
		Length = JsonUtf16beEncode(Utf16, sizeof(Utf16), 0, Character);
		TEST_IS_ZERO(Length, TestResult);
	}

	for (Character = 0xE000; Character < 0x10000; Character++)
	{
		Length = JsonUtf16beEncode(Utf16, sizeof(Utf16), 0, Character);
		TEST_IS_EQ(Length, 2, TestResult);
		TEST_IS_EQ(Utf16[0], Character >> 8, TestResult);
		TEST_IS_EQ(Utf16[1], Character & 0xFF, TestResult);
	}

	for (Character = 0x10000; Character < 0x110000; Character++)
	{
		Length = JsonUtf16beEncode(Utf16, sizeof(Utf16), 0, Character);
		TEST_IS_EQ(Length, 4, TestResult);
		TEST_IS_EQ(Utf16[0], 0xD8 + ((Character - 0x10000) >> 18), TestResult);
		TEST_IS_EQ(Utf16[1], ((Character - 0x10000) >> 10) & 0xFF, TestResult);
		TEST_IS_EQ(Utf16[2], 0xDC + (((Character - 0x10000) >> 8) & 0x03), TestResult);
		TEST_IS_EQ(Utf16[3], (Character - 0x10000) & 0xFF, TestResult);
	}

	for (Character = 0x110000; Character != 0; Character++)
	{
		Length = JsonUtf16beEncode(Utf16, sizeof(Utf16), 0, Character);
		TEST_IS_ZERO(Length, TestResult);
	}

	return TestResult;
}


static tTestResult TestJsonUtf16beDecodeNext(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonCharacter Character1;
	tJsonCharacter Character2;
	tJsonCharacter NextCharacter;
	tJsonUtf16 Utf16 = {};
	size_t Length;

	Length = JsonUtf16beDecodeNext(Utf16, 0, 0, &NextCharacter);
	TEST_IS_ZERO(Length, TestResult);
	TEST_IS_EQ(NextCharacter, '\0', TestResult);

	for (Character1 = 0; Character1 < 0xD800; Character1++)
	{
		Utf16[0] = Character1 >> 8;
		Utf16[1] = Character1;
		Length = JsonUtf16beDecodeNext(Utf16, sizeof(Utf16), 0, &NextCharacter);
		TEST_IS_EQ(Length, 2, TestResult)
		TEST_IS_EQ(NextCharacter, Character1, TestResult);
		Length = JsonUtf16beDecodeNext(Utf16, Length - 1, 0, &NextCharacter);
		TEST_IS_ZERO(Length, TestResult)
		TEST_IS_EQ(NextCharacter, JSON_CHARACTER_REPLACEMENT, TestResult);
	}
	
	for (Character1 = 0xD800; Character1 < 0xDC00; Character1++)
	{
		Utf16[0] = Character1 >> 8;
		Utf16[1] = Character1;
		for (Character2 = 0; Character2 < 0xDC00; Character2++)
		{
			Utf16[2] = Character2 >> 8;
			Utf16[3] = Character2;
			Length = JsonUtf16beDecodeNext(Utf16, sizeof(Utf16), 0, &NextCharacter);
			TEST_IS_ZERO(Length, TestResult)
			TEST_IS_EQ(NextCharacter, JSON_CHARACTER_REPLACEMENT, TestResult);
		}
		for (Character2 = 0xDC00; Character2 < 0xE000; Character2++)
		{
			Utf16[2] = Character2 >> 8;
			Utf16[3] = Character2;
			Length = JsonUtf16beDecodeNext(Utf16, sizeof(Utf16), 0, &NextCharacter);
			TEST_IS_EQ(Length, 4, TestResult)
			TEST_IS_EQ(NextCharacter, 0x10000 + ((Character1 - 0xD800) << 10) + Character2 - 0xDC00, TestResult);
			Length = JsonUtf16beDecodeNext(Utf16, Length - 1, 0, &NextCharacter);
			TEST_IS_ZERO(Length, TestResult)
			TEST_IS_EQ(NextCharacter, JSON_CHARACTER_REPLACEMENT, TestResult);
		}
		for (Character2 = 0xE000; Character2 < 0x10000; Character2++)
		{
			Utf16[2] = Character2 >> 8;
			Utf16[3] = Character2;
			Length = JsonUtf16beDecodeNext(Utf16, sizeof(Utf16), 0, &NextCharacter);
			TEST_IS_ZERO(Length, TestResult)
			TEST_IS_EQ(NextCharacter, JSON_CHARACTER_REPLACEMENT, TestResult);
		}
	}

	for (Character1 = 0xDC00; Character1 < 0xE000; Character1++)
	{
		Utf16[0] = Character1 >> 8;
		Utf16[1] = Character1;
		Length = JsonUtf16beDecodeNext(Utf16, sizeof(Utf16), 0, &NextCharacter);
		TEST_IS_ZERO(Length, TestResult)
		TEST_IS_EQ(NextCharacter, JSON_CHARACTER_REPLACEMENT, TestResult);
	}

	for (Character1 = 0xE000; Character1 < 0x10000; Character1++)
	{
		Utf16[0] = Character1 >> 8;
		Utf16[1] = Character1;
		Length = JsonUtf16beDecodeNext(Utf16, sizeof(Utf16), 0, &NextCharacter);
		TEST_IS_EQ(Length, 2, TestResult)
		TEST_IS_EQ(NextCharacter, Character1, TestResult);
		Length = JsonUtf16beDecodeNext(Utf16, Length - 1, 0, &NextCharacter);
		TEST_IS_ZERO(Length, TestResult)
		TEST_IS_EQ(NextCharacter, JSON_CHARACTER_REPLACEMENT, TestResult);
	}

	return TestResult;
}


static tTestResult TestJsonUtf16leEncode(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonCharacter Character;
	tJsonUtf16 Utf16;
	size_t Length;

	for (Character = 0; Character < 0xD800; Character++)
	{
		Length = JsonUtf16leEncode(Utf16, sizeof(Utf16), 0, Character);
		TEST_IS_EQ(Length, 2, TestResult);
		TEST_IS_EQ(Utf16[0], Character & 0xFF, TestResult);
		TEST_IS_EQ(Utf16[1], Character >> 8, TestResult);
	}

	for (Character = 0xD800; Character < 0xE000; Character++)
	{
		Length = JsonUtf16leEncode(Utf16, sizeof(Utf16), 0, Character);
		TEST_IS_ZERO(Length, TestResult);
	}

	for (Character = 0xE000; Character < 0x10000; Character++)
	{
		Length = JsonUtf16leEncode(Utf16, sizeof(Utf16), 0, Character);
		TEST_IS_EQ(Length, 2, TestResult);
		TEST_IS_EQ(Utf16[0], Character & 0xFF, TestResult);
		TEST_IS_EQ(Utf16[1], Character >> 8, TestResult);
	}

	for (Character = 0x10000; Character < 0x110000; Character++)
	{
		Length = JsonUtf16leEncode(Utf16, sizeof(Utf16), 0, Character);
		TEST_IS_EQ(Length, 4, TestResult);
		TEST_IS_EQ(Utf16[0], ((Character - 0x10000) >> 10) & 0xFF, TestResult);
		TEST_IS_EQ(Utf16[1], 0xD8 + ((Character - 0x10000) >> 18), TestResult);
		TEST_IS_EQ(Utf16[2], (Character - 0x10000) & 0xFF, TestResult);
		TEST_IS_EQ(Utf16[3], 0xDC + (((Character - 0x10000) >> 8) & 0x03), TestResult);
	}

	for (Character = 0x110000; Character != 0; Character++)
	{
		Length = JsonUtf16leEncode(Utf16, sizeof(Utf16), 0, Character);
		TEST_IS_ZERO(Length, TestResult);
	}

	return TestResult;
}


static tTestResult TestJsonUtf16leDecodeNext(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonCharacter Character1;
	tJsonCharacter Character2;
	tJsonCharacter NextCharacter;
	tJsonUtf16 Utf16 = {};
	size_t Length;

	Length = JsonUtf16leDecodeNext(Utf16, 0, 0, &NextCharacter);
	TEST_IS_ZERO(Length, TestResult);
	TEST_IS_EQ(NextCharacter, '\0', TestResult);

	for (Character1 = 0; Character1 < 0xD800; Character1++)
	{
		Utf16[0] = Character1;
		Utf16[1] = Character1 >> 8;
		Length = JsonUtf16leDecodeNext(Utf16, sizeof(Utf16), 0, &NextCharacter);
		TEST_IS_EQ(Length, 2, TestResult);
		TEST_IS_EQ(NextCharacter, Character1, TestResult);
		Length = JsonUtf16leDecodeNext(Utf16, Length - 1, 0, &NextCharacter);
		TEST_IS_ZERO(Length, TestResult);
		TEST_IS_EQ(NextCharacter, JSON_CHARACTER_REPLACEMENT, TestResult);
	}
	
	for (Character1 = 0xD800; Character1 < 0xDC00; Character1++)
	{
		Utf16[0] = Character1;
		Utf16[1] = Character1 >> 8;
		for (Character2 = 0; Character2 < 0xDC00; Character2++)
		{
			Utf16[2] = Character2;
			Utf16[3] = Character2 >> 8;
			Length = JsonUtf16leDecodeNext(Utf16, sizeof(Utf16), 0, &NextCharacter);
			TEST_IS_ZERO(Length, TestResult);
			TEST_IS_EQ(NextCharacter, JSON_CHARACTER_REPLACEMENT, TestResult);
		}
		for (Character2 = 0xDC00; Character2 < 0xE000; Character2++)
		{
			Utf16[2] = Character2;
			Utf16[3] = Character2 >> 8;
			Length = JsonUtf16leDecodeNext(Utf16, sizeof(Utf16), 0, &NextCharacter);
			TEST_IS_EQ(Length, 4, TestResult);
			TEST_IS_EQ(NextCharacter, 0x10000 + ((Character1 - 0xD800) << 10) + Character2 - 0xDC00, TestResult);
			Length = JsonUtf16leDecodeNext(Utf16, Length - 1, 0, &NextCharacter);
			TEST_IS_ZERO(Length, TestResult);
			TEST_IS_EQ(NextCharacter, JSON_CHARACTER_REPLACEMENT, TestResult);
		}
		for (Character2 = 0xE000; Character2 < 0x10000; Character2++)
		{
			Utf16[2] = Character2;
			Utf16[3] = Character2 >> 8;
			Length = JsonUtf16leDecodeNext(Utf16, sizeof(Utf16), 0, &NextCharacter);
			TEST_IS_ZERO(Length, TestResult);
			TEST_IS_EQ(NextCharacter, JSON_CHARACTER_REPLACEMENT, TestResult);
		}
	}

	for (Character1 = 0xDC00; Character1 < 0xE000; Character1++)
	{
		Utf16[0] = Character1;
		Utf16[1] = Character1 >> 8;
		Length = JsonUtf16leDecodeNext(Utf16, sizeof(Utf16), 0, &NextCharacter);
		TEST_IS_ZERO(Length, TestResult);
		TEST_IS_EQ(NextCharacter, JSON_CHARACTER_REPLACEMENT, TestResult);
	}

	for (Character1 = 0xE000; Character1 < 0x10000; Character1++)
	{
		Utf16[0] = Character1;
		Utf16[1] = Character1 >> 8;
		Length = JsonUtf16leDecodeNext(Utf16, sizeof(Utf16), 0, &NextCharacter);
		TEST_IS_EQ(Length, 2, TestResult);
		TEST_IS_EQ(NextCharacter, Character1, TestResult);
		Length = JsonUtf16leDecodeNext(Utf16, Length - 1, 0, &NextCharacter);
		TEST_IS_ZERO(Length, TestResult);
		TEST_IS_EQ(NextCharacter, JSON_CHARACTER_REPLACEMENT, TestResult);
	}

	return TestResult;
}


static tTestResult TestJsonUtf16beIsHighSurrogate(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonUtf16Unit Unit;

	for (Unit[0] = 0; Unit[0] < 0xD8; Unit[0]++)
	{
		TEST_IS_FALSE(JsonUtf16beIsHighSurrogate(Unit), TestResult);
	}

	for (Unit[0] = 0xD8; Unit[0] < 0xDC; Unit[0]++)
	{
		TEST_IS_TRUE(JsonUtf16beIsHighSurrogate(Unit), TestResult);
	}

	for (Unit[0] = 0xDC; Unit[0] != 0; Unit[0]++)
	{
		TEST_IS_FALSE(JsonUtf16beIsHighSurrogate(Unit), TestResult);
	}

	return TestResult;
}


static tTestResult TestJsonUtf16beIsLowSurrogate(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonUtf16Unit Unit;

	for (Unit[0] = 0; Unit[0] < 0xDC; Unit[0]++)
	{
		TEST_IS_FALSE(JsonUtf16beIsLowSurrogate(Unit), TestResult);
	}

	for (Unit[0] = 0xDC; Unit[0] < 0xE0; Unit[0]++)
	{
		TEST_IS_TRUE(JsonUtf16beIsLowSurrogate(Unit), TestResult);
	}

	for (Unit[0] = 0xE0; Unit[0] != 0; Unit[0]++)
	{
		TEST_IS_FALSE(JsonUtf16beIsLowSurrogate(Unit), TestResult);
	}

	return TestResult;
}


static const tTestCase TestCaseJsonUtf16[] =
{
	{ "JsonUtf16beEncode",          TestJsonUtf16beEncode          },
	{ "JsonUtf16beDecodeNext",      TestJsonUtf16beDecodeNext      },
	{ "JsonUtf16leEncode",          TestJsonUtf16leEncode          },
	{ "JsonUtf16leDecodeNext",      TestJsonUtf16leDecodeNext      },
	{ "JsonUtf16beIsHighSurrogate", TestJsonUtf16beIsHighSurrogate },
	{ "JsonUtf16beIsLowSurrogate",  TestJsonUtf16beIsLowSurrogate  },
};


const tTest TestJsonUtf16 =
{
	"JsonUtf16",
	TestCaseJsonUtf16,
	sizeof(TestCaseJsonUtf16) / sizeof(TestCaseJsonUtf16[0])
};
