#include "json_utf8.h"
#include "test_json.h"


static tTestResult TestJsonUtf8Encode(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonCharacter Character;
	tJsonUtf8 Utf8;
	size_t Length;

	for (Character = 0; Character < 0x80; Character++)
	{
		Length = JsonUtf8Encode(Utf8, sizeof(Utf8), 0, Character);
		TEST_IS_EQ(Length, 1, TestResult);
		TEST_IS_EQ(Utf8[0], Character, TestResult);
	}

	for (Character = 0x80; Character < 0x800; Character++)
	{
		Length = JsonUtf8Encode(Utf8, sizeof(Utf8), 0, Character);
		TEST_IS_EQ(Length, 2, TestResult);
		TEST_IS_EQ(Utf8[0], 0xC0 + (Character >> 6), TestResult);
		TEST_IS_EQ(Utf8[1], 0x80 + (Character & 0x3F), TestResult);
	}

	for (Character = 0x800; Character < 0xD800; Character++)
	{
		Length = JsonUtf8Encode(Utf8, sizeof(Utf8), 0, Character);
		TEST_IS_EQ(Length, 3, TestResult);
		TEST_IS_EQ(Utf8[0], 0xE0 + (Character >> 12), TestResult);
		TEST_IS_EQ(Utf8[1], 0x80 + ((Character >> 6) & 0x3F), TestResult);
		TEST_IS_EQ(Utf8[2], 0x80 + (Character & 0x3F), TestResult);
	}

	for (Character = 0xE000; Character < 0x10000; Character++)
	{
		Length = JsonUtf8Encode(Utf8, sizeof(Utf8), 0, Character);
		TEST_IS_EQ(Length, 3, TestResult);
		TEST_IS_EQ(Utf8[0], 0xE0 + (Character >> 12), TestResult);
		TEST_IS_EQ(Utf8[1], 0x80 + ((Character >> 6) & 0x3F), TestResult);
		TEST_IS_EQ(Utf8[2], 0x80 + (Character & 0x3F), TestResult);
	}

	for (Character = 0x10000; Character < 0x110000; Character++)
	{
		Length = JsonUtf8Encode(Utf8, sizeof(Utf8), 0, Character);
		TEST_IS_EQ(Length, 4, TestResult);
		TEST_IS_EQ(Utf8[0], 0xF0 + (Character >> 18), TestResult);
		TEST_IS_EQ(Utf8[1], 0x80 + ((Character >> 12) & 0x3F), TestResult);
		TEST_IS_EQ(Utf8[2], 0x80 + ((Character >> 6) & 0x3F), TestResult);
		TEST_IS_EQ(Utf8[3], 0x80 + (Character & 0x3F), TestResult);
	}

	for (Character = 0x110000; Character != 0; Character++)
	{
		Length = JsonUtf8Encode(Utf8, sizeof(Utf8), 0, Character);
		TEST_IS_ZERO(Length, TestResult);
	}

	return TestResult;
}


static tTestResult TestJsonUtf8DecodeNext(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonCharacter Character;
	tJsonCharacter NextCharacter;
	uint8_t Content[4] = {};
	size_t Length;

	TEST_IS_ZERO(JsonUtf8DecodeNext(Content, 0, 0, &NextCharacter), TestResult);

	for (Character = 0; Character < 0x110000; Character++)
	{
		if ((Character < 0xD800) || (Character > 0xDFFF))
		{
			Length = JsonUtf8Encode(Content, sizeof(Content), 0, Character);

			TEST_IS_EQ(JsonUtf8DecodeNext(Content, sizeof(Content), 0, &NextCharacter), Length, TestResult);
			TEST_IS_EQ(NextCharacter, Character, TestResult);
		}
	}

	for (Character = 0x80; Character < 0x100; Character++)
	{
		Content[0] = Character;

		TEST_IS_ZERO(JsonUtf8DecodeNext(Content, 1, 0, &NextCharacter), TestResult);
	}

	for (Character = 0x8000; Character < 0x10000; Character++)
	{
		if ((Character & 0xE0C0) != 0xC080)
		{
			Content[0] = Character >> 8;
			Content[1] = Character;

			TEST_IS_ZERO(JsonUtf8DecodeNext(Content, 2, 0, &NextCharacter), TestResult);
		}
	}

	for (Character = 0x800000; Character < 0x1000000; Character++)
	{
		if (((Character & 0xE0C000) != 0xC08000) && ((Character & 0xF0C0C0) != 0xE08080))
		{
			Content[0] = Character >> 16;
			Content[1] = Character >> 8;
			Content[2] = Character;

			TEST_IS_ZERO(JsonUtf8DecodeNext(Content, 3, 0, &NextCharacter), TestResult);
		}
	}

	for (Character = 0x80000000; Character != 0; Character++)
	{
		if (((Character & 0xE0C00000) != 0xC0800000) && ((Character & 0xF0C0C000) != 0xE0808000) && ((Character & 0xF8C0C0C0) != 0xF0808080))
		{
			Content[0] = Character >> 24;
			Content[1] = Character >> 16;
			Content[2] = Character >> 8;
			Content[3] = Character;

			TEST_IS_ZERO(JsonUtf8DecodeNext(Content, 4, 0, &NextCharacter), TestResult);
		}
	}

	for (Character = 0; Character < 0x80; Character++)
	{
		Content[0] = 0xC0 + (Character >> 6);
		Content[1] = 0x80 + (Character & 0x3F);

		TEST_IS_ZERO(JsonUtf8DecodeNext(Content, 2, 0, &NextCharacter), TestResult);
	}

	for (Character = 0; Character < 0x800; Character++)
	{
		Content[0] = 0xE0 + (Character >> 12);
		Content[1] = 0x80 + ((Character >> 6) & 0x3F);
		Content[2] = 0x80 + (Character & 0x3F);

		TEST_IS_ZERO(JsonUtf8DecodeNext(Content, 3, 0, &NextCharacter), TestResult);
	}

	for (Character = 0; Character < 0x10000; Character++)
	{
		Content[0] = 0xF0 + (Character >> 18);
		Content[1] = 0x80 + ((Character >> 12) & 0x3F);
		Content[2] = 0x80 + ((Character >> 6) & 0x3F);
		Content[3] = 0x80 + (Character & 0x3F);

		TEST_IS_ZERO(JsonUtf8DecodeNext(Content, 4, 0, &NextCharacter), TestResult);
	}

	for (Character = 0xD800; Character < 0xE000; Character++)
	{
		Content[0] = 0xE0 + (Character >> 12);
		Content[1] = 0x80 + ((Character >> 6) & 0x3F);
		Content[2] = 0x80 + (Character & 0x3F);

		TEST_IS_ZERO(JsonUtf8DecodeNext(Content, 3, 0, &NextCharacter), TestResult);
	}

	for (Character = 0x110000; Character < 0x200000; Character++)
	{
		Content[0] = 0xF0 + (Character >> 18);
		Content[1] = 0x80 + ((Character >> 12) & 0x3F);
		Content[2] = 0x80 + ((Character >> 6) & 0x3F);
		Content[3] = 0x80 + (Character & 0x3F);

		TEST_IS_ZERO(JsonUtf8DecodeNext(Content, 4, 0, &NextCharacter), TestResult);
	}

	return TestResult;
}


static tTestResult TestJsonUtf8DecodePrevious(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonCharacter Character;
	tJsonCharacter PreviousCharacter;
	tJsonUtf8 Utf8 = {};
	size_t Length;

	TEST_IS_ZERO(JsonUtf8DecodePrevious(Utf8, 0, 0, &PreviousCharacter), TestResult);

	for (Character = 0; Character < 0x110000; Character++)
	{
		if ((Character < 0xD800) || (Character > 0xDFFF))
		{
			Length = JsonUtf8Encode(Utf8, sizeof(Utf8), 0, Character);

			TEST_IS_EQ(JsonUtf8DecodePrevious(Utf8, Length, Length, &PreviousCharacter), Length, TestResult);
			TEST_IS_EQ(PreviousCharacter, Character, TestResult);
		}
	}

	for (Character = 0x80; Character < 0x100; Character++)
	{
		Utf8[0] = Character;

		TEST_IS_ZERO(JsonUtf8DecodePrevious(Utf8, 1, 1, &PreviousCharacter), TestResult);
	}

	for (Character = 0x100; Character < 0x10000; Character++)
	{
		if (((Character & 0x80) != 0) && ((Character & 0xE0C0) != 0xC080))
		{
			Utf8[0] = Character >> 8;
			Utf8[1] = Character;

			TEST_IS_ZERO(JsonUtf8DecodePrevious(Utf8, 2, 2, &PreviousCharacter), TestResult);
		}
	}

	for (Character = 0x10000; Character < 0x1000000; Character++)
	{
		if (((Character & 0x80) != 0) && ((Character & 0xE0C0) != 0xC080) && ((Character & 0xF0C0C0) != 0xE08080))
		{
			Utf8[0] = Character >> 16;
			Utf8[1] = Character >> 8;
			Utf8[2] = Character;

			TEST_IS_ZERO(JsonUtf8DecodePrevious(Utf8, 3, 3, &PreviousCharacter), TestResult);
		}
	}

	for (Character = 0x01000000; Character != 0; Character++)
	{
		if (((Character & 0x80) != 0) && ((Character & 0xE0C0) != 0xC080) && ((Character & 0xF0C0C0) != 0xE08080) && ((Character & 0xF8C0C0C0) != 0xF0808080))
		{
			Utf8[0] = Character >> 24;
			Utf8[1] = Character >> 16;
			Utf8[2] = Character >> 8;
			Utf8[3] = Character;

			TEST_IS_ZERO(JsonUtf8DecodePrevious(Utf8, 4, 4, &PreviousCharacter), TestResult);
		}
	}

	for (Character = 0; Character < 0x80; Character++)
	{
		Utf8[0] = 0xC0 + (Character >> 6);
		Utf8[1] = 0x80 + (Character & 0x3F);

		TEST_IS_ZERO(JsonUtf8DecodePrevious(Utf8, 2, 2, &PreviousCharacter), TestResult);
	}

	for (Character = 0; Character < 0x800; Character++)
	{
		Utf8[0] = 0xE0 + (Character >> 12);
		Utf8[1] = 0x80 + ((Character >> 6) & 0x3F);
		Utf8[2] = 0x80 + (Character & 0x3F);

		TEST_IS_ZERO(JsonUtf8DecodePrevious(Utf8, 3, 3, &PreviousCharacter), TestResult);
	}

	for (Character = 0xD800; Character < 0xE000; Character++)
	{
		Utf8[0] = 0xE0 + (Character >> 12);
		Utf8[1] = 0x80 + ((Character >> 6) & 0x3F);
		Utf8[2] = 0x80 + (Character & 0x3F);

		TEST_IS_ZERO(JsonUtf8DecodePrevious(Utf8, 3, 3, &PreviousCharacter), TestResult);
	}

	for (Character = 0; Character < 0x10000; Character++)
	{
		Utf8[0] = 0xF0 + (Character >> 18);
		Utf8[1] = 0x80 + ((Character >> 12) & 0x3F);
		Utf8[2] = 0x80 + ((Character >> 6) & 0x3F);
		Utf8[3] = 0x80 + (Character & 0x3F);

		TEST_IS_ZERO(JsonUtf8DecodePrevious(Utf8, 4, 4, &PreviousCharacter), TestResult);
	}

	for (Character = 0x110000; Character < 0x200000; Character++)
	{
		Utf8[0] = 0xF0 + (Character >> 18);
		Utf8[1] = 0x80 + ((Character >> 12) & 0x3F);
		Utf8[2] = 0x80 + ((Character >> 6) & 0x3F);
		Utf8[3] = 0x80 + (Character & 0x3F);

		TEST_IS_ZERO(JsonUtf8DecodePrevious(Utf8, 4, 4, &PreviousCharacter), TestResult);
	}

	return TestResult;
}


static const tTestCase TestCaseJsonUtf8[] =
{
	{ "JsonUtf8Encode",         TestJsonUtf8Encode         },
	{ "JsonUtf8DecodeNext",     TestJsonUtf8DecodeNext     },
	{ "JsonUtf8DecodePrevious", TestJsonUtf8DecodePrevious }
};


const tTest TestJsonUtf8 =
{
	"JsonUtf8",
	TestCaseJsonUtf8,
	sizeof(TestCaseJsonUtf8) / sizeof(TestCaseJsonUtf8[0])
};
