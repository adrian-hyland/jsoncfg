#include "json_character.h"


bool JsonCharacterIsLiteral(uint8_t Character)
{
	return ((Character >= '0') && (Character <= '9')) ||
	       ((Character >= 'a') && (Character <= 'z')) ||
	       ((Character >= 'A') && (Character <= 'Z')) ||
	        (Character == '-') ||
	        (Character == '+') ||
	        (Character == '.');
}


bool JsonCharacterIsWhitespace(uint8_t Character)
{
	return (Character == ' ') || (Character == '\t') || (Character == '\r') || (Character == '\n');
}


bool JsonCharacterIsEscapable(uint8_t Character)
{
	return (Character == '\b') || (Character == '\f') || (Character == '\n') || (Character == '\r') || (Character == '\t') || (Character == '"') || (Character == '\\');
}


uint8_t JsonCharacterToEscape(uint8_t Character)
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


uint8_t JsonCharacterFromEscape(uint8_t Character)
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
