#include <stdlib.h>
#include "json_string.h"


#define JSON_STRING_INITIAL_MAX_LENGTH 32


void JsonStringSetUp(tJsonString *String)
{
    String->Content = NULL;
    String->Length = 0;
    String->MaxLength = 0;
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
    String->MaxLength = 0;
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

    if (String->Length >= String->MaxLength)
    {
        NewLength = (String->MaxLength == 0) ? JSON_STRING_INITIAL_MAX_LENGTH : String->MaxLength + String->MaxLength / 2;
        if (NewLength < String->MaxLength)
        {
            return 0;
        }

        NewContent = realloc(String->Content, NewLength + 1);
        if (NewContent == NULL)
        {
            return 0;
        }

        String->Content = NewContent;
        String->MaxLength = NewLength;
    }

    String->Content[String->Length] = Character;
    String->Length++;
    String->Content[String->Length] = '\0';

    return 1;
}


uint8_t JsonStringGetCharacter(tJsonString *String, size_t Index)
{
    return (Index < String->Length) ? String->Content[Index] : '\0';
}
