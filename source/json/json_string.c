#include <stdlib.h>
#include <string.h>
#include "json_string.h"


#define JSON_STRING_SENTINEL       0xFF
#define JSON_STRING_INITIAL_LENGTH 32


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


size_t JsonStringGetLength(tJsonString *String)
{
    return String->Length;
}


int JsonStringAddCharacter(tJsonString *String, uint8_t Character)
{
    uint8_t *NewContent;
    size_t NewLength;

    if (Character == '\0')
    {
        return 0;
    }

    if ((String->Content == NULL) || (String->Content[String->Length + 1] == JSON_STRING_SENTINEL))
    {
        NewLength = (String->Content == NULL) ? JSON_STRING_INITIAL_LENGTH : String->Length + String->Length / 2;
        if (NewLength < String->Length)
        {
            return 0;
        }

        NewContent = (uint8_t *)realloc(String->Content, NewLength + 2);
        if (NewContent == NULL)
        {
            return 0;
        }

        memset(&NewContent[String->Length + 1], 0, NewLength - String->Length);
        NewContent[NewLength + 1] = JSON_STRING_SENTINEL;

        String->Content = NewContent;
    }

    String->Content[String->Length] = Character;
    String->Length++;

    return 1;
}


uint8_t JsonStringGetCharacter(tJsonString *String, size_t Index)
{
    return (Index < String->Length) ? String->Content[Index] : '\0';
}
