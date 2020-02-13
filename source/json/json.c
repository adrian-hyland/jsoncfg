#include "json.h"


int JsonReadString(tJsonElement *Root, const char *String)
{
    tJsonParse Parse;
    int Error;

    JsonParseSetUp(&Parse, Root);

    do
    {
        Error = JsonParse(&Parse, *String);
        String++;
    }
    while (Error > 0);

    JsonParseCleanUp(&Parse);

    return Error == 0;
}


int JsonReadFile(tJsonElement *Root, FILE *Stream)
{
    tJsonParse Parse;
    int Character;
    int Error;

    JsonParseSetUp(&Parse, Root);

    do
    {
        Character = fgetc(Stream);
        if (Character == EOF)
        {
            Character = '\0';
        }

        Error = JsonParse(&Parse, Character);
    }
    while (Error > 0);

    JsonParseCleanUp(&Parse);

    return Error == 0;
}


int JsonWriteFile(tJsonElement *Root, FILE *Stream)
{
    tJsonFormat Format;
    uint8_t Character;
    int Error;

    JsonFormatSetUpIndent(&Format, 3, Root);

    do
    {
        Error = JsonFormat(&Format, &Character);
        if (Error > 0)
        {
            if (fputc(Character, Stream) == EOF)
            {
                Error = -1;
            }
        }
    }
    while (Error > 0);

    JsonFormatCleanUp(&Format);

    return Error == 0;
}
