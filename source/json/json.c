#include "json.h"


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


bool JsonReadFile(tJsonElement *Root, bool StripComments, FILE *Stream)
{
	tJsonParse Parse;
	int Character;
	int Error;

	JsonParseSetUp(&Parse, StripComments, Root);

	do
	{
		Character = fgetc(Stream);
		if (Character == EOF)
		{
			Character = '\0';
		}

		Error = JsonParse(&Parse, Character);
	}
	while (Error == JSON_PARSE_INCOMPLETE);

	JsonParseCleanUp(&Parse);

	return Error == JSON_PARSE_COMPLETE;
}


bool JsonWriteFile(tJsonElement *Root, size_t IndentSize, tJsonCommentType CommentType, FILE *Stream)
{
	tJsonFormat Format;
	tJsonUtf8Unit CodeUnit;
	int Error;

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
		Error = JsonFormat(&Format, &CodeUnit);
		if (Error == JSON_FORMAT_INCOMPLETE)
		{
			if (fputc(CodeUnit, Stream) == EOF)
			{
				Error = JSON_FORMAT_ERROR;
			}
		}
	}
	while (Error == JSON_FORMAT_INCOMPLETE);

	JsonFormatCleanUp(&Format);

	return Error == JSON_FORMAT_COMPLETE;
}
