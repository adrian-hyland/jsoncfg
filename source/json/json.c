#include <fcntl.h>
#include "json_utf.h"
#include "json.h"


#define JSON_BUFFER_SIZE(n) (((n) > JSON_FORMAT_MIN_SIZE) ? (n) : JSON_FORMAT_MIN_SIZE)


bool JsonReadStringAscii(tJsonElement *Root, bool StripComments, const char *String)
{
	return JsonReadStringUtf8(Root, StripComments, (const uint8_t *)String);
}


bool JsonReadStringUtf8(tJsonElement *Root, bool StripComments, const uint8_t *String)
{
	tJsonParse Parse;
	int State;

	JsonParseSetUp(&Parse, StripComments, Root);

	State = JsonParse(&Parse, json_Utf8, String, strlen((const char *)String) + 1, NULL);

	JsonParseCleanUp(&Parse);

	return State == JSON_PARSE_COMPLETE;
}


bool JsonReadFile(tJsonElement *Root, bool StripComments, FILE *Stream, size_t BufferSize)
{
	tJsonUtfType UtfType;
	tJsonParse Parse;
	uint8_t Buffer[JSON_BUFFER_SIZE(BufferSize)];
	size_t Length;
	size_t Offset;
	int State = JSON_PARSE_INCOMPLETE;

#ifdef __WIN32__
	setmode(fileno(Stream), O_BINARY);
#endif

	Length = fread(Buffer, 1, JSON_BUFFER_SIZE(BufferSize), Stream);

	Offset = JsonUtfGetType(Buffer, Length, &UtfType);

	JsonParseSetUp(&Parse, StripComments, Root);

	while ((State == JSON_PARSE_INCOMPLETE) && (Length > 0))
	{
		State = JsonParse(&Parse, UtfType, Buffer, Length, &Offset);
		if (State != JSON_PARSE_ERROR)
		{
			if (Offset != 0)
			{
				memmove(Buffer, &Buffer[Offset], Length - Offset);
				Offset = Length - Offset;
				Length = Offset + fread(&Buffer[Offset], 1, JSON_BUFFER_SIZE(BufferSize) - Offset, Stream);
				Offset = 0;
			}
			else
			{
				State = JSON_PARSE_ERROR;
			}
		}
	}

	if (State == JSON_PARSE_INCOMPLETE)
	{
		Length = JsonUtfEncode(UtfType, Buffer, JSON_BUFFER_SIZE(BufferSize), 0, '\0');
		State = JsonParse(&Parse, UtfType, Buffer, Length, NULL);
	}

	JsonParseCleanUp(&Parse);

	return State == JSON_PARSE_COMPLETE;
}


bool JsonWriteFile(tJsonElement *Root, tJsonUtfType UtfType, bool RequireBOM, size_t IndentSize, tJsonCommentType CommentType, FILE *Stream, size_t BufferSize)
{
	tJsonFormat Format;
	uint8_t Buffer[JSON_BUFFER_SIZE(BufferSize)];
	size_t Offset;
	int State = JSON_FORMAT_INCOMPLETE;

	if (IndentSize == 0)
	{
		JsonFormatSetUpSpace(&Format, Root);
	}
	else
	{
		JsonFormatSetUpIndent(&Format, IndentSize, CommentType, Root);
	}

#ifdef __WIN32__
	setmode(fileno(Stream), O_BINARY);
#endif

	Offset = RequireBOM ? JsonUtfEncode(UtfType, Buffer, JSON_BUFFER_SIZE(BufferSize), 0, JSON_CHARACTER_BOM) : 0;

	do
	{
		State = JsonFormat(&Format, UtfType, Buffer, JSON_BUFFER_SIZE(BufferSize), &Offset);
		if (State != JSON_FORMAT_ERROR)
		{
			if ((Offset > 0) && (fwrite(Buffer, 1, Offset, Stream) != Offset))
			{
				State = JSON_FORMAT_ERROR;
			}
			Offset = 0;
		}
	}
	while (State == JSON_FORMAT_INCOMPLETE);

	JsonFormatCleanUp(&Format);

	return State == JSON_FORMAT_COMPLETE;
}
