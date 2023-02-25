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
