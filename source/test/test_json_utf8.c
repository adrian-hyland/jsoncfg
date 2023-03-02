#include "json_utf8.h"
#include "test_json.h"


static bool TestJsonUtf8Code(void)
{
	tJsonCharacter Character;
	bool ok = true;

	for (Character = 0; ok && (Character < 0xD800); Character++)
	{
		ok = (JsonUtf8Code(Character) != JSON_UTF8_CODE_REPLACEMENT);
	}
	
	for (Character = 0xD800; ok && (Character < 0xE000); Character++)
	{
		ok = (JsonUtf8Code(Character) == JSON_UTF8_CODE_REPLACEMENT);
	}

	for (Character = 0xE000; ok && (Character < JSON_CHARACTER_REPLACEMENT); Character++)
	{
		ok = (JsonUtf8Code(Character) != JSON_UTF8_CODE_REPLACEMENT);
	}

	ok = ok && (JsonUtf8Code(JSON_CHARACTER_REPLACEMENT) == JSON_UTF8_CODE_REPLACEMENT);

	for (Character = JSON_CHARACTER_REPLACEMENT + 1; ok && (Character < 0x110000); Character++)
	{
		ok = (JsonUtf8Code(Character) != JSON_UTF8_CODE_REPLACEMENT);
	}

	for (Character = 0x110000; ok && (Character != 0); Character++)
	{
		ok = (JsonUtf8Code(Character) == JSON_UTF8_CODE_REPLACEMENT);
	}

	return ok;
}


static bool TestJsonUtf8GetCharacter(void)
{
	tJsonUtf8Code Code;
	tJsonCharacter ExpectedCharacter = 0;
	tJsonCharacter Character;
	bool ok = true;

	for (Code = 0; ok && (Code < JSON_UTF8_CODE_REPLACEMENT); Code++)
	{
		Character = JsonUtf8CodeGetCharacter(Code);
		ok = (Character == JSON_CHARACTER_REPLACEMENT) || ((Character == ExpectedCharacter++) && (JsonUtf8Code(Character) == Code));
		if (ExpectedCharacter == 0xD800)
		{
			ExpectedCharacter = 0xE000;
		}
	}

	ok = ok && (ExpectedCharacter++ == JSON_CHARACTER_REPLACEMENT);
	ok = ok && (JsonUtf8CodeGetCharacter(Code) == JSON_CHARACTER_REPLACEMENT);

	for (Code = JSON_UTF8_CODE_REPLACEMENT + 1; ok && (Code != 0); Code++)
	{
		Character = JsonUtf8CodeGetCharacter(Code);
		ok = (Character == JSON_CHARACTER_REPLACEMENT) || ((Character == ExpectedCharacter++) && (JsonUtf8Code(Character) == Code));
	}

	ok = ok && (ExpectedCharacter == 0x110000);

	return ok;
}


static bool TestJsonUtf8CodeIsValid(void)
{
	tJsonUtf8Code Code;
	bool ok = true;

	for (Code = 0; ok && (Code < JSON_UTF8_CODE_REPLACEMENT); Code++)
	{
		if (JsonUtf8CodeIsValid(Code))
		{
			ok = (JsonUtf8CodeGetCharacter(Code) != JSON_CHARACTER_REPLACEMENT);
		}
		else
		{
			ok = (JsonUtf8CodeGetCharacter(Code) == JSON_CHARACTER_REPLACEMENT);
		}
	}

	ok = ok && JsonUtf8CodeIsValid(JSON_UTF8_CODE_REPLACEMENT);

	for (Code = JSON_UTF8_CODE_REPLACEMENT + 1; ok && (Code != 0); Code++)
	{
		if (JsonUtf8CodeIsValid(Code))
		{
			ok = (JsonUtf8CodeGetCharacter(Code) != JSON_CHARACTER_REPLACEMENT);
		}
		else
		{
			ok = (JsonUtf8CodeGetCharacter(Code) == JSON_CHARACTER_REPLACEMENT);
		}
	}

	return ok;
}


static bool TestJsonUtf8CodeGetUnitLength(void)
{
	tJsonUtf8Code Code;
	bool ok = true;

	for (Code = 0; ok && (Code < 0xF4908080); Code++)
	{
		if (JsonUtf8CodeIsValid(Code))
		{
			if (Code < 0x100)
			{
				ok = (JsonUtf8CodeGetUnitLength(Code) == 1);
			}
			else if (Code < 0x10000)
			{
				ok = (JsonUtf8CodeGetUnitLength(Code) == 2);
			}
			else if (Code < 0x1000000)
			{
				ok = (JsonUtf8CodeGetUnitLength(Code) == 3);
			}
			else
			{
				ok = (JsonUtf8CodeGetUnitLength(Code) == 4);
			}
		}
		else
		{
			ok = (JsonUtf8CodeGetUnitLength(Code) == 0);
		}
	}

	for (Code = 0xF4908080; ok && Code != 0; Code++)
	{
		ok = (JsonUtf8CodeGetUnitLength(Code) == 0);
	}

	return ok;
}


static bool TestJsonUtf8CodeGetUnit(void)
{
	tJsonUtf8Code Code;
	bool ok = true;

	for (Code = 0; ok && (Code < 0xF4908080); Code++)
	{
		if (JsonUtf8CodeIsValid(Code))
		{
			if (Code < 0x100)
			{
				ok = (JsonUtf8CodeGetUnit(Code, 0) == Code);
			}
			else if (Code < 0x10000)
			{
				ok = (JsonUtf8CodeGetUnit(Code, 0) == (Code >> 8));
				ok = ok && (JsonUtf8CodeGetUnit(Code, 1) == (Code & 0xFF));
			}
			else if (Code < 0x1000000)
			{
				ok = (JsonUtf8CodeGetUnit(Code, 0) == (Code >> 16));
				ok = ok && (JsonUtf8CodeGetUnit(Code, 1) == ((Code >> 8) & 0xFF));
				ok = ok && (JsonUtf8CodeGetUnit(Code, 2) == (Code & 0xFF));
			}
			else
			{
				ok = (JsonUtf8CodeGetUnit(Code, 0) == (Code >> 24));
				ok = ok && (JsonUtf8CodeGetUnit(Code, 1) == ((Code >> 16) & 0xFF));
				ok = ok && (JsonUtf8CodeGetUnit(Code, 2) == ((Code >> 8) & 0xFF));
				ok = ok && (JsonUtf8CodeGetUnit(Code, 3) == (Code & 0xFF));
			}
		}
		else
		{
			ok = (JsonUtf8CodeGetUnit(Code, 0) == 0x00);
		}
	}

	for (Code = 0xF4908080; ok && (Code != 0); Code++)
	{
		ok = (JsonUtf8CodeGetUnit(Code, 0) == 0x00);
	}

	return ok;
}


static bool TestJsonUtf8CodeAddUnit(void)
{
	tJsonUtf8Code Code;
	tJsonUtf8Unit Unit1;
	tJsonUtf8Unit Unit2;
	tJsonUtf8Unit Unit3;
	tJsonUtf8Unit Unit4;
	bool ok = true;

	for (Unit1 = 0; ok && (Unit1 < 0x80); Unit1++)
	{
		Code = 0;
		ok = JsonUtf8CodeAddUnit(&Code, Unit1);
		ok = ok && (Code == Unit1);
		ok = ok && !JsonUtf8CodeAddUnit(&Code, 0x80);
	}

	for (Unit1 = 0x80; ok && (Unit1 < 0xC2); Unit1++)
	{
		Code = 0;
		ok = !JsonUtf8CodeAddUnit(&Code, Unit1);
	}

	for (Unit1 = 0xC2; ok && (Unit1 < 0xE0); Unit1++)
	{
		for (Unit2 = 0; ok && (Unit2 < 0x80); Unit2++)
		{
			Code = 0;
			ok = JsonUtf8CodeAddUnit(&Code, Unit1);
			ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit2);
		}

		for (Unit2 = 0x80; ok && (Unit2 < 0xC0); Unit2++)
		{
			Code = 0;
			ok = JsonUtf8CodeAddUnit(&Code, Unit1);
			ok = ok && JsonUtf8CodeAddUnit(&Code, Unit2);
			ok = ok && (Code == ((tJsonUtf8Code)Unit1 << 8) + (tJsonUtf8Code)Unit2);
			ok = ok && !JsonUtf8CodeAddUnit(&Code, 0x80);
		}

		for (Unit2 = 0xC0; ok && (Unit2 != 0); Unit2++)
		{
			Code = 0;
			ok = JsonUtf8CodeAddUnit(&Code, Unit1);
			ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit2);
		}
	}

	for (Unit1 = 0xE0; ok && (Unit1 < 0xE1); Unit1++)
	{
		for (Unit2 = 0; ok && (Unit2 < 0xA0); Unit2++)
		{
			Code = 0;
			ok = JsonUtf8CodeAddUnit(&Code, Unit1);
			ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit2);
		}

		for (Unit2 = 0xA0; ok && (Unit2 < 0xC0); Unit2++)
		{
			for (Unit3 = 0; ok && (Unit2 < 0x80); Unit2++)
			{
				Code = 0;
				ok = JsonUtf8CodeAddUnit(&Code, Unit1);
				ok = ok && JsonUtf8CodeAddUnit(&Code, Unit2);
				ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit3);
			}

			for (Unit3 = 0x80; ok && (Unit3 < 0xC0); Unit3++)
			{
				Code = 0;
				ok = JsonUtf8CodeAddUnit(&Code, Unit1);
				ok = ok && JsonUtf8CodeAddUnit(&Code, Unit2);
				ok = ok && JsonUtf8CodeAddUnit(&Code, Unit3);
				ok = ok && (Code == ((tJsonUtf8Code)Unit1 << 16) + ((tJsonUtf8Code)Unit2 << 8) + (tJsonUtf8Code)Unit3);
				ok = ok && !JsonUtf8CodeAddUnit(&Code, 0x80);
			}

			for (Unit3 = 0xC0; ok && (Unit3 != 0); Unit3++)
			{
				Code = 0;
				ok = JsonUtf8CodeAddUnit(&Code, Unit1);
				ok = ok && JsonUtf8CodeAddUnit(&Code, Unit2);
				ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit3);
			}
		}

		for (Unit2 = 0xC0; ok && (Unit2 != 0); Unit2++)
		{
			Code = 0;
			ok = JsonUtf8CodeAddUnit(&Code, Unit1);
			ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit2);
		}
	}

	for (Unit1 = 0xE1; ok && (Unit1 < 0xED); Unit1++)
	{
		for (Unit2 = 0; ok && (Unit2 < 0x80); Unit2++)
		{
			Code = 0;
			ok = JsonUtf8CodeAddUnit(&Code, Unit1);
			ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit2);
		}

		for (Unit2 = 0x80; ok && (Unit2 < 0xC0); Unit2++)
		{
			for (Unit3 = 0; ok && (Unit2 < 0x80); Unit2++)
			{
				Code = 0;
				ok = JsonUtf8CodeAddUnit(&Code, Unit1);
				ok = ok && JsonUtf8CodeAddUnit(&Code, Unit2);
				ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit3);
			}

			for (Unit3 = 0x80; ok && (Unit3 < 0xC0); Unit3++)
			{
				Code = 0;
				ok = JsonUtf8CodeAddUnit(&Code, Unit1);
				ok = ok && JsonUtf8CodeAddUnit(&Code, Unit2);
				ok = ok && JsonUtf8CodeAddUnit(&Code, Unit3);
				ok = ok && (Code == ((tJsonUtf8Code)Unit1 << 16) + ((tJsonUtf8Code)Unit2 << 8) + (tJsonUtf8Code)Unit3);
				ok = ok && !JsonUtf8CodeAddUnit(&Code, 0x80);
			}

			for (Unit3 = 0xC0; ok && (Unit3 != 0); Unit3++)
			{
				Code = 0;
				ok = JsonUtf8CodeAddUnit(&Code, Unit1);
				ok = ok && JsonUtf8CodeAddUnit(&Code, Unit2);
				ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit3);
			}
		}

		for (Unit2 = 0xC0; ok && (Unit2 != 0); Unit2++)
		{
			Code = 0;
			ok = JsonUtf8CodeAddUnit(&Code, Unit1);
			ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit2);
		}
	}

	for (Unit1 = 0xED; ok && (Unit1 < 0xEE); Unit1++)
	{
		for (Unit2 = 0; ok && (Unit2 < 0x80); Unit2++)
		{
			Code = 0;
			ok = JsonUtf8CodeAddUnit(&Code, Unit1);
			ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit2);
		}

		for (Unit2 = 0x80; ok && (Unit2 < 0xA0); Unit2++)
		{
			for (Unit3 = 0; ok && (Unit2 < 0x80); Unit2++)
			{
				Code = 0;
				ok = JsonUtf8CodeAddUnit(&Code, Unit1);
				ok = ok && JsonUtf8CodeAddUnit(&Code, Unit2);
				ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit3);
			}

			for (Unit3 = 0x80; ok && (Unit3 < 0xC0); Unit3++)
			{
				Code = 0;
				ok = JsonUtf8CodeAddUnit(&Code, Unit1);
				ok = ok && JsonUtf8CodeAddUnit(&Code, Unit2);
				ok = ok && JsonUtf8CodeAddUnit(&Code, Unit3);
				ok = ok && (Code == ((tJsonUtf8Code)Unit1 << 16) + ((tJsonUtf8Code)Unit2 << 8) + (tJsonUtf8Code)Unit3);
				ok = ok && !JsonUtf8CodeAddUnit(&Code, 0x80);
			}

			for (Unit3 = 0xC0; ok && (Unit3 != 0); Unit3++)
			{
				Code = 0;
				ok = JsonUtf8CodeAddUnit(&Code, Unit1);
				ok = ok && JsonUtf8CodeAddUnit(&Code, Unit2);
				ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit3);
			}
		}

		for (Unit2 = 0xA0; ok && (Unit2 != 0); Unit2++)
		{
			Code = 0;
			ok = JsonUtf8CodeAddUnit(&Code, Unit1);
			ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit2);
		}
	}

	for (Unit1 = 0xEE; ok && (Unit1 < 0xF0); Unit1++)
	{
		for (Unit2 = 0; ok && (Unit2 < 0x80); Unit2++)
		{
			Code = 0;
			ok = JsonUtf8CodeAddUnit(&Code, Unit1);
			ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit2);
		}

		for (Unit2 = 0x80; ok && (Unit2 < 0xC0); Unit2++)
		{
			for (Unit3 = 0; ok && (Unit2 < 0x80); Unit2++)
			{
				Code = 0;
				ok = JsonUtf8CodeAddUnit(&Code, Unit1);
				ok = ok && JsonUtf8CodeAddUnit(&Code, Unit2);
				ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit3);
			}

			for (Unit3 = 0x80; ok && (Unit3 < 0xC0); Unit3++)
			{
				Code = 0;
				ok = JsonUtf8CodeAddUnit(&Code, Unit1);
				ok = ok && JsonUtf8CodeAddUnit(&Code, Unit2);
				ok = ok && JsonUtf8CodeAddUnit(&Code, Unit3);
				ok = ok && (Code == ((tJsonUtf8Code)Unit1 << 16) + ((tJsonUtf8Code)Unit2 << 8) + (tJsonUtf8Code)Unit3);
				ok = ok && !JsonUtf8CodeAddUnit(&Code, 0x80);
			}

			for (Unit3 = 0xC0; ok && (Unit3 != 0); Unit3++)
			{
				Code = 0;
				ok = JsonUtf8CodeAddUnit(&Code, Unit1);
				ok = ok && JsonUtf8CodeAddUnit(&Code, Unit2);
				ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit3);
			}
		}

		for (Unit2 = 0xC0; ok && (Unit2 != 0); Unit2++)
		{
			Code = 0;
			ok = JsonUtf8CodeAddUnit(&Code, Unit1);
			ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit2);
		}
	}

	for (Unit1 = 0xF0; ok && (Unit1 < 0xF1); Unit1++)
	{
		for (Unit2 = 0; ok && (Unit2 < 0x90); Unit2++)
		{
			Code = 0;
			ok = JsonUtf8CodeAddUnit(&Code, Unit1);
			ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit2);
		}

		for (Unit2 = 0x90; ok && (Unit2 < 0xC0); Unit2++)
		{
			for (Unit3 = 0; ok && (Unit2 < 0x80); Unit2++)
			{
				Code = 0;
				ok = JsonUtf8CodeAddUnit(&Code, Unit1);
				ok = ok && JsonUtf8CodeAddUnit(&Code, Unit2);
				ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit3);
			}

			for (Unit3 = 0x80; ok && (Unit3 < 0xC0); Unit3++)
			{
				for (Unit4 = 0x00; ok && (Unit4 < 0x80); Unit4++)
				{
					Code = 0;
					ok = JsonUtf8CodeAddUnit(&Code, Unit1);
					ok = ok && JsonUtf8CodeAddUnit(&Code, Unit2);
					ok = ok && JsonUtf8CodeAddUnit(&Code, Unit3);
					ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit4);
				}

				for (Unit4 = 0x80; ok && (Unit4 < 0xC0); Unit4++)
				{
					Code = 0;
					ok = JsonUtf8CodeAddUnit(&Code, Unit1);
					ok = ok && JsonUtf8CodeAddUnit(&Code, Unit2);
					ok = ok && JsonUtf8CodeAddUnit(&Code, Unit3);
					ok = ok && JsonUtf8CodeAddUnit(&Code, Unit4);
					ok = ok && (Code == ((tJsonUtf8Code)Unit1 << 24) + ((tJsonUtf8Code)Unit2 << 16) + ((tJsonUtf8Code)Unit3 << 8) + (tJsonUtf8Code)Unit4);
					ok = ok && !JsonUtf8CodeAddUnit(&Code, 0x80);
				}
			}

			for (Unit3 = 0xC0; ok && (Unit3 != 0); Unit3++)
			{
				Code = 0;
				ok = JsonUtf8CodeAddUnit(&Code, Unit1);
				ok = ok && JsonUtf8CodeAddUnit(&Code, Unit2);
				ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit3);
			}
		}

		for (Unit2 = 0xC0; ok && (Unit2 != 0); Unit2++)
		{
			Code = 0;
			ok = JsonUtf8CodeAddUnit(&Code, Unit1);
			ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit2);
		}
	}

	for (Unit1 = 0xF1; ok && (Unit1 < 0xF4); Unit1++)
	{
		for (Unit2 = 0; ok && (Unit2 < 0x80); Unit2++)
		{
			Code = 0;
			ok = JsonUtf8CodeAddUnit(&Code, Unit1);
			ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit2);
		}

		for (Unit2 = 0x80; ok && (Unit2 < 0xC0); Unit2++)
		{
			for (Unit3 = 0; ok && (Unit2 < 0x80); Unit2++)
			{
				Code = 0;
				ok = JsonUtf8CodeAddUnit(&Code, Unit1);
				ok = ok && JsonUtf8CodeAddUnit(&Code, Unit2);
				ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit3);
			}

			for (Unit3 = 0x80; ok && (Unit3 < 0xC0); Unit3++)
			{
				for (Unit4 = 0x00; ok && (Unit4 < 0x80); Unit4++)
				{
					Code = 0;
					ok = JsonUtf8CodeAddUnit(&Code, Unit1);
					ok = ok && JsonUtf8CodeAddUnit(&Code, Unit2);
					ok = ok && JsonUtf8CodeAddUnit(&Code, Unit3);
					ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit4);
				}

				for (Unit4 = 0x80; ok && (Unit4 < 0xC0); Unit4++)
				{
					Code = 0;
					ok = JsonUtf8CodeAddUnit(&Code, Unit1);
					ok = ok && JsonUtf8CodeAddUnit(&Code, Unit2);
					ok = ok && JsonUtf8CodeAddUnit(&Code, Unit3);
					ok = ok && JsonUtf8CodeAddUnit(&Code, Unit4);
					ok = ok && (Code == ((tJsonUtf8Code)Unit1 << 24) + ((tJsonUtf8Code)Unit2 << 16) + ((tJsonUtf8Code)Unit3 << 8) + (tJsonUtf8Code)Unit4);
					ok = ok && !JsonUtf8CodeAddUnit(&Code, 0x80);
				}
			}

			for (Unit3 = 0xC0; ok && (Unit3 != 0); Unit3++)
			{
				Code = 0;
				ok = JsonUtf8CodeAddUnit(&Code, Unit1);
				ok = ok && JsonUtf8CodeAddUnit(&Code, Unit2);
				ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit3);
			}
		}

		for (Unit2 = 0xC0; ok && (Unit2 != 0); Unit2++)
		{
			Code = 0;
			ok = JsonUtf8CodeAddUnit(&Code, Unit1);
			ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit2);
		}
	}

	for (Unit1 = 0xF4; ok && (Unit1 < 0xF5); Unit1++)
	{
		for (Unit2 = 0; ok && (Unit2 < 0x80); Unit2++)
		{
			Code = 0;
			ok = JsonUtf8CodeAddUnit(&Code, Unit1);
			ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit2);
		}

		for (Unit2 = 0x80; ok && (Unit2 < 0x90); Unit2++)
		{
			for (Unit3 = 0; ok && (Unit2 < 0x80); Unit2++)
			{
				Code = 0;
				ok = JsonUtf8CodeAddUnit(&Code, Unit1);
				ok = ok && JsonUtf8CodeAddUnit(&Code, Unit2);
				ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit3);
			}

			for (Unit3 = 0x80; ok && (Unit3 < 0xC0); Unit3++)
			{
				for (Unit4 = 0x00; ok && (Unit4 < 0x80); Unit4++)
				{
					Code = 0;
					ok = JsonUtf8CodeAddUnit(&Code, Unit1);
					ok = ok && JsonUtf8CodeAddUnit(&Code, Unit2);
					ok = ok && JsonUtf8CodeAddUnit(&Code, Unit3);
					ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit4);
				}

				for (Unit4 = 0x80; ok && (Unit4 < 0xC0); Unit4++)
				{
					Code = 0;
					ok = JsonUtf8CodeAddUnit(&Code, Unit1);
					ok = ok && JsonUtf8CodeAddUnit(&Code, Unit2);
					ok = ok && JsonUtf8CodeAddUnit(&Code, Unit3);
					ok = ok && JsonUtf8CodeAddUnit(&Code, Unit4);
					ok = ok && (Code == ((tJsonUtf8Code)Unit1 << 24) + ((tJsonUtf8Code)Unit2 << 16) + ((tJsonUtf8Code)Unit3 << 8) + (tJsonUtf8Code)Unit4);
					ok = ok && !JsonUtf8CodeAddUnit(&Code, 0x80);
				}
			}

			for (Unit3 = 0xC0; ok && (Unit3 != 0); Unit3++)
			{
				Code = 0;
				ok = JsonUtf8CodeAddUnit(&Code, Unit1);
				ok = ok && JsonUtf8CodeAddUnit(&Code, Unit2);
				ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit3);
			}
		}

		for (Unit2 = 0x90; ok && (Unit2 != 0); Unit2++)
		{
			Code = 0;
			ok = JsonUtf8CodeAddUnit(&Code, Unit1);
			ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit2);
		}
	}

	for (Unit1 = 0xF5; ok && (Unit1 != 0); Unit1++)
	{
		ok = ok && !JsonUtf8CodeAddUnit(&Code, Unit1);
	}

	return ok;
}


static bool TestJsonUtf8GetNextCode(void)
{
	tJsonUtf8Unit Content[4] = {};
	tJsonCharacter Character;
	tJsonUtf8Code Code;
	tJsonUtf8Code NextCode;
	bool ok = true;

	ok = ok && (JsonUtf8GetNextCode(Content, 0, 0, &NextCode) == 0);

	for (Character = 0; ok && (Character < 0x110000); Character++)
	{
		if ((Character < 0xD800) || (Character > 0xDFFF))
		{
			Code = JsonUtf8Code(Character);
			Content[0] = JsonUtf8CodeGetUnit(Code, 0);
			Content[1] = JsonUtf8CodeGetUnit(Code, 1);
			Content[2] = JsonUtf8CodeGetUnit(Code, 2);
			Content[3] = JsonUtf8CodeGetUnit(Code, 3);

			ok = (JsonUtf8GetNextCode(Content, sizeof(Content), 0, &NextCode) == JsonUtf8CodeGetUnitLength(Code));

			ok = ok && (Code == NextCode);
		}
	}

	for (Code = 0x80; ok && (Code < 0x100); Code++)
	{
		Content[0] = Code;

		ok = (JsonUtf8GetNextCode(Content, 1, 0, &NextCode) == 0);
	}

	for (Code = 0x8000; ok && (Code < 0x10000); Code++)
	{
		if ((Code & 0xE0C0) != 0xC080)
		{
			Content[0] = Code >> 8;
			Content[1] = Code;

			ok = (JsonUtf8GetNextCode(Content, 2, 0, &NextCode) == 0);
		}
	}

	for (Code = 0x800000; ok && (Code < 0x1000000); Code++)
	{
		if (((Code & 0xE0C000) != 0xC08000) && ((Code & 0xF0C0C0) != 0xE08080))
		{
			Content[0] = Code >> 16;
			Content[1] = Code >> 8;
			Content[2] = Code;

			ok = (JsonUtf8GetNextCode(Content, 3, 0, &NextCode) == 0);
		}
	}

	for (Code = 0x80000000; ok && (Code != 0); Code++)
	{
		if (((Code & 0xE0C00000) != 0xC0800000) && ((Code & 0xF0C0C000) != 0xE0808000) && ((Code & 0xF8C0C0C0) != 0xF0808080))
		{
			Content[0] = Code >> 24;
			Content[1] = Code >> 16;
			Content[2] = Code >> 8;
			Content[3] = Code;

			ok = (JsonUtf8GetNextCode(Content, 4, 0, &NextCode) == 0);
		}
	}

	for (Code = 0; ok && (Code < 0x80); Code++)
	{
		Content[0] = 0xC0 + (Code >> 6);
		Content[1] = 0x80 + (Code & 0x3F);

		ok = (JsonUtf8GetNextCode(Content, 2, 0, &NextCode) == 0);
	}

	for (Code = 0; ok && (Code < 0x800); Code++)
	{
		Content[0] = 0xE0 + (Code >> 12);
		Content[1] = 0x80 + ((Code >> 6) & 0x3F);
		Content[2] = 0x80 + (Code & 0x3F);

		ok = (JsonUtf8GetNextCode(Content, 3, 0, &NextCode) == 0);
	}

	for (Code = 0; ok && (Code < 0x10000); Code++)
	{
		Content[0] = 0xF0 + (Code >> 18);
		Content[1] = 0x80 + ((Code >> 12) & 0x3F);
		Content[2] = 0x80 + ((Code >> 6) & 0x3F);
		Content[3] = 0x80 + (Code & 0x3F);

		ok = (JsonUtf8GetNextCode(Content, 4, 0, &NextCode) == 0);
	}

	for (Code = 0xD800; ok && (Code < 0xE000); Code++)
	{
		Content[0] = 0xE0 + (Code >> 12);
		Content[1] = 0x80 + ((Code >> 6) & 0x3F);
		Content[2] = 0x80 + (Code & 0x3F);

		ok = (JsonUtf8GetNextCode(Content, 3, 0, &NextCode) == 0);
	}

	for (Code = 0x110000; ok && (Code < 0x200000); Code++)
	{
		Content[0] = 0xF0 + (Code >> 18);
		Content[1] = 0x80 + ((Code >> 12) & 0x3F);
		Content[2] = 0x80 + ((Code >> 6) & 0x3F);
		Content[3] = 0x80 + (Code & 0x3F);

		ok = (JsonUtf8GetNextCode(Content, 4, 0, &NextCode) == 0);
	}

	return ok;
}


static bool TestJsonUtf8GetPreviousCode(void)
{
	tJsonUtf8Unit Content[4] = {};
	tJsonCharacter Character;
	tJsonUtf8Code Code;
	tJsonUtf8Code PreviousCode;
	size_t Length;
	bool ok = true;

	ok = ok && (JsonUtf8GetPreviousCode(Content, 0, 0, &PreviousCode) == 0);

	for (Character = 0; ok && (Character < 0x110000); Character++)
	{
		if ((Character < 0xD800) || (Character > 0xDFFF))
		{
			Code = JsonUtf8Code(Character);
			Length = JsonUtf8CodeGetUnitLength(Code);
			Content[0] = JsonUtf8CodeGetUnit(Code, 0);
			Content[1] = JsonUtf8CodeGetUnit(Code, 1);
			Content[2] = JsonUtf8CodeGetUnit(Code, 2);
			Content[3] = JsonUtf8CodeGetUnit(Code, 3);

			ok = (JsonUtf8GetPreviousCode(Content, Length, Length, &PreviousCode) == Length);

			ok = ok && (Code == PreviousCode);
		}
	}

	for (Code = 0x80; ok && (Code < 0x100); Code++)
	{
		Content[0] = Code;

		ok = (JsonUtf8GetPreviousCode(Content, 1, 1, &PreviousCode) == 0);
	}

	for (Code = 0x100; ok && (Code < 0x10000); Code++)
	{
		if (((Code & 0x80) != 0) && ((Code & 0xE0C0) != 0xC080))
		{
			Content[0] = Code >> 8;
			Content[1] = Code;

			ok = (JsonUtf8GetPreviousCode(Content, 2, 2, &PreviousCode) == 0);
		}
	}

	for (Code = 0x10000; ok && (Code < 0x1000000); Code++)
	{
		if (((Code & 0x80) != 0) && ((Code & 0xE0C0) != 0xC080) && ((Code & 0xF0C0C0) != 0xE08080))
		{
			Content[0] = Code >> 16;
			Content[1] = Code >> 8;
			Content[2] = Code;

			ok = (JsonUtf8GetPreviousCode(Content, 3, 3, &PreviousCode) == 0);
		}
	}

	for (Code = 0x01000000; ok && (Code != 0); Code++)
	{
		if (((Code & 0x80) != 0) && ((Code & 0xE0C0) != 0xC080) && ((Code & 0xF0C0C0) != 0xE08080) && ((Code & 0xF8C0C0C0) != 0xF0808080))
		{
			Content[0] = Code >> 24;
			Content[1] = Code >> 16;
			Content[2] = Code >> 8;
			Content[3] = Code;

			ok = (JsonUtf8GetPreviousCode(Content, 4, 4, &PreviousCode) == 0);
		}
	}

	for (Code = 0; ok && (Code < 0x80); Code++)
	{
		Content[0] = 0xC0 + (Code >> 6);
		Content[1] = 0x80 + (Code & 0x3F);

		ok = (JsonUtf8GetPreviousCode(Content, 2, 2, &PreviousCode) == 0);
	}

	for (Code = 0; ok && (Code < 0x800); Code++)
	{
		Content[0] = 0xE0 + (Code >> 12);
		Content[1] = 0x80 + ((Code >> 6) & 0x3F);
		Content[2] = 0x80 + (Code & 0x3F);

		ok = (JsonUtf8GetPreviousCode(Content, 3, 3, &PreviousCode) == 0);
	}

	for (Code = 0xD800; ok && (Code < 0xE000); Code++)
	{
		Content[0] = 0xE0 + (Code >> 12);
		Content[1] = 0x80 + ((Code >> 6) & 0x3F);
		Content[2] = 0x80 + (Code & 0x3F);

		ok = (JsonUtf8GetPreviousCode(Content, 3, 3, &PreviousCode) == 0);
	}

	for (Code = 0; ok && (Code < 0x10000); Code++)
	{
		Content[0] = 0xF0 + (Code >> 18);
		Content[1] = 0x80 + ((Code >> 12) & 0x3F);
		Content[2] = 0x80 + ((Code >> 6) & 0x3F);
		Content[3] = 0x80 + (Code & 0x3F);

		ok = (JsonUtf8GetPreviousCode(Content, 4, 4, &PreviousCode) == 0);
	}

	for (Code = 0x110000; ok && (Code < 0x200000); Code++)
	{
		Content[0] = 0xF0 + (Code >> 18);
		Content[1] = 0x80 + ((Code >> 12) & 0x3F);
		Content[2] = 0x80 + ((Code >> 6) & 0x3F);
		Content[3] = 0x80 + (Code & 0x3F);

		ok = (JsonUtf8GetPreviousCode(Content, 4, 4, &PreviousCode) == 0);
	}

	return ok;
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
