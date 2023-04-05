#include <stdlib.h>
#include <string.h>
#include "json_string.h"


#define JSON_STRING_SENTINEL       0xFF
#define JSON_STRING_INITIAL_LENGTH 32


static bool JsonStringAddByte(tJsonString *String, uint8_t Byte)
{
	uint8_t *NewContent;
	size_t NewLength;

	if (Byte == '\0')
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

		NewContent = (uint8_t *)realloc(String->Content, NewLength);
		if (NewContent == NULL)
		{
			return false;
		}

		memset(&NewContent[String->Length + 1], 0, NewLength - String->Length - 2);
		NewContent[NewLength - 1] = JSON_STRING_SENTINEL;

		String->Content = NewContent;
	}

	String->Content[String->Length] = Byte;
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


bool JsonStringAddCharacter(tJsonString *String, tJsonCharacter Character)
{
	tJsonUtf8 Utf8;
	size_t Length;
	size_t n;

	Length = JsonUtf8Encode(Utf8, sizeof(Utf8), 0, Character);
	if (Length == 0)
	{
		return false;
	}

	for (n = 0; (n < Length) && JsonStringAddByte(String, Utf8[n]); n++)
		;

	return n == Length;
}


size_t JsonStringGetNextCharacter(const tJsonString *String, size_t Offset, tJsonCharacter *Character)
{
	return JsonUtf8DecodeNext(String->Content, String->Length, Offset, Character);
}
