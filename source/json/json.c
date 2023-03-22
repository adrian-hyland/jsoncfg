#include "json.h"


#define JSON_BUFFER_SIZE(n) (((n) != 0) ? (n) : 1)


bool JsonReadStringAscii(tJsonElement *Root, bool StripComments, const char *String)
{
	return JsonReadStringUtf8(Root, StripComments, (const tJsonUtf8Unit *)String);
}


bool JsonReadStringUtf8(tJsonElement *Root, bool StripComments, const tJsonUtf8Unit *String)
{
	tJsonParse Parse;
	int Error;

	JsonParseSetUp(&Parse, StripComments, Root);

	do
	{
		Error = JsonParse(&Parse, *String);
		String++;
	}
	while (Error == JSON_PARSE_INCOMPLETE);

	JsonParseCleanUp(&Parse);

	return Error == JSON_PARSE_COMPLETE;
}


bool JsonReadFile(tJsonElement *Root, bool StripComments, FILE *Stream, size_t BufferSize)
{
	tJsonParse Parse;
	tJsonUtf8Unit Buffer[JSON_BUFFER_SIZE(BufferSize)];
	size_t Length;
	size_t n;
	int Error = JSON_PARSE_INCOMPLETE;

	JsonParseSetUp(&Parse, StripComments, Root);

	do
	{
		Length = fread(Buffer, 1, JSON_BUFFER_SIZE(BufferSize), Stream);
		for (n = 0; (Error == JSON_PARSE_INCOMPLETE) && (n < Length); n++)
		{
			Error = JsonParse(&Parse, Buffer[n]);
		}
		if ((Error == JSON_PARSE_INCOMPLETE) && (Length < JSON_BUFFER_SIZE(BufferSize)))
		{
			Error = JsonParse(&Parse, '\0');
		}
	}
	while (Error == JSON_PARSE_INCOMPLETE);

	JsonParseCleanUp(&Parse);

	return Error == JSON_PARSE_COMPLETE;
}


bool JsonWriteFile(tJsonElement *Root, size_t IndentSize, tJsonCommentType CommentType, FILE *Stream, size_t BufferSize)
{
	tJsonFormat Format;
	tJsonUtf8Unit Buffer[JSON_BUFFER_SIZE(BufferSize)];
	size_t Length;
	int Error = JSON_FORMAT_INCOMPLETE;

	if (IndentSize == 0)
	{
		JsonFormatSetUpSpace(&Format, Root);
	}
	else
	{
		JsonFormatSetUpIndent(&Format, IndentSize, CommentType, Root);
	}

	do
	{
		for (Length = 0; (Error == JSON_FORMAT_INCOMPLETE) && (Length < JSON_BUFFER_SIZE(BufferSize)); Length++)
		{
			Error = JsonFormat(&Format, &Buffer[Length]);
			if (Error == JSON_FORMAT_COMPLETE)
			{
				break;
			}
		}

		if ((Error != JSON_FORMAT_ERROR) && (Length > 0) && (fwrite(Buffer, 1, Length, Stream) != Length))
		{
			Error = JSON_FORMAT_ERROR;
		}
	}
	while (Error == JSON_FORMAT_INCOMPLETE);

	JsonFormatCleanUp(&Format);

	return Error == JSON_FORMAT_COMPLETE;
}
