#include "json_utf8.h"
#include "test_json.h"


static tTestResult TestJsonUtf8Code(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonCharacter Character;

	for (Character = 0; Character < 0xD800; Character++)
	{
		TEST_IS_NOT_EQ(JsonUtf8Code(Character), JSON_UTF8_CODE_REPLACEMENT, TestResult);
	}
	
	for (Character = 0xD800; Character < 0xE000; Character++)
	{
		TEST_IS_EQ(JsonUtf8Code(Character), JSON_UTF8_CODE_REPLACEMENT, TestResult);
	}

	for (Character = 0xE000; Character < JSON_CHARACTER_REPLACEMENT; Character++)
	{
		TEST_IS_NOT_EQ(JsonUtf8Code(Character), JSON_UTF8_CODE_REPLACEMENT, TestResult);
	}

	TEST_IS_EQ(JsonUtf8Code(JSON_CHARACTER_REPLACEMENT), JSON_UTF8_CODE_REPLACEMENT, TestResult);

	for (Character = JSON_CHARACTER_REPLACEMENT + 1; Character < 0x110000; Character++)
	{
		TEST_IS_NOT_EQ(JsonUtf8Code(Character), JSON_UTF8_CODE_REPLACEMENT, TestResult);
	}

	for (Character = 0x110000; Character != 0; Character++)
	{
		TEST_IS_EQ(JsonUtf8Code(Character), JSON_UTF8_CODE_REPLACEMENT, TestResult);
	}

	return TestResult;
}


static tTestResult TestJsonUtf8GetCharacter(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonUtf8Code Code;
	tJsonCharacter ExpectedCharacter = 0;
	tJsonCharacter Character;

	for (Code = 0; Code < JSON_UTF8_CODE_REPLACEMENT; Code++)
	{
		Character = JsonUtf8CodeGetCharacter(Code);
		if (JsonUtf8CodeIsValid(Code))
		{
			TEST_IS_EQ(Character, ExpectedCharacter, TestResult);
			TEST_IS_EQ(JsonUtf8Code(Character), Code, TestResult);
			ExpectedCharacter++;
			if (ExpectedCharacter == 0xD800)
			{
				ExpectedCharacter = 0xE000;
			}
		}
		else
		{
			TEST_IS_EQ(Character, JSON_CHARACTER_REPLACEMENT, TestResult);
		}
	}

	TEST_IS_EQ(ExpectedCharacter, JSON_CHARACTER_REPLACEMENT, TestResult);
	TEST_IS_EQ(JsonUtf8CodeGetCharacter(Code), JSON_CHARACTER_REPLACEMENT, TestResult);
	ExpectedCharacter++;

	for (Code = JSON_UTF8_CODE_REPLACEMENT + 1; Code != 0; Code++)
	{
		Character = JsonUtf8CodeGetCharacter(Code);
		if (JsonUtf8CodeIsValid(Code))
		{
			TEST_IS_EQ(Character, ExpectedCharacter, TestResult);
			TEST_IS_EQ(JsonUtf8Code(Character), Code, TestResult);
			ExpectedCharacter++;
		}
		else
		{
			TEST_IS_EQ(Character, JSON_CHARACTER_REPLACEMENT, TestResult);
		}
	}

	TEST_IS_EQ(ExpectedCharacter, 0x110000, TestResult);

	return TestResult;
}


static tTestResult TestJsonUtf8CodeIsValid(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonUtf8Code Code;

	for (Code = 0; Code < JSON_UTF8_CODE_REPLACEMENT; Code++)
	{
		if (JsonUtf8CodeIsValid(Code))
		{
			TEST_IS_NOT_EQ(JsonUtf8CodeGetCharacter(Code), JSON_CHARACTER_REPLACEMENT, TestResult);
		}
		else
		{
			TEST_IS_EQ(JsonUtf8CodeGetCharacter(Code), JSON_CHARACTER_REPLACEMENT, TestResult);
		}
	}

	TEST_IS_TRUE(JsonUtf8CodeIsValid(JSON_UTF8_CODE_REPLACEMENT), TestResult);

	for (Code = JSON_UTF8_CODE_REPLACEMENT + 1; Code != 0; Code++)
	{
		if (JsonUtf8CodeIsValid(Code))
		{
			TEST_IS_NOT_EQ(JsonUtf8CodeGetCharacter(Code), JSON_CHARACTER_REPLACEMENT, TestResult);
		}
		else
		{
			TEST_IS_EQ(JsonUtf8CodeGetCharacter(Code), JSON_CHARACTER_REPLACEMENT, TestResult);
		}
	}

	return TestResult;
}


static tTestResult TestJsonUtf8CodeGetUnitLength(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonUtf8Code Code;

	for (Code = 0; Code < 0xF4908080; Code++)
	{
		if (JsonUtf8CodeIsValid(Code))
		{
			if (Code < 0x100)
			{
				TEST_IS_EQ(JsonUtf8CodeGetUnitLength(Code), 1, TestResult);
			}
			else if (Code < 0x10000)
			{
				TEST_IS_EQ(JsonUtf8CodeGetUnitLength(Code), 2, TestResult);
			}
			else if (Code < 0x1000000)
			{
				TEST_IS_EQ(JsonUtf8CodeGetUnitLength(Code), 3, TestResult);
			}
			else
			{
				TEST_IS_EQ(JsonUtf8CodeGetUnitLength(Code), 4, TestResult);
			}
		}
		else
		{
			TEST_IS_EQ(JsonUtf8CodeGetUnitLength(Code), 0, TestResult);
		}
	}

	for (Code = 0xF4908080; Code != 0; Code++)
	{
		TEST_IS_EQ(JsonUtf8CodeGetUnitLength(Code), 0, TestResult);
	}

	return TestResult;
}


static tTestResult TestJsonUtf8CodeGetUnit(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonUtf8Code Code;

	for (Code = 0; Code < 0xF4908080; Code++)
	{
		if (JsonUtf8CodeIsValid(Code))
		{
			if (Code < 0x100)
			{
				TEST_IS_EQ(JsonUtf8CodeGetUnit(Code, 0), Code, TestResult);
			}
			else if (Code < 0x10000)
			{
				TEST_IS_EQ(JsonUtf8CodeGetUnit(Code, 0), Code >> 8, TestResult);
				TEST_IS_EQ(JsonUtf8CodeGetUnit(Code, 1), Code & 0xFF, TestResult);
			}
			else if (Code < 0x1000000)
			{
				TEST_IS_EQ(JsonUtf8CodeGetUnit(Code, 0), Code >> 16, TestResult);
				TEST_IS_EQ(JsonUtf8CodeGetUnit(Code, 1), (Code >> 8) & 0xFF, TestResult);
				TEST_IS_EQ(JsonUtf8CodeGetUnit(Code, 2), Code & 0xFF, TestResult);
			}
			else
			{
				TEST_IS_EQ(JsonUtf8CodeGetUnit(Code, 0), Code >> 24, TestResult);
				TEST_IS_EQ(JsonUtf8CodeGetUnit(Code, 1), (Code >> 16) & 0xFF, TestResult);
				TEST_IS_EQ(JsonUtf8CodeGetUnit(Code, 2), (Code >> 8) & 0xFF, TestResult);
				TEST_IS_EQ(JsonUtf8CodeGetUnit(Code, 3), Code & 0xFF, TestResult);
			}
		}
		else
		{
			TEST_IS_EQ(JsonUtf8CodeGetUnit(Code, 0), 0x00, TestResult);
		}
	}

	for (Code = 0xF4908080; Code != 0; Code++)
	{
		TEST_IS_EQ(JsonUtf8CodeGetUnit(Code, 0), 0x00, TestResult);
	}

	return TestResult;
}


static tTestResult TestJsonUtf8CodeAddUnit(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonUtf8Code Code;
	tJsonUtf8Unit Unit1;
	tJsonUtf8Unit Unit2;
	tJsonUtf8Unit Unit3;
	tJsonUtf8Unit Unit4;

	for (Unit1 = 0; Unit1 < 0x80; Unit1++)
	{
		Code = 0;
		TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_VALID, TestResult);
		TEST_IS_EQ(Code, Unit1, TestResult);
		TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

		Code = (Unit1 << 8) + 0x80;
		TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

		Code = (Unit1 << 16) + 0x8080;
		TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

		Code = (Unit1 << 24) + 0x808080;
		TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
	}

	for (Unit1 = 0x80; Unit1 < 0xC2; Unit1++)
	{
		Code = 0;
		TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INVALID, TestResult);

		Code = Unit1;
		TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

		Code = (Unit1 << 8) + 0x80;
		TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

		Code = (Unit1 << 16) + 0x8080;
		TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

		Code = (Unit1 << 24) + 0x808080;
		TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
	}

	for (Unit1 = 0xC2; Unit1 < 0xE0; Unit1++)
	{
		for (Unit2 = 0; Unit2 < 0x80; Unit2++)
		{
			Code = 0;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 8) + Unit2;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 16) + (Unit2 << 8) + 0x80;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 24) + (Unit2 << 16) + 0x8080;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
		}

		for (Unit2 = 0x80; Unit2 < 0xC0; Unit2++)
		{
			Code = 0;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_VALID, TestResult);
			TEST_IS_EQ(Code, ((tJsonUtf8Code)Unit1 << 8) + (tJsonUtf8Code)Unit2, TestResult);
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 16) + (Unit2 << 8) + 0x80;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 24) + (Unit2 << 16) + 0x8080;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
		}

		for (Unit2 = 0xC0; Unit2 != 0; Unit2++)
		{
			Code = 0;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 8) + Unit2;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 16) + (Unit2 << 8) + 0x80;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 24) + (Unit2 << 16) + 0x8080;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
		}
	}

	for (Unit1 = 0xE0; Unit1 < 0xE1; Unit1++)
	{
		for (Unit2 = 0; Unit2 < 0xA0; Unit2++)
		{
			Code = 0;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 8) + Unit2;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 16) + (Unit2 << 8) + 0x80;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 24) + (Unit2 << 16) + 0x8080;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
		}

		for (Unit2 = 0xA0; Unit2 < 0xC0; Unit2++)
		{
			for (Unit3 = 0; Unit2 < 0x80; Unit2++)
			{
				Code = 0;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit3), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 16) + (Unit2 << 8) + Unit3;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 24) + (Unit2 << 16) + (Unit3 << 8) + 0x80;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
			}

			for (Unit3 = 0x80; Unit3 < 0xC0; Unit3++)
			{
				Code = 0;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit3), JSON_UTF8_VALID, TestResult);
				TEST_IS_EQ(Code, ((tJsonUtf8Code)Unit1 << 16) + ((tJsonUtf8Code)Unit2 << 8) + (tJsonUtf8Code)Unit3, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 24) + (Unit2 << 16) + (Unit3 << 8) + 0x80;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
			}

			for (Unit3 = 0xC0; Unit3 != 0; Unit3++)
			{
				Code = 0;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit3), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 16) + (Unit2 << 8) + Unit3;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 24) + (Unit2 << 16) + (Unit3 << 8) + 0x80;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
			}
		}

		for (Unit2 = 0xC0; Unit2 != 0; Unit2++)
		{
			Code = 0;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 8) + Unit2;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 16) + (Unit2 << 8) + 0x80;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 24) + (Unit2 << 16) + 0x8080;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
		}
	}

	for (Unit1 = 0xE1; Unit1 < 0xED; Unit1++)
	{
		for (Unit2 = 0; Unit2 < 0x80; Unit2++)
		{
			Code = 0;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 8) + Unit2;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 16) + (Unit2 << 8) + 0x80;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 24) + (Unit2 << 16) + 0x8080;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
		}

		for (Unit2 = 0x80; Unit2 < 0xC0; Unit2++)
		{
			for (Unit3 = 0; Unit2 < 0x80; Unit2++)
			{
				Code = 0;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit3), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 16) + (Unit2 << 8) + Unit3;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 24) + (Unit2 << 16) + (Unit3 << 8) + 0x80;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
			}

			for (Unit3 = 0x80; Unit3 < 0xC0; Unit3++)
			{
				Code = 0;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit3), JSON_UTF8_VALID, TestResult);
				TEST_IS_EQ(Code, ((tJsonUtf8Code)Unit1 << 16) + ((tJsonUtf8Code)Unit2 << 8) + (tJsonUtf8Code)Unit3, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 24) + (Unit2 << 16) + (Unit3 << 8) + 0x80;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
			}

			for (Unit3 = 0xC0; Unit3 != 0; Unit3++)
			{
				Code = 0;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit3), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 16) + (Unit2 << 8) + Unit3;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 24) + (Unit2 << 16) + (Unit3 << 8) + 0x80;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
			}
		}

		for (Unit2 = 0xC0; Unit2 != 0; Unit2++)
		{
			Code = 0;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 8) + Unit2;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 16) + (Unit2 << 8) + 0x80;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 24) + (Unit2 << 16) + 0x8080;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
		}
	}

	for (Unit1 = 0xED; Unit1 < 0xEE; Unit1++)
	{
		for (Unit2 = 0; Unit2 < 0x80; Unit2++)
		{
			Code = 0;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 8) + Unit2;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 16) + (Unit2 << 8) + 0x80;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 24) + (Unit2 << 16) + 0x8080;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
		}

		for (Unit2 = 0x80; Unit2 < 0xA0; Unit2++)
		{
			for (Unit3 = 0; Unit2 < 0x80; Unit2++)
			{
				Code = 0;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit3), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 16) + (Unit2 << 8) + Unit3;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 24) + (Unit2 << 16) + (Unit3 << 8) + 0x80;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
			}

			for (Unit3 = 0x80; Unit3 < 0xC0; Unit3++)
			{
				Code = 0;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit3), JSON_UTF8_VALID, TestResult);
				TEST_IS_EQ(Code, ((tJsonUtf8Code)Unit1 << 16) + ((tJsonUtf8Code)Unit2 << 8) + (tJsonUtf8Code)Unit3, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 24) + (Unit2 << 16) + (Unit3 << 8) + 0x80;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
			}

			for (Unit3 = 0xC0; Unit3 != 0; Unit3++)
			{
				Code = 0;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit3), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 16) + (Unit2 << 8) + Unit3;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 24) + (Unit2 << 16) + (Unit3 << 8) + 0x80;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
			}
		}

		for (Unit2 = 0xA0; Unit2 != 0; Unit2++)
		{
			Code = 0;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 8) + Unit2;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 16) + (Unit2 << 8) + 0x80;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 24) + (Unit2 << 16) + 0x8080;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
		}
	}

	for (Unit1 = 0xEE; Unit1 < 0xF0; Unit1++)
	{
		for (Unit2 = 0; Unit2 < 0x80; Unit2++)
		{
			Code = 0;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 8) + Unit2;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 16) + (Unit2 << 8) + 0x80;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 24) + (Unit2 << 16) + 0x8080;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
		}

		for (Unit2 = 0x80; Unit2 < 0xC0; Unit2++)
		{
			for (Unit3 = 0; Unit2 < 0x80; Unit2++)
			{
				Code = 0;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit3), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 16) + (Unit2 << 8) + Unit3;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 24) + (Unit2 << 16) + (Unit3 << 8) + 0x80;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
			}

			for (Unit3 = 0x80; Unit3 < 0xC0; Unit3++)
			{
				Code = 0;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit3), JSON_UTF8_VALID, TestResult);
				TEST_IS_EQ(Code, ((tJsonUtf8Code)Unit1 << 16) + ((tJsonUtf8Code)Unit2 << 8) + (tJsonUtf8Code)Unit3, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 24) + (Unit2 << 16) + (Unit3 << 8) + 0x80;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
			}

			for (Unit3 = 0xC0; Unit3 != 0; Unit3++)
			{
				Code = 0;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit3), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 16) + (Unit2 << 8) + Unit3;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 24) + (Unit2 << 16) + (Unit3 << 8) + 0x80;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
			}
		}

		for (Unit2 = 0xC0; Unit2 != 0; Unit2++)
		{
			Code = 0;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 8) + Unit2;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 16) + (Unit2 << 8) + 0x80;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 24) + (Unit2 << 16) + 0x8080;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
		}
	}

	for (Unit1 = 0xF0; Unit1 < 0xF1; Unit1++)
	{
		for (Unit2 = 0; Unit2 < 0x90; Unit2++)
		{
			Code = 0;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 8) + Unit2;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 16) + (Unit2 << 8) + 0x80;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 24) + (Unit2 << 16) + 0x8080;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
		}

		for (Unit2 = 0x90; Unit2 < 0xC0; Unit2++)
		{
			for (Unit3 = 0; Unit2 < 0x80; Unit2++)
			{
				Code = 0;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit3), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 16) + (Unit2 << 8) + Unit3;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 24) + (Unit2 << 16) + (Unit3 << 8) + 0x80;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
			}

			for (Unit3 = 0x80; Unit3 < 0xC0; Unit3++)
			{
				for (Unit4 = 0x00; Unit4 < 0x80; Unit4++)
				{
					Code = 0;
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INCOMPLETE, TestResult);
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit3), JSON_UTF8_INCOMPLETE, TestResult);
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit4), JSON_UTF8_INVALID, TestResult);

					Code = (Unit1 << 24) + (Unit2 << 16) + (Unit3 << 8) + Unit4;
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
				}

				for (Unit4 = 0x80; Unit4 < 0xC0; Unit4++)
				{
					Code = 0;
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INCOMPLETE, TestResult);
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit3), JSON_UTF8_INCOMPLETE, TestResult);
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit4), JSON_UTF8_VALID, TestResult);
					TEST_IS_EQ(Code, ((tJsonUtf8Code)Unit1 << 24) + ((tJsonUtf8Code)Unit2 << 16) + ((tJsonUtf8Code)Unit3 << 8) + (tJsonUtf8Code)Unit4, TestResult);
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
				}
			}

			for (Unit3 = 0xC0; Unit3 != 0; Unit3++)
			{
				Code = 0;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit3), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 16) + (Unit2 << 8) + Unit3;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 24) + (Unit2 << 16) + (Unit3 << 8) + 0x80;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
			}
		}

		for (Unit2 = 0xC0; Unit2 != 0; Unit2++)
		{
			Code = 0;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 8) + Unit2;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 16) + (Unit2 << 8) + 0x80;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 24) + (Unit2 << 16) + 0x8080;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
		}
	}

	for (Unit1 = 0xF1; Unit1 < 0xF4; Unit1++)
	{
		for (Unit2 = 0; Unit2 < 0x80; Unit2++)
		{
			Code = 0;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 8) + Unit2;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 16) + (Unit2 << 8) + 0x80;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 24) + (Unit2 << 16) + 0x8080;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
		}

		for (Unit2 = 0x80; Unit2 < 0xC0; Unit2++)
		{
			for (Unit3 = 0; Unit2 < 0x80; Unit2++)
			{
				Code = 0;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit3), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 16) + (Unit2 << 8) + Unit3;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 24) + (Unit2 << 16) + (Unit3 << 8) + 0x80;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
			}

			for (Unit3 = 0x80; Unit3 < 0xC0; Unit3++)
			{
				for (Unit4 = 0x00; Unit4 < 0x80; Unit4++)
				{
					Code = 0;
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INCOMPLETE, TestResult);
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit3), JSON_UTF8_INCOMPLETE, TestResult);
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit4), JSON_UTF8_INVALID, TestResult);

					Code = (Unit1 << 24) + (Unit2 << 16) + (Unit3 << 8) + Unit4;
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
				}

				for (Unit4 = 0x80; Unit4 < 0xC0; Unit4++)
				{
					Code = 0;
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INCOMPLETE, TestResult);
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit3), JSON_UTF8_INCOMPLETE, TestResult);
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit4), JSON_UTF8_VALID, TestResult);
					TEST_IS_EQ(Code, ((tJsonUtf8Code)Unit1 << 24) + ((tJsonUtf8Code)Unit2 << 16) + ((tJsonUtf8Code)Unit3 << 8) + (tJsonUtf8Code)Unit4, TestResult);
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
				}
			}

			for (Unit3 = 0xC0; Unit3 != 0; Unit3++)
			{
				Code = 0;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit3), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 16) + (Unit2 << 8) + Unit3;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 24) + (Unit2 << 16) + (Unit3 << 8) + 0x80;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
			}
		}

		for (Unit2 = 0xC0; Unit2 != 0; Unit2++)
		{
			Code = 0;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 8) + Unit2;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 16) + (Unit2 << 8) + 0x80;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 24) + (Unit2 << 16) + 0x8080;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
		}
	}

	for (Unit1 = 0xF4; Unit1 < 0xF5; Unit1++)
	{
		for (Unit2 = 0; Unit2 < 0x80; Unit2++)
		{
			Code = 0;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 8) + Unit2;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 16) + (Unit2 << 8) + 0x80;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 24) + (Unit2 << 16) + 0x8080;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
		}

		for (Unit2 = 0x80; Unit2 < 0x90; Unit2++)
		{
			for (Unit3 = 0; Unit2 < 0x80; Unit2++)
			{
				Code = 0;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit3), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 16) + (Unit2 << 8) + Unit3;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 24) + (Unit2 << 16) + (Unit3 << 8) + 0x80;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
			}

			for (Unit3 = 0x80; Unit3 < 0xC0; Unit3++)
			{
				for (Unit4 = 0x00; Unit4 < 0x80; Unit4++)
				{
					Code = 0;
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INCOMPLETE, TestResult);
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit3), JSON_UTF8_INCOMPLETE, TestResult);
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit4), JSON_UTF8_INVALID, TestResult);

					Code = (Unit1 << 24) + (Unit2 << 16) + (Unit3 << 8) + Unit4;
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
				}

				for (Unit4 = 0x80; Unit4 < 0xC0; Unit4++)
				{
					Code = 0;
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INCOMPLETE, TestResult);
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit3), JSON_UTF8_INCOMPLETE, TestResult);
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit4), JSON_UTF8_VALID, TestResult);
					TEST_IS_EQ(Code, ((tJsonUtf8Code)Unit1 << 24) + ((tJsonUtf8Code)Unit2 << 16) + ((tJsonUtf8Code)Unit3 << 8) + (tJsonUtf8Code)Unit4, TestResult);
					TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
				}
			}

			for (Unit3 = 0xC0; Unit3 != 0; Unit3++)
			{
				Code = 0;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INCOMPLETE, TestResult);
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit3), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 16) + (Unit2 << 8) + Unit3;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

				Code = (Unit1 << 24) + (Unit2 << 16) + (Unit3 << 8) + 0x80;
				TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
			}
		}

		for (Unit2 = 0x90; Unit2 != 0; Unit2++)
		{
			Code = 0;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INCOMPLETE, TestResult);
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit2), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 8) + Unit2;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 16) + (Unit2 << 8) + 0x80;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

			Code = (Unit1 << 24) + (Unit2 << 16) + 0x8080;
			TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
		}
	}

	for (Unit1 = 0xF5; Unit1 != 0; Unit1++)
	{
		Code = 0;
		TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, Unit1), JSON_UTF8_INVALID, TestResult);

		Code = Unit1;
		TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

		Code = (Unit1 << 16) + 0x8080;
		TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);

		Code = (Unit1 << 24) + 0x808080;
		TEST_IS_EQ(JsonUtf8CodeAddUnit(&Code, 0x80), JSON_UTF8_INVALID, TestResult);
	}

	return TestResult;
}


static tTestResult TestJsonUtf8GetNextCode(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonUtf8Unit Content[4] = {};
	tJsonCharacter Character;
	tJsonUtf8Code Code;
	tJsonUtf8Code NextCode;

	TEST_IS_EQ(JsonUtf8GetNextCode(Content, 0, 0, &NextCode), 0, TestResult);

	for (Character = 0; Character < 0x110000; Character++)
	{
		if ((Character < 0xD800) || (Character > 0xDFFF))
		{
			Code = JsonUtf8Code(Character);
			Content[0] = JsonUtf8CodeGetUnit(Code, 0);
			Content[1] = JsonUtf8CodeGetUnit(Code, 1);
			Content[2] = JsonUtf8CodeGetUnit(Code, 2);
			Content[3] = JsonUtf8CodeGetUnit(Code, 3);

			TEST_IS_EQ(JsonUtf8GetNextCode(Content, sizeof(Content), 0, &NextCode), JsonUtf8CodeGetUnitLength(Code), TestResult);
			TEST_IS_EQ(Code, NextCode, TestResult);
		}
	}

	for (Code = 0x80; Code < 0x100; Code++)
	{
		Content[0] = Code;

		TEST_IS_EQ(JsonUtf8GetNextCode(Content, 1, 0, &NextCode), 0, TestResult);
	}

	for (Code = 0x8000; Code < 0x10000; Code++)
	{
		if ((Code & 0xE0C0) != 0xC080)
		{
			Content[0] = Code >> 8;
			Content[1] = Code;

			TEST_IS_EQ(JsonUtf8GetNextCode(Content, 2, 0, &NextCode), 0, TestResult);
		}
	}

	for (Code = 0x800000; Code < 0x1000000; Code++)
	{
		if (((Code & 0xE0C000) != 0xC08000) && ((Code & 0xF0C0C0) != 0xE08080))
		{
			Content[0] = Code >> 16;
			Content[1] = Code >> 8;
			Content[2] = Code;

			TEST_IS_EQ(JsonUtf8GetNextCode(Content, 3, 0, &NextCode), 0, TestResult);
		}
	}

	for (Code = 0x80000000; Code != 0; Code++)
	{
		if (((Code & 0xE0C00000) != 0xC0800000) && ((Code & 0xF0C0C000) != 0xE0808000) && ((Code & 0xF8C0C0C0) != 0xF0808080))
		{
			Content[0] = Code >> 24;
			Content[1] = Code >> 16;
			Content[2] = Code >> 8;
			Content[3] = Code;

			TEST_IS_EQ(JsonUtf8GetNextCode(Content, 4, 0, &NextCode), 0, TestResult);
		}
	}

	for (Code = 0; Code < 0x80; Code++)
	{
		Content[0] = 0xC0 + (Code >> 6);
		Content[1] = 0x80 + (Code & 0x3F);

		TEST_IS_EQ(JsonUtf8GetNextCode(Content, 2, 0, &NextCode), 0, TestResult);
	}

	for (Code = 0; Code < 0x800; Code++)
	{
		Content[0] = 0xE0 + (Code >> 12);
		Content[1] = 0x80 + ((Code >> 6) & 0x3F);
		Content[2] = 0x80 + (Code & 0x3F);

		TEST_IS_EQ(JsonUtf8GetNextCode(Content, 3, 0, &NextCode), 0, TestResult);
	}

	for (Code = 0; Code < 0x10000; Code++)
	{
		Content[0] = 0xF0 + (Code >> 18);
		Content[1] = 0x80 + ((Code >> 12) & 0x3F);
		Content[2] = 0x80 + ((Code >> 6) & 0x3F);
		Content[3] = 0x80 + (Code & 0x3F);

		TEST_IS_EQ(JsonUtf8GetNextCode(Content, 4, 0, &NextCode), 0, TestResult);
	}

	for (Code = 0xD800; Code < 0xE000; Code++)
	{
		Content[0] = 0xE0 + (Code >> 12);
		Content[1] = 0x80 + ((Code >> 6) & 0x3F);
		Content[2] = 0x80 + (Code & 0x3F);

		TEST_IS_EQ(JsonUtf8GetNextCode(Content, 3, 0, &NextCode), 0, TestResult);
	}

	for (Code = 0x110000; Code < 0x200000; Code++)
	{
		Content[0] = 0xF0 + (Code >> 18);
		Content[1] = 0x80 + ((Code >> 12) & 0x3F);
		Content[2] = 0x80 + ((Code >> 6) & 0x3F);
		Content[3] = 0x80 + (Code & 0x3F);

		TEST_IS_EQ(JsonUtf8GetNextCode(Content, 4, 0, &NextCode), 0, TestResult);
	}

	return TestResult;
}


static tTestResult TestJsonUtf8GetPreviousCode(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonUtf8Unit Content[4] = {};
	tJsonCharacter Character;
	tJsonUtf8Code Code;
	tJsonUtf8Code PreviousCode;
	size_t Length;

	TEST_IS_EQ(JsonUtf8GetPreviousCode(Content, 0, 0, &PreviousCode), 0, TestResult);

	for (Character = 0; Character < 0x110000; Character++)
	{
		if ((Character < 0xD800) || (Character > 0xDFFF))
		{
			Code = JsonUtf8Code(Character);
			Length = JsonUtf8CodeGetUnitLength(Code);
			Content[0] = JsonUtf8CodeGetUnit(Code, 0);
			Content[1] = JsonUtf8CodeGetUnit(Code, 1);
			Content[2] = JsonUtf8CodeGetUnit(Code, 2);
			Content[3] = JsonUtf8CodeGetUnit(Code, 3);

			TEST_IS_EQ(JsonUtf8GetPreviousCode(Content, Length, Length, &PreviousCode), Length, TestResult);
			TEST_IS_EQ(Code, PreviousCode, TestResult);
		}
	}

	for (Code = 0x80; Code < 0x100; Code++)
	{
		Content[0] = Code;

		TEST_IS_EQ(JsonUtf8GetPreviousCode(Content, 1, 1, &PreviousCode), 0, TestResult);
	}

	for (Code = 0x100; Code < 0x10000; Code++)
	{
		if (((Code & 0x80) != 0) && ((Code & 0xE0C0) != 0xC080))
		{
			Content[0] = Code >> 8;
			Content[1] = Code;

			TEST_IS_EQ(JsonUtf8GetPreviousCode(Content, 2, 2, &PreviousCode), 0, TestResult);
		}
	}

	for (Code = 0x10000; Code < 0x1000000; Code++)
	{
		if (((Code & 0x80) != 0) && ((Code & 0xE0C0) != 0xC080) && ((Code & 0xF0C0C0) != 0xE08080))
		{
			Content[0] = Code >> 16;
			Content[1] = Code >> 8;
			Content[2] = Code;

			TEST_IS_EQ(JsonUtf8GetPreviousCode(Content, 3, 3, &PreviousCode), 0, TestResult);
		}
	}

	for (Code = 0x01000000; Code != 0; Code++)
	{
		if (((Code & 0x80) != 0) && ((Code & 0xE0C0) != 0xC080) && ((Code & 0xF0C0C0) != 0xE08080) && ((Code & 0xF8C0C0C0) != 0xF0808080))
		{
			Content[0] = Code >> 24;
			Content[1] = Code >> 16;
			Content[2] = Code >> 8;
			Content[3] = Code;

			TEST_IS_EQ(JsonUtf8GetPreviousCode(Content, 4, 4, &PreviousCode), 0, TestResult);
		}
	}

	for (Code = 0; Code < 0x80; Code++)
	{
		Content[0] = 0xC0 + (Code >> 6);
		Content[1] = 0x80 + (Code & 0x3F);

		TEST_IS_EQ(JsonUtf8GetPreviousCode(Content, 2, 2, &PreviousCode), 0, TestResult);
	}

	for (Code = 0; Code < 0x800; Code++)
	{
		Content[0] = 0xE0 + (Code >> 12);
		Content[1] = 0x80 + ((Code >> 6) & 0x3F);
		Content[2] = 0x80 + (Code & 0x3F);

		TEST_IS_EQ(JsonUtf8GetPreviousCode(Content, 3, 3, &PreviousCode), 0, TestResult);
	}

	for (Code = 0xD800; Code < 0xE000; Code++)
	{
		Content[0] = 0xE0 + (Code >> 12);
		Content[1] = 0x80 + ((Code >> 6) & 0x3F);
		Content[2] = 0x80 + (Code & 0x3F);

		TEST_IS_EQ(JsonUtf8GetPreviousCode(Content, 3, 3, &PreviousCode), 0, TestResult);
	}

	for (Code = 0; Code < 0x10000; Code++)
	{
		Content[0] = 0xF0 + (Code >> 18);
		Content[1] = 0x80 + ((Code >> 12) & 0x3F);
		Content[2] = 0x80 + ((Code >> 6) & 0x3F);
		Content[3] = 0x80 + (Code & 0x3F);

		TEST_IS_EQ(JsonUtf8GetPreviousCode(Content, 4, 4, &PreviousCode), 0, TestResult);
	}

	for (Code = 0x110000; Code < 0x200000; Code++)
	{
		Content[0] = 0xF0 + (Code >> 18);
		Content[1] = 0x80 + ((Code >> 12) & 0x3F);
		Content[2] = 0x80 + ((Code >> 6) & 0x3F);
		Content[3] = 0x80 + (Code & 0x3F);

		TEST_IS_EQ(JsonUtf8GetPreviousCode(Content, 4, 4, &PreviousCode), 0, TestResult);
	}

	return TestResult;
}


static const tTestCase TestCaseJsonUtf8[] =
{
	{ "JsonUtf8Code",              TestJsonUtf8Code              },
	{ "JsonUtf8GetCharacter",      TestJsonUtf8GetCharacter      },
	{ "JsonUtf8CodeIsValid",       TestJsonUtf8CodeIsValid       },
	{ "JsonUtf8CodeGetUnitLength", TestJsonUtf8CodeGetUnitLength },
	{ "JsonUtf8CodeGetUnit",       TestJsonUtf8CodeGetUnit       },
	{ "JsonUtf8CodeAddUnit",       TestJsonUtf8CodeAddUnit       },
	{ "JsonUtf8GetNextCode",       TestJsonUtf8GetNextCode       },
	{ "JsonUtf8GetPreviousCode",   TestJsonUtf8GetPreviousCode   }
};


const tTest TestJsonUtf8 =
{
	"JsonUtf8",
	TestCaseJsonUtf8,
	sizeof(TestCaseJsonUtf8) / sizeof(TestCaseJsonUtf8[0])
};
