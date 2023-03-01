#include "json_character.h"


bool JsonCharacterIsLiteral(tJsonCharacter Character)
{
	return ((Character >= '0') && (Character <= '9')) ||
	       ((Character >= 'a') && (Character <= 'z')) ||
	       ((Character >= 'A') && (Character <= 'Z')) ||
	        (Character == '-') ||
	        (Character == '+') ||
	        (Character == '.');
}


bool JsonCharacterIsWhitespace(tJsonCharacter Character)
{
	return (Character == ' ') || (Character == '\t') || (Character == '\r') || (Character == '\n');
}


bool JsonCharacterIsHexDigit(tJsonCharacter Character)
{
	return ((Character >= '0') && (Character <= '9')) || ((Character >= 'a') && (Character <= 'f')) || ((Character >= 'A') && (Character <= 'F'));
}


uint8_t JsonCharacterToHexDigit(tJsonCharacter Character)
{
	if ((Character >= '0') && (Character <= '9'))
	{
		return Character - '0';
	}
	else if ((Character >= 'a') && (Character <= 'f'))
	{
		return Character - 'a' + 10;
	}
	else if ((Character >= 'A') && (Character <= 'F'))
	{
		return Character - 'A' + 10;
	}
	else
	{
		return 0;
	}
}


tJsonCharacter JsonCharacterFromHexDigit(uint8_t HexDigit)
{
	if ((HexDigit >= 0) && (HexDigit <= 9))
	{
		return '0' + HexDigit;
	}
	else if ((HexDigit >= 0x0A) && (HexDigit <= 0x0F))
	{
		return 'A' + HexDigit - 0x0A;
	}
	else
	{
		return 0;
	}
}


bool JsonCharacterIsEscapable(tJsonCharacter Character)
{
	return (Character == '\b') || (Character == '\f') || (Character == '\n') || (Character == '\r') || (Character == '\t') || (Character == '"') || (Character == '\\');
}


tJsonCharacter JsonCharacterToEscape(tJsonCharacter Character)
{
	switch (Character)
	{
		case '\b':
			Character = 'b';
		break;

		case '\f':
			Character = 'f';
		break;

		case '\n':
			Character = 'n';
		break;

		case '\r':
			Character = 'r';
		break;

		case '\t':
			Character = 't';
		break;
	}

	return Character;
}


tJsonCharacter JsonCharacterFromEscape(tJsonCharacter Character)
{
	switch (Character)
	{
		case 'b':
			Character = '\b';
		break;

		case 'f':
			Character = '\f';
		break;

		case 'n':
			Character = '\n';
		break;

		case 'r':
			Character = '\r';
		break;

		case 't':
			Character = '\t';
		break;
	}

	return Character;
}
