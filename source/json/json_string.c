#include <stdlib.h>
#include <string.h>
#include "json_string.h"


#define JSON_STRING_SENTINEL       0xFF
#define JSON_STRING_INITIAL_LENGTH 32


static bool JsonStringAddUtf8CodeUnit(tJsonString *String, tJsonUtf8Unit CodeUnit)
{
	tJsonUtf8Unit *NewContent;
	size_t NewLength;

	if (CodeUnit == '\0')
	{
		return false;
	}

	if ((String->Content == NULL) || (String->Content[String->Length + 1] == JSON_STRING_SENTINEL))
	{
		NewLength = 2 + ((String->Content == NULL) ? JSON_STRING_INITIAL_LENGTH : String->Length + String->Length / 2);
		if (NewLength < String->Length)
		{
			return false;
		}

		NewContent = (tJsonUtf8Unit *)realloc(String->Content, NewLength);
		if (NewContent == NULL)
		{
			return false;
		}

		memset(&NewContent[String->Length + 1], 0, NewLength - String->Length - 2);
		NewContent[NewLength - 1] = JSON_STRING_SENTINEL;

		String->Content = NewContent;
	}

	String->Content[String->Length] = CodeUnit;
	String->Length++;

	return true;
}


void JsonStringSetUp(tJsonString *String)
{
	String->Content = NULL;
	String->Length = 0;
}


void JsonStringCleanUp(tJsonString *String)
{
	JsonStringClear(String);
}


void JsonStringClear(tJsonString *String)
{
	if (String->Content != NULL)
	{
		free(String->Content);
		String->Content = NULL;
	}
	String->Length = 0;
}


size_t JsonStringGetLength(const tJsonString *String)
{
	return String->Length;
}


bool JsonStringAddUtf8Code(tJsonString *String, tJsonUtf8Code Code)
{
	size_t Length = JsonUtf8CodeGetUnitLength(Code);
	size_t n;
	bool ok;

	ok = (Length != 0);

	for (n = 0; ok && (n < Length); n++)
	{
		ok = JsonStringAddUtf8CodeUnit(String, JsonUtf8CodeGetUnit(Code, n));
	}

	return ok;
}


bool JsonStringAddCharacter(tJsonString *String, tJsonCharacter Character)
{
	return JsonStringAddUtf8Code(String, JsonUtf8Code(Character));
}


size_t JsonStringGetNextUtf8Code(const tJsonString *String, size_t Offset, tJsonUtf8Code *Code)
{
	return JsonUtf8GetNextCode(String->Content, String->Length, Offset, Code);
}


size_t JsonStringGetNextCharacter(const tJsonString *String, size_t Offset, tJsonCharacter *Character)
{
	tJsonUtf8Code Code;
	size_t Length;

	Length = JsonStringGetNextUtf8Code(String, Offset, &Code);

	*Character = (Length != 0) ? JsonUtf8CodeGetCharacter(Code) : 0;

	return Length;
}
