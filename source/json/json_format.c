#include "json_character.h"
#include "json_format.h"


#define JSON_FORMAT_INDENT_SPACE_COUNT 3


static tJsonFormatState JsonFormatValueEnd(tJsonFormat *Format, uint8_t *Character);


static tJsonFormatState JsonFormatKeyEscape(tJsonFormat *Format, uint8_t *Character)
{
    *Character = JsonStringGetCharacter(&Format->Element->Name, Format->NameIndex);
    *Character = JsonCharacterToEscape(*Character);
    Format->NameIndex++;
    return json_FormatKey;
}


static tJsonFormatState JsonFormatKey(tJsonFormat *Format, uint8_t *Character)
{
    if (Format->NameIndex < Format->Element->Name.Length)
    {
        *Character = JsonStringGetCharacter(&Format->Element->Name, Format->NameIndex);
        if (JsonCharacterIsEscapable(*Character))
        {
            *Character = '\\';
            return json_FormatKeyEscape;
        }
        Format->NameIndex++;
        return json_FormatKey;
    }
    else
    {
        *Character = '"';
        return json_FormatKeyEnd;
    }
}


static tJsonFormatState JsonFormatKeyEnd(tJsonFormat *Format, uint8_t *Character)
{
    *Character = ':';
    Format->Element = Format->Element->Child;
    if (Format->Type != json_FormatCompress)
    {
        Format->SpaceCount = 1;
    }
    return json_FormatValueStart;
}


static tJsonFormatState JsonFormatValueNext(tJsonFormat *Format, uint8_t *Character)
{
    tJsonType PreviousType;
    
    if (Format->Element->Next != NULL)
    {
        *Character = ',';
        Format->Element = Format->Element->Next;
        if (Format->Type == json_FormatIndent)
        {
            Format->NewLine = 1;
            if (!Format->NewLine)
            {
                Format->SpaceCount = 1;
            }
        }
        else if (Format->Type == json_FormatSpace)
        {
            Format->SpaceCount = 1;
        }
        return json_FormatValueStart;
    }
    else
    {
        PreviousType = Format->Element->Type;
        Format->Element = Format->Element->Parent;
        if (Format->Type == json_FormatIndent)
        {
            if ((Format->Element->Type == json_TypeObject) || (Format->Element->Type == json_TypeArray))
            {
                Format->Indent--;
                Format->NewLine = (Format->Element->Type == json_TypeObject) || ((Format->Element->Type == json_TypeArray) && ((PreviousType == json_TypeObject) || (PreviousType == json_TypeArray) || (Format->Element->Child->Next != NULL)));
                if (!Format->NewLine)
                {
                    Format->SpaceCount = 1;
                }
            }
        }
        else if (Format->Type == json_FormatSpace)
        {
            if ((Format->Element->Type == json_TypeObject) || (Format->Element->Type == json_TypeArray))
            {
                Format->SpaceCount = 1;
            }
        }
    }

    return JsonFormatValueEnd(Format, Character);
}


static tJsonFormatState JsonFormatValueEnd(tJsonFormat *Format, uint8_t *Character)
{
    if (Format->NewLine)
    {
        *Character = '\n';
        Format->NewLine = 0;
        Format->SpaceCount = Format->Indent * Format->IndentSize;
        return json_FormatValueEnd;
    }
    else if (Format->SpaceCount != 0)
    {
        *Character = ' ';
        Format->SpaceCount--;
        return json_FormatValueEnd;
    }
    else if (Format->Element->Type == json_TypeObject)
    {
        *Character = '}';
        return json_FormatValueNext;
    }
    else if (Format->Element->Type == json_TypeArray)
    {
        *Character = ']';
        return json_FormatValueNext;        
    }
    else if (Format->Element->Type == json_TypeKey)
    {
        return JsonFormatValueNext(Format, Character);
    }
    else if (Format->Element->Type == json_TypeRoot)
    {
        *Character = '\0';
        return json_FormatComplete;
    }

    return json_FormatError;
}


static tJsonFormatState JsonFormatValueStringEscape(tJsonFormat *Format, uint8_t *Character)
{
    *Character = JsonStringGetCharacter(&Format->Element->Name, Format->NameIndex);
    *Character = JsonCharacterToEscape(*Character);
    Format->NameIndex++;
    return json_FormatValueString;
}


static tJsonFormatState JsonFormatValueString(tJsonFormat *Format, uint8_t *Character)
{
    if (Format->NameIndex < Format->Element->Name.Length)
    {
        *Character = JsonStringGetCharacter(&Format->Element->Name, Format->NameIndex);
        if (JsonCharacterIsEscapable(*Character))
        {
            *Character = '\\';
            return json_FormatValueStringEscape;
        }
        Format->NameIndex++;
        return json_FormatValueString;
    }
    else
    {
        *Character = '"';
        return json_FormatValueNext;
    }
}


static tJsonFormatState JsonFormatValueLiteral(tJsonFormat *Format, uint8_t *Character)
{
    if (Format->NameIndex < Format->Element->Name.Length)
    {
        *Character = JsonStringGetCharacter(&Format->Element->Name, Format->NameIndex);
        Format->NameIndex++;
        return json_FormatValueLiteral;
    }
    else
    {
        return JsonFormatValueNext(Format, Character);
    }
}


static tJsonFormatState JsonFormatValueStart(tJsonFormat *Format, uint8_t *Character)
{
    if (Format->NewLine)
    {
        *Character = '\n';
        Format->NewLine = 0;
        Format->SpaceCount = Format->Indent * Format->IndentSize;
        return json_FormatValueStart;
    }
    else if (Format->SpaceCount != 0)
    {
        *Character = ' ';
        Format->SpaceCount--;
        return json_FormatValueStart;
    }
    else if (Format->Element->Type == json_TypeObject)
    {
        if (Format->Element->Child == NULL)
        {
            *Character = '{';
            return json_FormatValueEnd;
        }
        else
        {
            *Character = '{';
            Format->Element = Format->Element->Child;
            if (Format->Type == json_FormatIndent)
            {
                Format->NewLine = 1;
                Format->Indent++;
            }
            else if (Format->Type == json_FormatSpace)
            {
                Format->SpaceCount = 1;
            }
            return json_FormatValueStart;
        }
    }
    else if (Format->Element->Type == json_TypeArray)
    {
        *Character = '[';
        if (Format->Element->Child == NULL)
        {
            return json_FormatValueEnd;
        }
        else
        {
            Format->Element = Format->Element->Child;
            if (Format->Type == json_FormatIndent)
            {
                Format->Indent++;
                Format->NewLine = (Format->Element->Type == json_TypeObject) || (Format->Element->Type == json_TypeArray) || (Format->Element->Next != NULL);
                if (!Format->NewLine)
                {
                    Format->SpaceCount = 1;
                }
            }
            else if (Format->Type == json_FormatSpace)
            {
                Format->SpaceCount = 1;
            }
            return json_FormatValueStart;
        }
    }
    else if (Format->Element->Type == json_TypeKey)
    {
        *Character = '"';
        Format->NameIndex = 0;
        return json_FormatKey;
    }
    else if (Format->Element->Type == json_TypeValueString)
    {
        *Character = '"';
        Format->NameIndex = 0;
        return json_FormatValueString;
    }
    else if (Format->Element->Type == json_TypeValueLiteral)
    {
        Format->NameIndex = 0;
        return JsonFormatValueLiteral(Format, Character);
    }

    return json_FormatError;
}


static void JsonFormatSetUp(tJsonFormat *Format, tJsonFormatType Type, size_t IndentSize, tJsonElement *RootElement)
{
    Format->Type = Type;
    Format->State = json_FormatValueStart;
    Format->Element = RootElement->Child;
    Format->NameIndex = 0;
    Format->Indent = 0;
    Format->IndentSize = IndentSize;
    Format->SpaceCount = 0;
    Format->NewLine = 0;
}


void JsonFormatSetUpCompress(tJsonFormat *Format, tJsonElement *RootElement)
{
    JsonFormatSetUp(Format, json_FormatCompress, 0, RootElement);
}


void JsonFormatSetUpSpace(tJsonFormat *Format, tJsonElement *RootElement)
{
    JsonFormatSetUp(Format, json_FormatSpace, 0, RootElement);
}


void JsonFormatSetUpIndent(tJsonFormat *Format, size_t IndentSize, tJsonElement *RootElement)
{
    JsonFormatSetUp(Format, json_FormatIndent, (IndentSize == 0) ? JSON_FORMAT_INDENT_SPACE_COUNT : IndentSize, RootElement);
}


void JsonFormatCleanUp(tJsonFormat *Format)
{
    Format->Type = json_FormatCompress;
    Format->State = json_FormatComplete;
    Format->Element = NULL;
    Format->NameIndex = 0;
    Format->Indent = 0;
    Format->IndentSize = 0;
    Format->SpaceCount = 0;
    Format->NewLine = 0;
}


int JsonFormat(tJsonFormat *Format, uint8_t *Character)
{
    *Character = '\0';

    if ((Format->State != json_FormatComplete) && (Format->State != json_FormatError))
    {
        if (Format->Element == NULL)
        {
            Format->State = json_FormatError;
        }
        else
        {
            switch (Format->State)
            {
                case json_FormatKey:
                    Format->State = JsonFormatKey(Format, Character);
                break;

                case json_FormatKeyEscape:
                    Format->State = JsonFormatKeyEscape(Format, Character);
                break;

                case json_FormatKeyEnd:
                    Format->State = JsonFormatKeyEnd(Format, Character);
                break;

                case json_FormatValueStart:
                    Format->State = JsonFormatValueStart(Format, Character);
                break;

                case json_FormatValueString:
                    Format->State = JsonFormatValueString(Format, Character);
                break;

                case json_FormatValueStringEscape:
                    Format->State = JsonFormatValueStringEscape(Format, Character);
                break;

                case json_FormatValueLiteral:
                    Format->State = JsonFormatValueLiteral(Format, Character);
                break;

                case json_FormatValueNext:
                    Format->State = JsonFormatValueNext(Format, Character);
                break;

                case json_FormatValueEnd:
                    Format->State = JsonFormatValueEnd(Format, Character);
                break;

                default:
                    Format->State = json_FormatError;
                break;
            }
        }
    }

    if (Format->State == json_FormatError)
    {
        return JSON_FORMAT_ERROR;
    }
    else if (Format->State == json_FormatComplete)
    {
        return JSON_FORMAT_COMPLETE;
    }
    else
    {
        return JSON_FORMAT_INCOMPLETE;
    }
}
