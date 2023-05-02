#include "json_character.h"
#include "test_json.h"


static tTestResult TestJsonCharacterIsLiteral(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonCharacter Character;
	bool IsLiteral;

	for (Character = 0; Character < 0x80; Character++)
	{
		IsLiteral = JsonCharacterIsLiteral(Character);
		if (((Character >= '0') && (Character <= '9')) ||
		    ((Character >= 'a') && (Character <= 'z')) ||
		    ((Character >= 'A') && (Character <= 'Z')) ||
		     (Character == '-') || (Character == '+') || (Character == '.'))
		{
			TEST_IS_TRUE(IsLiteral, TestResult);
		}
		else
		{
			TEST_IS_FALSE(IsLiteral, TestResult);
		}
	}

	return TestResult;
}


static tTestResult TestJsonCharacterIsWhitespace(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonCharacter Character;
	bool IsWhitespace;

	for (Character = 0; Character < 0x80; Character++)
	{
		IsWhitespace = JsonCharacterIsWhitespace(Character);
		if ((Character == ' ') || (Character == '\t') || (Character == '\r') || (Character == '\n'))
		{
			TEST_IS_TRUE(IsWhitespace, TestResult);
		}
		else
		{
			TEST_IS_FALSE(IsWhitespace, TestResult);
		}
	}

	return TestResult;
}


static tTestResult TestJsonCharacterIsControl(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonCharacter Character;
	bool IsControl;

	for (Character = 0; Character < 0x80; Character++)
	{
		IsControl = JsonCharacterIsControl(Character);
		if (Character < 0x20)
		{
			TEST_IS_TRUE(IsControl, TestResult);
		}
		else
		{
			TEST_IS_FALSE(IsControl, TestResult);
		}
	}

	return TestResult;
}


static tTestResult TestJsonCharacterIsHexDigit(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonCharacter Character;
	bool IsHexDigit;

	for (Character = 0; Character < 0x80; Character++)
	{
		IsHexDigit = JsonCharacterIsHexDigit(Character);
		if (((Character >= '0') && (Character <= '9')) || ((Character >= 'a') && (Character <= 'f')) || ((Character >= 'A') && (Character <= 'F')))
		{
			TEST_IS_TRUE(IsHexDigit, TestResult);
		}
		else
		{
			TEST_IS_FALSE(IsHexDigit, TestResult);
		}
	}

	return TestResult;
}


static tTestResult TestJsonCharacterToHexDigit(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonCharacter Character;
	uint8_t HexDigit;

	for (Character = 0; Character < 0x80; Character++)
	{
		HexDigit = JsonCharacterToHexDigit(Character);
		if ((Character >= '0') && (Character <= '9'))
		{
			TEST_IS_EQ(HexDigit, Character - '0', TestResult);
		}
		else if ((Character >= 'a') && (Character <= 'f'))
		{
			TEST_IS_EQ(HexDigit, Character - 'a' + 10, TestResult);
		}
		else if ((Character >= 'A') && (Character <= 'F'))
		{
			TEST_IS_EQ(HexDigit, Character - 'A' + 10, TestResult);
		}
		else
		{
			TEST_IS_ZERO(HexDigit, TestResult);
		}
	}

	return TestResult;
}


static tTestResult TestJsonCharacterFromHexDigit(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonCharacter Character;
	uint8_t HexDigit;

	for (HexDigit = 0; HexDigit < 0x0A; HexDigit++)
	{
		Character = JsonCharacterFromHexDigit(HexDigit);

		TEST_IS_EQ(Character, HexDigit + '0', TestResult);
	}

	for (HexDigit = 0x0A; HexDigit < 0x10; HexDigit++)
	{
		Character = JsonCharacterFromHexDigit(HexDigit);

		TEST_IS_EQ(Character, HexDigit - 0x0A + 'A', TestResult);
	}

	for (HexDigit = 0x10; HexDigit != 0; HexDigit++)
	{
		TEST_IS_ZERO(JsonCharacterFromHexDigit(HexDigit), TestResult);
	}

	return TestResult;
}


static tTestResult TestJsonCharacterIsEscapable(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonCharacter Character;
	bool IsEscapable;

	for (Character = 0; Character < 0x80; Character++)
	{
		IsEscapable = JsonCharacterIsEscapable(Character);
		if ((Character == '\b') || (Character == '\f') || (Character == '\t') || (Character == '\r') || (Character == '\n') || (Character == '"') || (Character == '\\'))
		{
			TEST_IS_TRUE(IsEscapable, TestResult);
		}
		else
		{
			TEST_IS_FALSE(IsEscapable, TestResult);
		}
	}

	return TestResult;
}


static tTestResult TestJsonCharacterToEscape(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonCharacter Character;
	tJsonCharacter EscapedCharacter;

	for (Character = 0; Character < 0x80; Character++)
	{
		EscapedCharacter = JsonCharacterToEscape(Character);
		if (Character == '\b')
		{
			TEST_IS_EQ(EscapedCharacter, 'b', TestResult);
		}
		else if (Character == '\f')
		{
			TEST_IS_EQ(EscapedCharacter, 'f', TestResult);
		}
		else if (Character == '\t')
		{
			TEST_IS_EQ(EscapedCharacter, 't', TestResult);
		}
		else if (Character == '\r')
		{
			TEST_IS_EQ(EscapedCharacter, 'r', TestResult);
		}
		else if (Character == '\n')
		{
			TEST_IS_EQ(EscapedCharacter, 'n', TestResult);
		}
		else
		{
			TEST_IS_EQ(EscapedCharacter, Character, TestResult);
		}
	}

	return TestResult;
}


static tTestResult TestJsonCharacterFromEscape(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonCharacter Character;
	tJsonCharacter UnescapedCharacter;

	for (Character = 0; Character < 0x80; Character++)
	{
		UnescapedCharacter = JsonCharacterFromEscape(Character);
		if (Character == 'b')
		{
			TEST_IS_EQ(UnescapedCharacter, '\b', TestResult);
		}
		else if (Character == 'f')
		{
			TEST_IS_EQ(UnescapedCharacter, '\f', TestResult);
		}
		else if (Character == 't')
		{
			TEST_IS_EQ(UnescapedCharacter, '\t', TestResult);
		}
		else if (Character == 'r')
		{
			TEST_IS_EQ(UnescapedCharacter, '\r', TestResult);
		}
		else if (Character == 'n')
		{
			TEST_IS_EQ(UnescapedCharacter, '\n', TestResult);
		}
		else
		{
			TEST_IS_EQ(UnescapedCharacter, Character, TestResult);
		}
	}

	return TestResult;
}


static const tTestCase TestCaseJsonCharacter[] =
{
	{ "JsonCharacterIsLiteral",    TestJsonCharacterIsLiteral    },
	{ "JsonCharacterIsWhitespace", TestJsonCharacterIsWhitespace },
	{ "JsonCharacterIsControl",    TestJsonCharacterIsControl    },
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
