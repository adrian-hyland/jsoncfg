#include "json_utf16.h"
#include "test_json.h"


static bool TestJsonUtf16Code(void)
{
	tJsonCharacter Character;
	bool ok = true;

	for (Character = 0; ok && (Character < 0xD800); Character++)
	{
		ok = (JsonUtf16Code(Character) != JSON_UTF16_CODE_REPLACEMENT);
	}
	
	for (Character = 0xD800; ok && (Character < 0xE000); Character++)
	{
		ok = (JsonUtf16Code(Character) == JSON_UTF16_CODE_REPLACEMENT);
	}

	for (Character = 0xE000; ok && (Character < JSON_CHARACTER_REPLACEMENT); Character++)
	{
		ok = (JsonUtf16Code(Character) != JSON_UTF16_CODE_REPLACEMENT);
	}

	ok = ok && (JsonUtf16Code(JSON_CHARACTER_REPLACEMENT) == JSON_UTF16_CODE_REPLACEMENT);

	for (Character = JSON_CHARACTER_REPLACEMENT + 1; ok && (Character < 0x110000); Character++)
	{
		ok = (JsonUtf16Code(Character) != JSON_UTF16_CODE_REPLACEMENT);
	}

	for (Character = 0x110000; ok && (Character != 0); Character++)
	{
		ok = (JsonUtf16Code(Character) == JSON_UTF16_CODE_REPLACEMENT);
	}

	return ok;
}


static bool TestJsonUtf16GetCharacter(void)
{
	tJsonUtf16Code Code;
	tJsonCharacter ExpectedCharacter = 0;
	tJsonCharacter Character;
	bool ok = true;

	for (Code = 0; ok && (Code < JSON_UTF16_CODE_REPLACEMENT); Code++)
	{
		Character = JsonUtf16CodeGetCharacter(Code);
		ok = (Character == JSON_CHARACTER_REPLACEMENT) || ((Character == ExpectedCharacter++) && (JsonUtf16Code(Character) == Code));
		if (ExpectedCharacter == 0xD800)
		{
			ExpectedCharacter = 0xE000;
		}
	}

	ok = ok && (ExpectedCharacter++ == JSON_CHARACTER_REPLACEMENT);
	ok = ok && (JsonUtf16CodeGetCharacter(Code) == JSON_CHARACTER_REPLACEMENT);

	Character = JsonUtf16CodeGetCharacter(0xD800E000);

	for (Code = JSON_UTF16_CODE_REPLACEMENT + 1; ok && (Code != 0); Code++)
	{
		Character = JsonUtf16CodeGetCharacter(Code);
		ok = (Character == JSON_CHARACTER_REPLACEMENT) || ((Character == ExpectedCharacter++) && (JsonUtf16Code(Character) == Code));
	}

	ok = ok && (ExpectedCharacter == 0x110000);

	return ok;
}


static bool TestJsonUtf16CodeIsValid(void)
{
	tJsonUtf16Code Code;
	bool ok = true;

	for (Code = 0; ok && (Code < JSON_UTF16_CODE_REPLACEMENT); Code++)
	{
		if (JsonUtf16CodeIsValid(Code))
		{
			ok = (JsonUtf16CodeGetCharacter(Code) != JSON_CHARACTER_REPLACEMENT);
		}
		else
		{
			ok = (JsonUtf16CodeGetCharacter(Code) == JSON_CHARACTER_REPLACEMENT);
		}
	}

	ok = ok && JsonUtf16CodeIsValid(JSON_UTF16_CODE_REPLACEMENT);

	for (Code = JSON_UTF16_CODE_REPLACEMENT + 1; ok && (Code != 0); Code++)
	{
		if (JsonUtf16CodeIsValid(Code))
		{
			ok = (JsonUtf16CodeGetCharacter(Code) != JSON_CHARACTER_REPLACEMENT);
		}
		else
		{
			ok = (JsonUtf16CodeGetCharacter(Code) == JSON_CHARACTER_REPLACEMENT);
		}
	}

	return ok;
}


static bool TestJsonUtf16UnitIsHighSurrogate(void)
{
	tJsonUtf16Unit Unit;
	bool ok = true;

	for (Unit = 0; ok && (Unit < 0xD800); Unit++)
	{
		ok = !JsonUtf16UnitIsHighSurrogate(Unit);
	}

	for (Unit = 0xD800; ok && (Unit < 0xDC00); Unit++)
	{
		ok = JsonUtf16UnitIsHighSurrogate(Unit);
	}

	for (Unit = 0xDC00; ok && (Unit != 0); Unit++)
	{
		ok = !JsonUtf16UnitIsHighSurrogate(Unit);
	}

	return ok;
}


static bool TestJsonUtf16UnitIsLowSurrogate(void)
{
	tJsonUtf16Unit Unit;
	bool ok = true;

	for (Unit = 0; ok && (Unit < 0xDC00); Unit++)
	{
		ok = !JsonUtf16UnitIsLowSurrogate(Unit);
	}

	for (Unit = 0xDC00; ok && (Unit < 0xE000); Unit++)
	{
		ok = JsonUtf16UnitIsLowSurrogate(Unit);
	}

	for (Unit = 0xE000; ok && (Unit != 0); Unit++)
	{
		ok = !JsonUtf16UnitIsLowSurrogate(Unit);
	}

	return ok;
}


static bool TestJsonUtf16UnitSetNibble(void)
{
	tJsonUtf16Unit Unit;
	uint8_t Nibble;
	bool ok = true;

	Unit = 0x1234;
	for (Nibble = 0; ok && (Nibble < 0x10); Nibble++)
	{
		ok = JsonUtf16UnitSetNibble(&Unit, 0, Nibble);

		ok = ok && (Unit == ((tJsonUtf16Unit)Nibble << 12) + 0x0234);
	}

	Unit = 0x1234;
	for (Nibble = 0; ok && (Nibble < 0x10); Nibble++)
	{
		ok = JsonUtf16UnitSetNibble(&Unit, 1, Nibble);

		ok = ok && (Unit == ((tJsonUtf16Unit)Nibble << 8) + 0x1034);
	}

	Unit = 0x1234;
	for (Nibble = 0; ok && (Nibble < 0x10); Nibble++)
	{
		ok = JsonUtf16UnitSetNibble(&Unit, 2, Nibble);

		ok = ok && (Unit == ((tJsonUtf16Unit)Nibble << 4) + 0x1204);
	}

	Unit = 0x1234;
	for (Nibble = 0; ok && (Nibble < 0x10); Nibble++)
	{
		ok = JsonUtf16UnitSetNibble(&Unit, 3, Nibble);

		ok = ok && (Unit == (tJsonUtf16Unit)Nibble + 0x1230);
	}

	ok = ok && !JsonUtf16UnitSetNibble(&Unit, 0, 0x10);

	ok = ok && !JsonUtf16UnitSetNibble(&Unit, 4, 0);

	return ok;
}


static bool TestJsonUtf16CodeAddUnit(void)
{
	tJsonUtf16Code Code;
	tJsonUtf16Unit Unit1;
	tJsonUtf16Unit Unit2;
	bool ok = true;

	Code = 0;

	ok = JsonUtf16CodeAddUnit(&Code, 0);
	ok = ok && (Code == 0);

	for (Unit1 = 1; ok && (Unit1 < 0xD800); Unit1++)
	{
		for (Unit2 = 0; ok && (Unit2 < 0xDC00); Unit2++)
		{
			Code = 0;

			ok = JsonUtf16CodeAddUnit(&Code, Unit1);
			ok = ok && (Code == Unit1);
			ok = ok && !JsonUtf16CodeAddUnit(&Code, Unit2);
		}
	}
	for (Unit1 = 1; ok && (Unit1 < 0xD800); Unit1++)
	{
		for (Unit2 = 0xDC00; ok && (Unit2 != 0); Unit2++)
		{
			Code = 0;

			ok = JsonUtf16CodeAddUnit(&Code, Unit1);
			ok = ok && (Code == Unit1);
			ok = ok && !JsonUtf16CodeAddUnit(&Code, Unit2);
		}
	}

	for (Unit1 = 0xD800; ok && (Unit1 < 0xDC00); Unit1++)
	{
		for (Unit2 = 0; ok && (Unit2 < 0xDC00); Unit2++)
		{
			Code = 0;

			ok = JsonUtf16CodeAddUnit(&Code, Unit1);
			ok = ok && (Code == Unit1);
			ok = ok && !JsonUtf16CodeAddUnit(&Code, Unit2);
		}
	}

	for (Unit1 = 0xD800; ok && (Unit1 < 0xDC00); Unit1++)
	{
		for (Unit2 = 0xDC00; ok && (Unit2 < 0xE000); Unit2++)
		{
			Code = 0;

			ok = JsonUtf16CodeAddUnit(&Code, Unit1);
			ok = ok && (Code == Unit1);
			ok = ok && JsonUtf16CodeAddUnit(&Code, Unit2);
			ok = ok && (Code == ((tJsonUtf16Code)Unit1 << 16) + (tJsonUtf16Code)Unit2);
			ok = ok && !JsonUtf16CodeAddUnit(&Code, 0);
		}
	}

	for (Unit1 = 0xD800; ok && (Unit1 < 0xDC00); Unit1++)
	{
		for (Unit2 = 0xE000; ok && (Unit2 != 0); Unit2++)
		{
			Code = 0;

			ok = JsonUtf16CodeAddUnit(&Code, Unit1);
			ok = ok && (Code == Unit1);
			ok = ok && !JsonUtf16CodeAddUnit(&Code, Unit2);
		}
	}

	for (Unit1 = 0xDC00; ok && (Unit1 < 0xE000); Unit1++)
	{
		Code = 0;

		ok = !JsonUtf16CodeAddUnit(&Code, Unit1);
	}

	for (Unit1 = 0xE000; ok && (Unit1 != 0); Unit1++)
	{
		for (Unit2 = 0; ok && (Unit2 < 0xDC00); Unit2++)
		{
			Code = 0;

			ok = JsonUtf16CodeAddUnit(&Code, Unit1);
			ok = ok && (Code == Unit1);
			ok = ok && !JsonUtf16CodeAddUnit(&Code, Unit2);
		}
	}
	for (Unit1 = 0xE000; ok && (Unit1 != 0); Unit1++)
	{
		for (Unit2 = 0xDC00; ok && (Unit2 != 0); Unit2++)
		{
			Code = 0;

			ok = JsonUtf16CodeAddUnit(&Code, Unit1);
			ok = ok && (Code == Unit1);
			ok = ok && !JsonUtf16CodeAddUnit(&Code, Unit2);
		}
	}

	return ok;
}


static bool TestJsonUtf16CodeGetNibbleLength(void)
{
	tJsonUtf16Code Code;
	bool ok = true;

	for (Code = 0; ok && (Code < 0xD800); Code++)
	{
		ok = (JsonUtf16CodeGetNibbleLength(Code) == 4);
	}

	for (Code = 0xD800; ok && (Code < 0xE000); Code++)
	{
		ok = (JsonUtf16CodeGetNibbleLength(Code) == 0);
	}

	for (Code = 0xE000; ok && (Code < 0x10000); Code++)
	{
		ok = (JsonUtf16CodeGetNibbleLength(Code) == 4);
	}

	for (Code = 0x10000; ok && (Code < 0xD8000000); Code++)
	{
		ok = (JsonUtf16CodeGetNibbleLength(Code) == 0);
	}

	for (Code = 0xD8000000; ok && (Code < 0xDC000000); Code++)
	{
		if (JsonUtf16CodeIsValid(Code))
		{
			ok = (JsonUtf16CodeGetNibbleLength(Code) == 8);
		}
		else
		{
			ok = (JsonUtf16CodeGetNibbleLength(Code) == 0);
		}
	}

	for (Code = 0xDC000000; ok && Code != 0; Code++)
	{
		ok = (JsonUtf16CodeGetNibbleLength(Code) == 0);
	}

	return ok;
}


static bool TestJsonUtf16CodeGetNibble(void)
{
	tJsonUtf16Code Code;
	bool ok = true;

	for (Code = 0; ok && (Code < 0x10000); Code++)
	{
		if (JsonUtf16CodeIsValid(Code))
		{
			ok = (JsonUtf16CodeGetNibble(Code, 0) == (Code >> 12));
			ok = ok && (JsonUtf16CodeGetNibble(Code, 1) == ((Code >> 8) & 0x0F));
			ok = ok && (JsonUtf16CodeGetNibble(Code, 2) == ((Code >> 4) & 0x0F));
			ok = ok && (JsonUtf16CodeGetNibble(Code, 3) == (Code & 0x0F));
		}
		else
		{
			ok = (JsonUtf16CodeGetNibble(Code, 0) == 0x00);
		}
	}

	for (Code = 0x10000; ok && (Code < 0xD8000000); Code++)
	{
		ok = (JsonUtf16CodeGetNibble(Code, 0) == 0x00);
	}

	for (Code = 0xD8000000; ok && (Code < 0xDC000000); Code++)
	{
		if (JsonUtf16CodeIsValid(Code))
		{
			ok = (JsonUtf16CodeGetNibble(Code, 0) == (Code >> 28));
			ok = ok && (JsonUtf16CodeGetNibble(Code, 1) == ((Code >> 24) & 0x0F));
			ok = ok && (JsonUtf16CodeGetNibble(Code, 2) == ((Code >> 20) & 0x0F));
			ok = ok && (JsonUtf16CodeGetNibble(Code, 3) == ((Code >> 16) & 0x0F));
			ok = ok && (JsonUtf16CodeGetNibble(Code, 4) == ((Code >> 12) & 0x0F));
			ok = ok && (JsonUtf16CodeGetNibble(Code, 5) == ((Code >> 8) & 0x0F));
			ok = ok && (JsonUtf16CodeGetNibble(Code, 6) == ((Code >> 4) & 0x0F));
			ok = ok && (JsonUtf16CodeGetNibble(Code, 7) == (Code & 0x0F));
		}
		else
		{
			ok = (JsonUtf16CodeGetNibble(Code, 0) == 0x00);
		}
	}

	for (Code = 0xDC000000; ok && (Code != 0); Code++)
	{
		ok = (JsonUtf16CodeGetNibble(Code, 0) == 0x00);
	}

	return ok;
}


static bool TestJsonUtf16CodeAddNibble(void)
{
	tJsonUtf16Code Code;
	uint8_t Nibble1;
	uint8_t Nibble2;
	uint8_t Nibble3;
	uint8_t Nibble4;
	uint8_t Nibble5;
	uint8_t Nibble6;
	uint8_t Nibble7;
	uint8_t Nibble8;
	bool ok = true;

	for (Nibble1 = 0x00; ok && (Nibble1 < 0x0D); Nibble1++)
	{		
		for (Nibble2 = 0x00; ok && (Nibble2 < 0x10); Nibble2++)
		{
			for (Nibble3 = 0x00; ok && (Nibble3 < 0x10); Nibble3++)
			{
				for (Nibble4 = 0x00; ok && (Nibble4 < 0x10); Nibble4++)
				{
					Code = 0;
					ok = JsonUtf16CodeAddNibble(&Code, Nibble1);
					ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble2);
					ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble3);
					ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble4);
					ok = ok && (Code == ((tJsonUtf16Code)Nibble1 << 12) + ((tJsonUtf16Code)Nibble2 << 8) + ((tJsonUtf16Code)Nibble3 << 4) + (tJsonUtf16Code)Nibble4);
					ok = ok && ((Code < 0x1000) || !JsonUtf16CodeAddNibble(&Code, 0));
				}

				for (Nibble4 = 0x10; ok && (Nibble4 != 0); Nibble4++)
				{
					Code = 0;
					ok = JsonUtf16CodeAddNibble(&Code, Nibble1);
					ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble2);
					ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble3);
					ok = ok && !JsonUtf16CodeAddNibble(&Code, Nibble4);
				}
			}

			for (Nibble3 = 0x10; ok && (Nibble3 != 0); Nibble3++)
			{
				Code = 0;
				ok = JsonUtf16CodeAddNibble(&Code, Nibble1);
				ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble2);
				ok = ok && !JsonUtf16CodeAddNibble(&Code, Nibble3);
			}
		}

		for (Nibble2 = 0x10; ok && (Nibble2 != 0); Nibble2++)
		{
			Code = 0;
			ok = JsonUtf16CodeAddNibble(&Code, Nibble1);
			ok = ok && !JsonUtf16CodeAddNibble(&Code, Nibble2);
		}
	}

	for (Nibble1 = 0x0D; ok && (Nibble1 < 0x0E); Nibble1++)
	{
		for (Nibble2 = 0x00; ok && (Nibble2 < 0x08); Nibble2++)
		{
			for (Nibble3 = 0x0; ok && (Nibble3 < 0x10); Nibble3++)
			{
				for (Nibble4 = 0x0; ok && (Nibble4 < 0x10); Nibble4++)
				{
					Code = 0;
					ok = JsonUtf16CodeAddNibble(&Code, Nibble1);
					ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble2);
					ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble3);
					ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble4);
					ok = ok && (Code == ((tJsonUtf16Code)Nibble1 << 12) + ((tJsonUtf16Code)Nibble2 << 8) + ((tJsonUtf16Code)Nibble3 << 4) + (tJsonUtf16Code)Nibble4);
					ok = ok && !JsonUtf16CodeAddNibble(&Code, 0);
				}

				for (Nibble4 = 0x10; ok && (Nibble4 != 0); Nibble4++)
				{
					Code = 0;
					ok = JsonUtf16CodeAddNibble(&Code, Nibble1);
					ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble2);
					ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble3);
					ok = ok && !JsonUtf16CodeAddNibble(&Code, Nibble4);
				}
			}

			for (Nibble3 = 0x10; ok && (Nibble3 != 0); Nibble3++)
			{
				Code = 0;
				ok = JsonUtf16CodeAddNibble(&Code, Nibble1);
				ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble2);
				ok = ok && !JsonUtf16CodeAddNibble(&Code, Nibble3);
			}
		}

		for (Nibble2 = 0x08; ok && (Nibble2 < 0x0C); Nibble2++)
		{
			for (Nibble3 = 0x00; ok && (Nibble3 < 0x10); Nibble3++)
			{
				for (Nibble4 = 0x00; ok && (Nibble4 < 0x10); Nibble4++)
				{
					for (Nibble5 = 0x00; ok && (Nibble5 < 0x0D); Nibble5++)
					{
						Code = 0;
						ok = JsonUtf16CodeAddNibble(&Code, Nibble1);
						ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble2);
						ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble3);
						ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble4);
						ok = ok && !JsonUtf16CodeAddNibble(&Code, Nibble5);
					}

					for (Nibble5 = 0x0D; ok && (Nibble5 < 0x0E); Nibble5++)
					{
						for (Nibble6 = 0x00; ok && (Nibble6 < 0x0C); Nibble6++)
						{
							Code = 0;
							ok = JsonUtf16CodeAddNibble(&Code, Nibble1);
							ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble2);
							ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble3);
							ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble4);
							ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble5);
							ok = ok && !JsonUtf16CodeAddNibble(&Code, Nibble6);
						}

						for (Nibble6 = 0x0C; ok && (Nibble6 < 0x010); Nibble6++)
						{
							for (Nibble7 = 0x00; ok && (Nibble7 < 0x10); Nibble7++)
							{
								for (Nibble8 = 0x00; ok && (Nibble8 < 0x10); Nibble8++)
								{
									Code = 0;
									ok = JsonUtf16CodeAddNibble(&Code, Nibble1);
									ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble2);
									ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble3);
									ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble4);
									ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble5);
									ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble6);
									ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble7);
									ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble8);
									ok = ok && (Code == ((tJsonUtf16Code)Nibble1 << 28) + ((tJsonUtf16Code)Nibble2 << 24) + ((tJsonUtf16Code)Nibble3 << 20) + ((tJsonUtf16Code)Nibble4 << 16) +
									                    ((tJsonUtf16Code)Nibble5 << 12) + ((tJsonUtf16Code)Nibble6 << 8) + ((tJsonUtf16Code)Nibble7 << 4) + (tJsonUtf16Code)Nibble8);
									ok = ok && !JsonUtf16CodeAddNibble(&Code, 0);
								}

								for (Nibble8 = 0x10; ok && (Nibble8 != 0); Nibble8++)
								{
									Code = 0;
									ok = JsonUtf16CodeAddNibble(&Code, Nibble1);
									ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble2);
									ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble3);
									ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble4);
									ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble5);
									ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble6);
									ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble7);
									ok = ok && !JsonUtf16CodeAddNibble(&Code, Nibble8);
								}
							}

							for (Nibble7 = 0x10; ok && (Nibble7 != 0); Nibble7++)
							{
								Code = 0;
								ok = JsonUtf16CodeAddNibble(&Code, Nibble1);
								ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble2);
								ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble3);
								ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble4);
								ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble5);
								ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble6);
								ok = ok && !JsonUtf16CodeAddNibble(&Code, Nibble7);
							}
						}

						for (Nibble6 = 0x10; ok && (Nibble6 != 0); Nibble6++)
						{
							Code = 0;
							ok = JsonUtf16CodeAddNibble(&Code, Nibble1);
							ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble2);
							ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble3);
							ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble4);
							ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble5);
							ok = ok && !JsonUtf16CodeAddNibble(&Code, Nibble6);
						}
					}

					for (Nibble5 = 0x0E; ok && (Nibble5 != 0); Nibble5++)
					{
						Code = 0;
						ok = JsonUtf16CodeAddNibble(&Code, Nibble1);
						ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble2);
						ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble3);
						ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble4);
						ok = ok && !JsonUtf16CodeAddNibble(&Code, Nibble5);
					}
				}

				for (Nibble4 = 0x10; ok && (Nibble4 != 0); Nibble4++)
				{
					Code = 0;
					ok = JsonUtf16CodeAddNibble(&Code, Nibble1);
					ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble2);
					ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble3);
					ok = ok && !JsonUtf16CodeAddNibble(&Code, Nibble4);
				}
			}

			for (Nibble3 = 0x10; ok && (Nibble3 != 0); Nibble3++)
			{
				Code = 0;
				ok = JsonUtf16CodeAddNibble(&Code, Nibble1);
				ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble2);
				ok = ok && !JsonUtf16CodeAddNibble(&Code, Nibble3);
			}
		}

		for (Nibble2 = 0x0C; ok && (Nibble2 < 0x10); Nibble2++)
		{
			for (Nibble3 = 0x0; ok && (Nibble3 < 0x10); Nibble3++)
			{
				for (Nibble4 = 0x0; ok && (Nibble4 < 0x10); Nibble4++)
				{
					Code = 0;
					ok = JsonUtf16CodeAddNibble(&Code, Nibble1);
					ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble2);
					ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble3);
					ok = ok && !JsonUtf16CodeAddNibble(&Code, Nibble4);
				}
				for (Nibble4 = 0x10; ok && (Nibble4 != 0); Nibble4++)
				{
					Code = 0;
					ok = JsonUtf16CodeAddNibble(&Code, Nibble1);
					ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble2);
					ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble3);
					ok = ok && !JsonUtf16CodeAddNibble(&Code, Nibble4);
				}
			}

			for (Nibble3 = 0x10; ok && (Nibble3 != 0); Nibble3++)
			{
				Code = 0;
				ok = JsonUtf16CodeAddNibble(&Code, Nibble1);
				ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble2);
				ok = ok && !JsonUtf16CodeAddNibble(&Code, Nibble3);
			}
		}

		for (Nibble2 = 0x10; ok && (Nibble2 != 0); Nibble2++)
		{
			Code = 0;
			ok = JsonUtf16CodeAddNibble(&Code, Nibble1);
			ok = ok && !JsonUtf16CodeAddNibble(&Code, Nibble2);
		}
	}

	for (Nibble1 = 0x0E; ok && (Nibble1 < 0x10); Nibble1++)
	{
		for (Nibble2 = 0x00; ok && (Nibble2 < 0x10); Nibble2++)
		{
			for (Nibble3 = 0x00; ok && (Nibble3 < 0x10); Nibble3++)
			{
				for (Nibble4 = 0x00; ok && (Nibble4 < 0x10); Nibble4++)
				{
					Code = 0;
					ok = JsonUtf16CodeAddNibble(&Code, Nibble1);
					ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble2);
					ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble3);
					ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble4);
					ok = ok && (Code == ((tJsonUtf16Code)Nibble1 << 12) + ((tJsonUtf16Code)Nibble2 << 8) + ((tJsonUtf16Code)Nibble3 << 4) + (tJsonUtf16Code)Nibble4);
					ok = ok && !JsonUtf16CodeAddNibble(&Code, 0);
				}

				for (Nibble4 = 0x10; ok && (Nibble4 != 0); Nibble4++)
				{
					Code = 0;
					ok = JsonUtf16CodeAddNibble(&Code, Nibble1);
					ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble2);
					ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble3);
					ok = ok && !JsonUtf16CodeAddNibble(&Code, Nibble4);
				}
			}

			for (Nibble3 = 0x10; ok && (Nibble3 != 0); Nibble3++)
			{
				Code = 0;
				ok = JsonUtf16CodeAddNibble(&Code, Nibble1);
				ok = ok && JsonUtf16CodeAddNibble(&Code, Nibble2);
				ok = ok && !JsonUtf16CodeAddNibble(&Code, Nibble3);
			}
		}

		for (Nibble2 = 0x10; ok && (Nibble2 != 0); Nibble2++)
		{
			Code = 0;
			ok = JsonUtf16CodeAddNibble(&Code, Nibble1);
			ok = ok && !JsonUtf16CodeAddNibble(&Code, Nibble2);
		}
	}

	for (Nibble1 = 0x10; ok && (Nibble1 != 0); Nibble1++)
	{
		Code = 0;
		ok = !JsonUtf16CodeAddNibble(&Code, Nibble1);
	}

	return ok;
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
