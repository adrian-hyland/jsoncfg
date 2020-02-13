#ifndef JSON_FORMAT_H
#define JSON_FORMAT_H

#include "json_element.h"


typedef enum
{
    json_FormatComplete,
    json_FormatError,
    json_FormatKey,
    json_FormatKeyEscape,
    json_FormatKeyEnd,
    json_FormatValueStart,
    json_FormatValueString,
    json_FormatValueStringEscape,
    json_FormatValueLiteral,
    json_FormatValueNext,
    json_FormatValueEnd
} tJsonFormatState;


typedef enum
{
    json_FormatCompress,
    json_FormatSpace,
    json_FormatIndent
} tJsonFormatType;


typedef struct
{
    tJsonFormatType  Type;
    tJsonFormatState State;
    tJsonElement    *Element;
    size_t           NameIndex;
    size_t           Indent;
    size_t           IndentSize;
    size_t           SpaceCount;
    int              NewLine;
} tJsonFormat;


void JsonFormatSetUpCompress(tJsonFormat *Format, tJsonElement *RootElement);


void JsonFormatSetUpSpace(tJsonFormat *Format, tJsonElement *RootElement);


void JsonFormatSetUpIndent(tJsonFormat *Format, size_t IndentSize, tJsonElement *RootElement);


void JsonFormatCleanUp(tJsonFormat *Format);


int JsonFormat(tJsonFormat *Format, uint8_t *Character);


#endif
