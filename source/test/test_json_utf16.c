#include "json_utf16.h"
#include "test_json.h"


static tTestResult TestJsonUtf16Code(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonCharacter Character;

	for (Character = 0; Character < 0xD800; Character++)
	{
		TEST_IS_NOT_EQ(JsonUtf16Code(Character), JSON_UTF16_CODE_REPLACEMENT, TestResult);
	}
	
	for (Character = 0xD800; Character < 0xE000; Character++)
	{
		TEST_IS_EQ(JsonUtf16Code(Character), JSON_UTF16_CODE_REPLACEMENT, TestResult);
	}

	for (Character = 0xE000; Character < JSON_CHARACTER_REPLACEMENT; Character++)
	{
		TEST_IS_NOT_EQ(JsonUtf16Code(Character), JSON_UTF16_CODE_REPLACEMENT, TestResult);
	}

	TEST_IS_EQ(JsonUtf16Code(JSON_CHARACTER_REPLACEMENT), JSON_UTF16_CODE_REPLACEMENT, TestResult);

	for (Character = JSON_CHARACTER_REPLACEMENT + 1; Character < 0x110000; Character++)
	{
		TEST_IS_NOT_EQ(JsonUtf16Code(Character), JSON_UTF16_CODE_REPLACEMENT, TestResult);
	}

	for (Character = 0x110000; Character != 0; Character++)
	{
		TEST_IS_EQ(JsonUtf16Code(Character), JSON_UTF16_CODE_REPLACEMENT, TestResult);
	}

	return TestResult;
}


static tTestResult TestJsonUtf16GetCharacter(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonUtf16Code Code;
	tJsonCharacter ExpectedCharacter = 0;
	tJsonCharacter Character;

	for (Code = 0; Code < JSON_UTF16_CODE_REPLACEMENT; Code++)
	{
		Character = JsonUtf16CodeGetCharacter(Code);
		if (JsonUtf16CodeIsValid(Code))
		{
			TEST_IS_EQ(Character, ExpectedCharacter, TestResult);
			TEST_IS_EQ(JsonUtf16Code(Character), Code, TestResult);
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
	TEST_IS_EQ(JsonUtf16CodeGetCharacter(Code), JSON_CHARACTER_REPLACEMENT, TestResult);
	ExpectedCharacter++;

	for (Code = JSON_UTF16_CODE_REPLACEMENT + 1; Code != 0; Code++)
	{
		Character = JsonUtf16CodeGetCharacter(Code);
		if (JsonUtf16CodeIsValid(Code))
		{
			TEST_IS_EQ(Character, ExpectedCharacter, TestResult);
			TEST_IS_EQ(JsonUtf16Code(Character), Code, TestResult);
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


static tTestResult TestJsonUtf16CodeIsValid(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonUtf16Code Code;

	for (Code = 0; Code < JSON_UTF16_CODE_REPLACEMENT; Code++)
	{
		if (JsonUtf16CodeIsValid(Code))
		{
			TEST_IS_NOT_EQ(JsonUtf16CodeGetCharacter(Code), JSON_CHARACTER_REPLACEMENT, TestResult);
		}
		else
		{
			TEST_IS_EQ(JsonUtf16CodeGetCharacter(Code), JSON_CHARACTER_REPLACEMENT, TestResult);
		}
	}

	TEST_IS_TRUE(JsonUtf16CodeIsValid(JSON_UTF16_CODE_REPLACEMENT), TestResult);

	for (Code = JSON_UTF16_CODE_REPLACEMENT + 1; Code != 0; Code++)
	{
		if (JsonUtf16CodeIsValid(Code))
		{
			TEST_IS_NOT_EQ(JsonUtf16CodeGetCharacter(Code), JSON_CHARACTER_REPLACEMENT, TestResult);
		}
		else
		{
			TEST_IS_EQ(JsonUtf16CodeGetCharacter(Code), JSON_CHARACTER_REPLACEMENT, TestResult);
		}
	}

	return TestResult;
}


static tTestResult TestJsonUtf16UnitIsHighSurrogate(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonUtf16Unit Unit;

	for (Unit = 0; Unit < 0xD800; Unit++)
	{
		TEST_IS_FALSE(JsonUtf16UnitIsHighSurrogate(Unit), TestResult);
	}

	for (Unit = 0xD800; Unit < 0xDC00; Unit++)
	{
		TEST_IS_TRUE(JsonUtf16UnitIsHighSurrogate(Unit), TestResult);
	}

	for (Unit = 0xDC00; Unit != 0; Unit++)
	{
		TEST_IS_FALSE(JsonUtf16UnitIsHighSurrogate(Unit), TestResult);
	}

	return TestResult;
}


static tTestResult TestJsonUtf16UnitIsLowSurrogate(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonUtf16Unit Unit;

	for (Unit = 0; Unit < 0xDC00; Unit++)
	{
		TEST_IS_FALSE(JsonUtf16UnitIsLowSurrogate(Unit), TestResult);
	}

	for (Unit = 0xDC00; Unit < 0xE000; Unit++)
	{
		TEST_IS_TRUE(JsonUtf16UnitIsLowSurrogate(Unit), TestResult);
	}

	for (Unit = 0xE000; Unit != 0; Unit++)
	{
		TEST_IS_FALSE(JsonUtf16UnitIsLowSurrogate(Unit), TestResult);
	}

	return TestResult;
}


static tTestResult TestJsonUtf16UnitSetNibble(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonUtf16Unit Unit;
	uint8_t Nibble;

	Unit = 0x1234;
	for (Nibble = 0; Nibble < 0x10; Nibble++)
	{
		TEST_IS_TRUE(JsonUtf16UnitSetNibble(&Unit, 0, Nibble), TestResult);

		TEST_IS_EQ(Unit, ((tJsonUtf16Unit)Nibble << 12) + 0x0234, TestResult);
	}

	Unit = 0x1234;
	for (Nibble = 0; Nibble < 0x10; Nibble++)
	{
		TEST_IS_TRUE(JsonUtf16UnitSetNibble(&Unit, 1, Nibble), TestResult);

		TEST_IS_EQ(Unit, ((tJsonUtf16Unit)Nibble << 8) + 0x1034, TestResult);
	}

	Unit = 0x1234;
	for (Nibble = 0; Nibble < 0x10; Nibble++)
	{
		TEST_IS_TRUE(JsonUtf16UnitSetNibble(&Unit, 2, Nibble), TestResult);

		TEST_IS_EQ(Unit, ((tJsonUtf16Unit)Nibble << 4) + 0x1204, TestResult);
	}

	Unit = 0x1234;
	for (Nibble = 0; Nibble < 0x10; Nibble++)
	{
		TEST_IS_TRUE(JsonUtf16UnitSetNibble(&Unit, 3, Nibble), TestResult);

		TEST_IS_EQ(Unit, (tJsonUtf16Unit)Nibble + 0x1230, TestResult);
	}

	TEST_IS_FALSE(JsonUtf16UnitSetNibble(&Unit, 0, 0x10), TestResult);

	TEST_IS_FALSE(JsonUtf16UnitSetNibble(&Unit, 4, 0), TestResult);

	return TestResult;
}


static tTestResult TestJsonUtf16CodeAddUnit(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonUtf16Code Code;
	tJsonUtf16Unit Unit1;
	tJsonUtf16Unit Unit2;

	Code = 0;

	TEST_IS_TRUE(JsonUtf16CodeAddUnit(&Code, 0), TestResult);
	TEST_IS_EQ(Code, 0, TestResult);

	for (Unit1 = 1; Unit1 < 0xD800; Unit1++)
	{
		for (Unit2 = 0; Unit2 < 0xDC00; Unit2++)
		{
			Code = 0;

			TEST_IS_TRUE(JsonUtf16CodeAddUnit(&Code, Unit1), TestResult);
			TEST_IS_EQ(Code, Unit1, TestResult);
			TEST_IS_FALSE(JsonUtf16CodeAddUnit(&Code, Unit2), TestResult);
		}
	}
	for (Unit1 = 1; Unit1 < 0xD800; Unit1++)
	{
		for (Unit2 = 0xDC00; Unit2 != 0; Unit2++)
		{
			Code = 0;

			TEST_IS_TRUE(JsonUtf16CodeAddUnit(&Code, Unit1), TestResult);
			TEST_IS_EQ(Code, Unit1, TestResult);
			TEST_IS_FALSE(JsonUtf16CodeAddUnit(&Code, Unit2), TestResult);
		}
	}

	for (Unit1 = 0xD800; Unit1 < 0xDC00; Unit1++)
	{
		for (Unit2 = 0; Unit2 < 0xDC00; Unit2++)
		{
			Code = 0;

			TEST_IS_TRUE(JsonUtf16CodeAddUnit(&Code, Unit1), TestResult);
			TEST_IS_EQ(Code, Unit1, TestResult);
			TEST_IS_FALSE(JsonUtf16CodeAddUnit(&Code, Unit2), TestResult);
		}
	}

	for (Unit1 = 0xD800; Unit1 < 0xDC00; Unit1++)
	{
		for (Unit2 = 0xDC00; Unit2 < 0xE000; Unit2++)
		{
			Code = 0;

			TEST_IS_TRUE(JsonUtf16CodeAddUnit(&Code, Unit1), TestResult);
			TEST_IS_EQ(Code, Unit1, TestResult);
			TEST_IS_TRUE(JsonUtf16CodeAddUnit(&Code, Unit2), TestResult);
			TEST_IS_EQ(Code, ((tJsonUtf16Code)Unit1 << 16) + (tJsonUtf16Code)Unit2, TestResult);
			TEST_IS_FALSE(JsonUtf16CodeAddUnit(&Code, 0), TestResult);
		}
	}

	for (Unit1 = 0xD800; Unit1 < 0xDC00; Unit1++)
	{
		for (Unit2 = 0xE000; Unit2 != 0; Unit2++)
		{
			Code = 0;

			TEST_IS_TRUE(JsonUtf16CodeAddUnit(&Code, Unit1), TestResult);
			TEST_IS_EQ(Code, Unit1, TestResult);
			TEST_IS_FALSE(JsonUtf16CodeAddUnit(&Code, Unit2), TestResult);
		}
	}

	for (Unit1 = 0xDC00; Unit1 < 0xE000; Unit1++)
	{
		Code = 0;

		TEST_IS_FALSE(JsonUtf16CodeAddUnit(&Code, Unit1), TestResult);
	}

	for (Unit1 = 0xE000; Unit1 != 0; Unit1++)
	{
		for (Unit2 = 0; Unit2 < 0xDC00; Unit2++)
		{
			Code = 0;

			TEST_IS_TRUE(JsonUtf16CodeAddUnit(&Code, Unit1), TestResult);
			TEST_IS_EQ(Code, Unit1, TestResult);
			TEST_IS_FALSE(JsonUtf16CodeAddUnit(&Code, Unit2), TestResult);
		}
	}
	for (Unit1 = 0xE000; Unit1 != 0; Unit1++)
	{
		for (Unit2 = 0xDC00; Unit2 != 0; Unit2++)
		{
			Code = 0;

			TEST_IS_TRUE(JsonUtf16CodeAddUnit(&Code, Unit1), TestResult);
			TEST_IS_EQ(Code, Unit1, TestResult);
			TEST_IS_FALSE(JsonUtf16CodeAddUnit(&Code, Unit2), TestResult);
		}
	}

	return TestResult;
}


static tTestResult TestJsonUtf16CodeGetUnitLength(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonUtf16Code Code;

	for (Code = 0; Code < 0xD800; Code++)
	{
		TEST_IS_EQ(JsonUtf16CodeGetUnitLength(Code), 1, TestResult);
	}

	for (Code = 0xD800; Code < 0xE000; Code++)
	{
		TEST_IS_EQ(JsonUtf16CodeGetUnitLength(Code), 0, TestResult);
	}

	for (Code = 0xE000; Code < 0x10000; Code++)
	{
		TEST_IS_EQ(JsonUtf16CodeGetUnitLength(Code), 1, TestResult);
	}

	for (Code = 0x10000; Code < 0xD8000000; Code++)
	{
		TEST_IS_EQ(JsonUtf16CodeGetUnitLength(Code), 0, TestResult);
	}

	for (Code = 0xD8000000; Code < 0xDC000000; Code++)
	{
		if (JsonUtf16CodeIsValid(Code))
		{
			TEST_IS_EQ(JsonUtf16CodeGetUnitLength(Code), 2, TestResult);
		}
		else
		{
			TEST_IS_EQ(JsonUtf16CodeGetUnitLength(Code), 0, TestResult);
		}
	}

	for (Code = 0xDC000000; Code != 0; Code++)
	{
		TEST_IS_EQ(JsonUtf16CodeGetUnitLength(Code), 0, TestResult);
	}

	return TestResult;
}


static tTestResult TestJsonUtf16CodeGetUnit(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonUtf16Code Code;

	for (Code = 0; Code < 0x10000; Code++)
	{
		if (JsonUtf16CodeIsValid(Code))
		{
			TEST_IS_EQ(JsonUtf16CodeGetUnit(Code, 0), Code, TestResult);
		}
		else
		{
			TEST_IS_EQ(JsonUtf16CodeGetUnit(Code, 0), 0x00, TestResult);
		}
	}

	for (Code = 0x10000; Code < 0xD8000000; Code++)
	{
		TEST_IS_EQ(JsonUtf16CodeGetUnit(Code, 0), 0x00, TestResult);
	}

	for (Code = 0xD8000000; Code < 0xDC000000; Code++)
	{
		if (JsonUtf16CodeIsValid(Code))
		{
			TEST_IS_EQ(JsonUtf16CodeGetUnit(Code, 0), Code >> 16, TestResult);
			TEST_IS_EQ(JsonUtf16CodeGetUnit(Code, 1), Code & 0xFFFF, TestResult);
		}
		else
		{
			TEST_IS_EQ(JsonUtf16CodeGetUnit(Code, 0), 0x00, TestResult);
		}
	}

	for (Code = 0xDC000000; Code != 0; Code++)
	{
		TEST_IS_EQ(JsonUtf16CodeGetUnit(Code, 0), 0x00, TestResult);
	}

	return TestResult;
}


static tTestResult TestJsonUtf16CodeGetNibbleLength(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonUtf16Code Code;

	for (Code = 0; Code < 0xD800; Code++)
	{
		TEST_IS_EQ(JsonUtf16CodeGetNibbleLength(Code), 4, TestResult);
	}

	for (Code = 0xD800; Code < 0xE000; Code++)
	{
		TEST_IS_EQ(JsonUtf16CodeGetNibbleLength(Code), 0, TestResult);
	}

	for (Code = 0xE000; Code < 0x10000; Code++)
	{
		TEST_IS_EQ(JsonUtf16CodeGetNibbleLength(Code), 4, TestResult);
	}

	for (Code = 0x10000; Code < 0xD8000000; Code++)
	{
		TEST_IS_EQ(JsonUtf16CodeGetNibbleLength(Code), 0, TestResult);
	}

	for (Code = 0xD8000000; Code < 0xDC000000; Code++)
	{
		if (JsonUtf16CodeIsValid(Code))
		{
			TEST_IS_EQ(JsonUtf16CodeGetNibbleLength(Code), 8, TestResult);
		}
		else
		{
			TEST_IS_EQ(JsonUtf16CodeGetNibbleLength(Code), 0, TestResult);
		}
	}

	for (Code = 0xDC000000; Code != 0; Code++)
	{
		TEST_IS_EQ(JsonUtf16CodeGetNibbleLength(Code), 0, TestResult);
	}

	return TestResult;
}


static tTestResult TestJsonUtf16CodeGetNibble(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonUtf16Code Code;

	for (Code = 0; Code < 0x10000; Code++)
	{
		if (JsonUtf16CodeIsValid(Code))
		{
			TEST_IS_EQ(JsonUtf16CodeGetNibble(Code, 0), (Code >> 12), TestResult);
			TEST_IS_EQ(JsonUtf16CodeGetNibble(Code, 1), ((Code >> 8) & 0x0F), TestResult);
			TEST_IS_EQ(JsonUtf16CodeGetNibble(Code, 2), ((Code >> 4) & 0x0F), TestResult);
			TEST_IS_EQ(JsonUtf16CodeGetNibble(Code, 3), (Code & 0x0F), TestResult);
		}
		else
		{
			TEST_IS_EQ(JsonUtf16CodeGetNibble(Code, 0), 0x00, TestResult);
		}
	}

	for (Code = 0x10000; Code < 0xD8000000; Code++)
	{
		TEST_IS_EQ(JsonUtf16CodeGetNibble(Code, 0), 0x00, TestResult);
	}

	for (Code = 0xD8000000; Code < 0xDC000000; Code++)
	{
		if (JsonUtf16CodeIsValid(Code))
		{
			TEST_IS_EQ(JsonUtf16CodeGetNibble(Code, 0), Code >> 28, TestResult);
			TEST_IS_EQ(JsonUtf16CodeGetNibble(Code, 1), (Code >> 24) & 0x0F, TestResult);
			TEST_IS_EQ(JsonUtf16CodeGetNibble(Code, 2), (Code >> 20) & 0x0F, TestResult);
			TEST_IS_EQ(JsonUtf16CodeGetNibble(Code, 3), (Code >> 16) & 0x0F, TestResult);
			TEST_IS_EQ(JsonUtf16CodeGetNibble(Code, 4), (Code >> 12) & 0x0F, TestResult);
			TEST_IS_EQ(JsonUtf16CodeGetNibble(Code, 5), (Code >> 8) & 0x0F, TestResult);
			TEST_IS_EQ(JsonUtf16CodeGetNibble(Code, 6), (Code >> 4) & 0x0F, TestResult);
			TEST_IS_EQ(JsonUtf16CodeGetNibble(Code, 7), Code & 0x0F, TestResult);
		}
		else
		{
			TEST_IS_EQ(JsonUtf16CodeGetNibble(Code, 0), 0x00, TestResult);
		}
	}

	for (Code = 0xDC000000; Code != 0; Code++)
	{
		TEST_IS_EQ(JsonUtf16CodeGetNibble(Code, 0), 0x00, TestResult);
	}

	return TestResult;
}


static tTestResult TestJsonUtf16CodeAddNibble(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonUtf16Code Code;
	uint8_t Nibble1;
	uint8_t Nibble2;
	uint8_t Nibble3;
	uint8_t Nibble4;
	uint8_t Nibble5;
	uint8_t Nibble6;
	uint8_t Nibble7;
	uint8_t Nibble8;

	for (Nibble1 = 0x00; Nibble1 < 0x0D; Nibble1++)
	{		
		for (Nibble2 = 0x00; Nibble2 < 0x10; Nibble2++)
		{
			for (Nibble3 = 0x00; Nibble3 < 0x10; Nibble3++)
			{
				for (Nibble4 = 0x00; Nibble4 < 0x10; Nibble4++)
				{
					Code = 0;
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble1), TestResult);
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble2), TestResult);
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble3), TestResult);
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble4), TestResult);
					TEST_IS_EQ(Code, ((tJsonUtf16Code)Nibble1 << 12) + ((tJsonUtf16Code)Nibble2 << 8) + ((tJsonUtf16Code)Nibble3 << 4) + (tJsonUtf16Code)Nibble4, TestResult);
					if (Code >= 0x1000)
					{
						TEST_IS_FALSE(JsonUtf16CodeAddNibble(&Code, 0), TestResult);
					}
				}

				for (Nibble4 = 0x10; Nibble4 != 0; Nibble4++)
				{
					Code = 0;
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble1), TestResult);
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble2), TestResult);
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble3), TestResult);
					TEST_IS_FALSE(JsonUtf16CodeAddNibble(&Code, Nibble4), TestResult);
				}
			}

			for (Nibble3 = 0x10; Nibble3 != 0; Nibble3++)
			{
				Code = 0;
				TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble1), TestResult);
				TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble2), TestResult);
				TEST_IS_FALSE(JsonUtf16CodeAddNibble(&Code, Nibble3), TestResult);
			}
		}

		for (Nibble2 = 0x10; Nibble2 != 0; Nibble2++)
		{
			Code = 0;
			TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble1), TestResult);
			TEST_IS_FALSE(JsonUtf16CodeAddNibble(&Code, Nibble2), TestResult);
		}
	}

	for (Nibble1 = 0x0D; Nibble1 < 0x0E; Nibble1++)
	{
		for (Nibble2 = 0x00; Nibble2 < 0x08; Nibble2++)
		{
			for (Nibble3 = 0x0; Nibble3 < 0x10; Nibble3++)
			{
				for (Nibble4 = 0x0; Nibble4 < 0x10; Nibble4++)
				{
					Code = 0;
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble1), TestResult);
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble2), TestResult);
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble3), TestResult);
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble4), TestResult);
					TEST_IS_EQ(Code, ((tJsonUtf16Code)Nibble1 << 12) + ((tJsonUtf16Code)Nibble2 << 8) + ((tJsonUtf16Code)Nibble3 << 4) + (tJsonUtf16Code)Nibble4, TestResult);
					TEST_IS_FALSE(JsonUtf16CodeAddNibble(&Code, 0), TestResult);
				}

				for (Nibble4 = 0x10; Nibble4 != 0; Nibble4++)
				{
					Code = 0;
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble1), TestResult);
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble2), TestResult);
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble3), TestResult);
					TEST_IS_FALSE(JsonUtf16CodeAddNibble(&Code, Nibble4), TestResult);
				}
			}

			for (Nibble3 = 0x10; Nibble3 != 0; Nibble3++)
			{
				Code = 0;
				TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble1), TestResult);
				TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble2), TestResult);
				TEST_IS_FALSE(JsonUtf16CodeAddNibble(&Code, Nibble3), TestResult);
			}
		}

		for (Nibble2 = 0x08; Nibble2 < 0x0C; Nibble2++)
		{
			for (Nibble3 = 0x00; Nibble3 < 0x10; Nibble3++)
			{
				for (Nibble4 = 0x00; Nibble4 < 0x10; Nibble4++)
				{
					for (Nibble5 = 0x00; Nibble5 < 0x0D; Nibble5++)
					{
						Code = 0;
						TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble1), TestResult);
						TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble2), TestResult);
						TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble3), TestResult);
						TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble4), TestResult);
						TEST_IS_FALSE(JsonUtf16CodeAddNibble(&Code, Nibble5), TestResult);
					}

					for (Nibble5 = 0x0D; Nibble5 < 0x0E; Nibble5++)
					{
						for (Nibble6 = 0x00; Nibble6 < 0x0C; Nibble6++)
						{
							Code = 0;
							TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble1), TestResult);
							TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble2), TestResult);
							TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble3), TestResult);
							TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble4), TestResult);
							TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble5), TestResult);
							TEST_IS_FALSE(JsonUtf16CodeAddNibble(&Code, Nibble6), TestResult);
						}

						for (Nibble6 = 0x0C; Nibble6 < 0x010; Nibble6++)
						{
							for (Nibble7 = 0x00; Nibble7 < 0x10; Nibble7++)
							{
								for (Nibble8 = 0x00; Nibble8 < 0x10; Nibble8++)
								{
									Code = 0;
									TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble1), TestResult);
									TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble2), TestResult);
									TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble3), TestResult);
									TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble4), TestResult);
									TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble5), TestResult);
									TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble6), TestResult);
									TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble7), TestResult);
									TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble8), TestResult);
									TEST_IS_EQ(Code, ((tJsonUtf16Code)Nibble1 << 28) + ((tJsonUtf16Code)Nibble2 << 24) + ((tJsonUtf16Code)Nibble3 << 20) + ((tJsonUtf16Code)Nibble4 << 16) +
									                 ((tJsonUtf16Code)Nibble5 << 12) + ((tJsonUtf16Code)Nibble6 << 8) + ((tJsonUtf16Code)Nibble7 << 4) + (tJsonUtf16Code)Nibble8, TestResult);
									TEST_IS_FALSE(JsonUtf16CodeAddNibble(&Code, 0), TestResult);
								}

								for (Nibble8 = 0x10; Nibble8 != 0; Nibble8++)
								{
									Code = 0;
									TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble1), TestResult);
									TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble2), TestResult);
									TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble3), TestResult);
									TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble4), TestResult);
									TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble5), TestResult);
									TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble6), TestResult);
									TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble7), TestResult);
									TEST_IS_FALSE(JsonUtf16CodeAddNibble(&Code, Nibble8), TestResult);
								}
							}

							for (Nibble7 = 0x10; Nibble7 != 0; Nibble7++)
							{
								Code = 0;
								TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble1), TestResult);
								TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble2), TestResult);
								TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble3), TestResult);
								TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble4), TestResult);
								TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble5), TestResult);
								TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble6), TestResult);
								TEST_IS_FALSE(JsonUtf16CodeAddNibble(&Code, Nibble7), TestResult);
							}
						}

						for (Nibble6 = 0x10; Nibble6 != 0; Nibble6++)
						{
							Code = 0;
							TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble1), TestResult);
							TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble2), TestResult);
							TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble3), TestResult);
							TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble4), TestResult);
							TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble5), TestResult);
							TEST_IS_FALSE(JsonUtf16CodeAddNibble(&Code, Nibble6), TestResult);
						}
					}

					for (Nibble5 = 0x0E; Nibble5 != 0; Nibble5++)
					{
						Code = 0;
						TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble1), TestResult);
						TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble2), TestResult);
						TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble3), TestResult);
						TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble4), TestResult);
						TEST_IS_FALSE(JsonUtf16CodeAddNibble(&Code, Nibble5), TestResult);
					}
				}

				for (Nibble4 = 0x10; Nibble4 != 0; Nibble4++)
				{
					Code = 0;
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble1), TestResult);
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble2), TestResult);
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble3), TestResult);
					TEST_IS_FALSE(JsonUtf16CodeAddNibble(&Code, Nibble4), TestResult);
				}
			}

			for (Nibble3 = 0x10; Nibble3 != 0; Nibble3++)
			{
				Code = 0;
				TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble1), TestResult);
				TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble2), TestResult);
				TEST_IS_FALSE(JsonUtf16CodeAddNibble(&Code, Nibble3), TestResult);
			}
		}

		for (Nibble2 = 0x0C; Nibble2 < 0x10; Nibble2++)
		{
			for (Nibble3 = 0x0; Nibble3 < 0x10; Nibble3++)
			{
				for (Nibble4 = 0x0; Nibble4 < 0x10; Nibble4++)
				{
					Code = 0;
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble1), TestResult);
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble2), TestResult);
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble3), TestResult);
					TEST_IS_FALSE(JsonUtf16CodeAddNibble(&Code, Nibble4), TestResult);
				}
				for (Nibble4 = 0x10; Nibble4 != 0; Nibble4++)
				{
					Code = 0;
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble1), TestResult);
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble2), TestResult);
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble3), TestResult);
					TEST_IS_FALSE(JsonUtf16CodeAddNibble(&Code, Nibble4), TestResult);
				}
			}

			for (Nibble3 = 0x10; Nibble3 != 0; Nibble3++)
			{
				Code = 0;
				TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble1), TestResult);
				TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble2), TestResult);
				TEST_IS_FALSE(JsonUtf16CodeAddNibble(&Code, Nibble3), TestResult);
			}
		}

		for (Nibble2 = 0x10; Nibble2 != 0; Nibble2++)
		{
			Code = 0;
			TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble1), TestResult);
			TEST_IS_FALSE(JsonUtf16CodeAddNibble(&Code, Nibble2), TestResult);
		}
	}

	for (Nibble1 = 0x0E; Nibble1 < 0x10; Nibble1++)
	{
		for (Nibble2 = 0x00; Nibble2 < 0x10; Nibble2++)
		{
			for (Nibble3 = 0x00; Nibble3 < 0x10; Nibble3++)
			{
				for (Nibble4 = 0x00; Nibble4 < 0x10; Nibble4++)
				{
					Code = 0;
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble1), TestResult);
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble2), TestResult);
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble3), TestResult);
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble4), TestResult);
					TEST_IS_EQ(Code, ((tJsonUtf16Code)Nibble1 << 12) + ((tJsonUtf16Code)Nibble2 << 8) + ((tJsonUtf16Code)Nibble3 << 4) + (tJsonUtf16Code)Nibble4, TestResult);
					TEST_IS_FALSE(JsonUtf16CodeAddNibble(&Code, 0), TestResult);
				}

				for (Nibble4 = 0x10; Nibble4 != 0; Nibble4++)
				{
					Code = 0;
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble1), TestResult);
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble2), TestResult);
					TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble3), TestResult);
					TEST_IS_FALSE(JsonUtf16CodeAddNibble(&Code, Nibble4), TestResult);
				}
			}

			for (Nibble3 = 0x10; Nibble3 != 0; Nibble3++)
			{
				Code = 0;
				TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble1), TestResult);
				TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble2), TestResult);
				TEST_IS_FALSE(JsonUtf16CodeAddNibble(&Code, Nibble3), TestResult);
			}
		}

		for (Nibble2 = 0x10; Nibble2 != 0; Nibble2++)
		{
			Code = 0;
			TEST_IS_TRUE(JsonUtf16CodeAddNibble(&Code, Nibble1), TestResult);
			TEST_IS_FALSE(JsonUtf16CodeAddNibble(&Code, Nibble2), TestResult);
		}
	}

	for (Nibble1 = 0x10; Nibble1 != 0; Nibble1++)
	{
		Code = 0;
		TEST_IS_FALSE(JsonUtf16CodeAddNibble(&Code, Nibble1), TestResult);
	}

	return TestResult;
}


static const tTestCase TestCaseJsonUtf16[] =
{
	{ "JsonUtf16Code",                TestJsonUtf16Code                },
	{ "JsonUtf16GetCharacter",        TestJsonUtf16GetCharacter        },
	{ "JsonUtf16CodeIsValid",         TestJsonUtf16CodeIsValid         },
	{ "JsonUtf16UnitIsHighSurrogate", TestJsonUtf16UnitIsHighSurrogate },
	{ "JsonUtf16UnitIsLowSurrogate",  TestJsonUtf16UnitIsLowSurrogate  },
	{ "JsonUtf16UnitSetNibble",       TestJsonUtf16UnitSetNibble       },
	{ "JsonUtf16CodeAddUnit",         TestJsonUtf16CodeAddUnit         },
	{ "JsonUtf16CodeGetUnitLength",   TestJsonUtf16CodeGetUnitLength   },
	{ "JsonUtf16CodeGetUnit",         TestJsonUtf16CodeGetUnit         },
	{ "JsonUtf16CodeGetNibbleLength", TestJsonUtf16CodeGetNibbleLength },
	{ "JsonUtf16CodeGetNibble",       TestJsonUtf16CodeGetNibble       },
	{ "JsonUtf16CodeAddNibble",       TestJsonUtf16CodeAddNibble       },
};


const tTest TestJsonUtf16 =
{
	"JsonUtf16",
	TestCaseJsonUtf16,
	sizeof(TestCaseJsonUtf16) / sizeof(TestCaseJsonUtf16[0])
};
