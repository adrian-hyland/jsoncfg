#include "json_character.h"
#include "test_json.h"


static bool TestJsonCharacterIsLiteral(void)
{
	tJsonCharacter Character;
	bool IsLiteral;
	bool ok;

	for (ok = true, Character = 0; ok && (Character < 0x80); Character++)
	{
		IsLiteral = JsonCharacterIsLiteral(Character);
		if (((Character >= '0') && (Character <= '9')) ||
		    ((Character >= 'a') && (Character <= 'z')) ||
		    ((Character >= 'A') && (Character <= 'Z')) ||
		     (Character == '-') || (Character == '+') || (Character == '.'))
		{
			ok = IsLiteral;
		}
		else
		{
			ok = !IsLiteral;
		}
	}

	return ok;
}


static bool TestJsonCharacterIsWhitespace(void)
{
	tJsonCharacter Character;
	bool IsWhitespace;
	bool ok;

	for (ok = true, Character = 0; ok && (Character < 0x80); Character++)
	{
		IsWhitespace = JsonCharacterIsWhitespace(Character);
		if ((Character == ' ') || (Character == '\t') || (Character == '\r') || (Character == '\n'))
		{
			ok = IsWhitespace;
		}
		else
		{
			ok = !IsWhitespace;
		}
	}

	return ok;
}


static bool TestJsonCharacterIsHexDigit(void)
{
	tJsonCharacter Character;
	bool IsHexDigit;
	bool ok;

	for (ok = true, Character = 0; ok && (Character < 0x80); Character++)
	{
		IsHexDigit = JsonCharacterIsHexDigit(Character);
		if (((Character >= '0') && (Character <= '9')) || ((Character >= 'a') && (Character <= 'f')) || ((Character >= 'A') && (Character <= 'F')))
		{
			ok = IsHexDigit;
		}
		else
		{
			ok = !IsHexDigit;
		}
	}

	return ok;
}


static bool TestJsonCharacterToHexDigit(void)
{
	tJsonCharacter Character;
	uint8_t HexDigit;
	bool ok;

	for (ok = true, Character = 0; ok && (Character < 0x80); Character++)
	{
		HexDigit = JsonCharacterToHexDigit(Character);
		if ((Character >= '0') && (Character <= '9'))
		{
			ok = (HexDigit == Character - '0');
		}
		else if ((Character >= 'a') && (Character <= 'f'))
		{
			ok = (HexDigit == Character - 'a' + 10);
		}
		else if ((Character >= 'A') && (Character <= 'F'))
		{
			ok = (HexDigit == Character - 'A' + 10);
		}
		else
		{
			ok = (HexDigit == 0);
		}
	}

	return ok;
}


static bool TestJsonCharacterFromHexDigit(void)
{
	tJsonCharacter Character;
	uint8_t HexDigit;
	bool ok;

	for (ok = true, HexDigit = 0; ok && (HexDigit < 0x0A); HexDigit++)
	{
		Character = JsonCharacterFromHexDigit(HexDigit);

		ok = (Character == HexDigit + '0');
	}

	for (HexDigit = 0x0A; ok && (HexDigit < 0x10); HexDigit++)
	{
		Character = JsonCharacterFromHexDigit(HexDigit);

		ok = (Character == HexDigit - 0x0A + 'A');
	}

	for (HexDigit = 0x10; ok && (HexDigit != 0); HexDigit++)
	{
		ok = (JsonCharacterFromHexDigit(HexDigit) == 0);
	}

	return ok;
}


static bool TestJsonCharacterIsEscapable(void)
{
	tJsonCharacter Character;
	bool IsEscapable;
	bool ok;

	for (ok = true, Character = 0; ok && (Character < 0x80); Character++)
	{
		IsEscapable = JsonCharacterIsEscapable(Character);
		if ((Character == '\b') || (Character == '\f') || (Character == '\t') || (Character == '\r') || (Character == '\n') || (Character == '"') || (Character == '\\'))
		{
			ok = IsEscapable;
		}
		else
		{
			ok = !IsEscapable;
		}
	}

	return ok;
}


static bool TestJsonCharacterToEscape(void)
{
	tJsonCharacter Character;
	tJsonCharacter EscapedCharacter;
	bool ok;

	for (ok = true, Character = 0; ok && (Character < 0x80); Character++)
	{
		EscapedCharacter = JsonCharacterToEscape(Character);
		if (Character == '\b')
		{
			ok = (EscapedCharacter == 'b');
		}
		else if (Character == '\f')
		{
			ok = (EscapedCharacter == 'f');
		}
		else if (Character == '\t')
		{
			ok = (EscapedCharacter == 't');
		}
		else if (Character == '\r')
		{
			ok = (EscapedCharacter == 'r');
		}
		else if (Character == '\n')
		{
			ok = (EscapedCharacter = 'n');
		}
		else
		{
			ok = (EscapedCharacter == Character);
		}
	}

	return ok;
}


static bool TestJsonCharacterFromEscape(void)
{
	tJsonCharacter Character;
	tJsonCharacter UnescapedCharacter;
	bool ok;

	for (ok = true, Character = 0; ok && (Character < 0x80); Character++)
	{
		UnescapedCharacter = JsonCharacterFromEscape(Character);
		if (Character == 'b')
		{
			ok = (UnescapedCharacter == '\b');
		}
		else if (Character == 'f')
		{
			ok = (UnescapedCharacter == '\f');
		}
		else if (Character == 't')
		{
			ok = (UnescapedCharacter == '\t');
		}
		else if (Character == 'r')
		{
			ok = (UnescapedCharacter == '\r');
		}
		else if (Character == 'n')
		{
			ok = (UnescapedCharacter = '\n');
		}
		else
		{
			ok = (UnescapedCharacter == Character);
		}
	}

	return ok;
}


static const tTestCase TestCaseJsonCharacter[] =
{
	{ "JsonCharacterIsLiteral",    TestJsonCharacterIsLiteral    },
	{ "JsonCharacterIsWhitespace", TestJsonCharacterIsWhitespace },
	{ "JsonCharacterIsHexDigit",   TestJsonCharacterIsHexDigit   },
	{ "JsonCharacterToHexDigit",   TestJsonCharacterToHexDigit   },
	{ "JsonCharacterFromHexDigit", TestJsonCharacterFromHexDigit },
	{ "JsonCharacterIsEscapable",  TestJsonCharacterIsEscapable  },
	{ "JsonCharacterToEscape",     TestJsonCharacterToEscape     },
	{ "JsonCharacterFromEscape",   TestJsonCharacterFromEscape   }
};


const tTest TestJsonCharacter =
{
	"JsonCharacter",
	TestCaseJsonCharacter,
	sizeof(TestCaseJsonCharacter) / sizeof(TestCaseJsonCharacter[0])
};
