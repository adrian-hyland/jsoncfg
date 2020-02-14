#ifndef JSON_PARSE_H
#define JSON_PARSE_H

#include "json_element.h"


typedef enum
{
    json_ParseComplete,
    json_ParseError,
    json_ParseKeyStart,
    json_ParseKey,
    json_ParseKeyEscape,
    json_ParseKeyEnd,
    json_ParseValueStart,
    json_ParseValueString,
    json_ParseValueStringEscape,
    json_ParseValueLiteral,
    json_ParseValueEnd,
} tJsonParseState;


typedef struct
{
    tJsonParseState State;
    tJsonElement   *Element;
} tJsonParse;


#define JSON_PARSE_ERROR      -1
#define JSON_PARSE_COMPLETE   0
#define JSON_PARSE_INCOMPLETE 1


void JsonParseSetUp(tJsonParse *Parse, tJsonElement *RootElement);


void JsonParseCleanUp(tJsonParse *Parse);


int JsonParse(tJsonParse *Parse, uint8_t Character);


#endif
