#ifndef JSON_STRING_H
#define JSON_STRING_H

#include <stddef.h>
#include <stdint.h>


typedef struct
{
    uint8_t *Content;
    size_t   Length;
    size_t   MaxLength;
} tJsonString;


void JsonStringSetUp(tJsonString *String);


void JsonStringCleanUp(tJsonString *String);


void JsonStringClear(tJsonString *String);


size_t JsonStringGetLength(tJsonString *String);


int JsonStringAddCharacter(tJsonString *String, uint8_t Character);


uint8_t JsonStringGetCharacter(tJsonString *String, size_t Index);


#endif
