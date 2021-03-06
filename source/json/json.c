#include "json.h"


int JsonReadString(tJsonElement *Root, int StripComments, const char *String)
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


int JsonReadFile(tJsonElement *Root, int StripComments, FILE *Stream)
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


int JsonWriteFile(tJsonElement *Root, size_t IndentSize, tJsonCommentType CommentType, FILE *Stream)
{
    tJsonFormat Format;
    uint8_t Character;
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
        Error = JsonFormat(&Format, &Character);
        if (Error == JSON_FORMAT_INCOMPLETE)
        {
            if (fputc(Character, Stream) == EOF)
            {
                Error = JSON_FORMAT_ERROR;
            }
        }
    }
    while (Error == JSON_FORMAT_INCOMPLETE);

    JsonFormatCleanUp(&Format);

    return Error == JSON_FORMAT_COMPLETE;
}
