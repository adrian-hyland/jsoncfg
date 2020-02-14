#include "json_character.h"
#include "json_parse.h"


static tJsonParseState JsonParseKeyEscape(tJsonParse *Parse, uint8_t Character)
{
    Character = JsonCharacterFromEscape(Character);

    if (!JsonStringAddCharacter(&Parse->Element->Name, Character))
    {
        return json_ParseError;
    }

    return json_ParseKey;
}


static tJsonParseState JsonParseKey(tJsonParse *Parse, uint8_t Character)
{
    if (Character == '\\')
    {
        return json_ParseKeyEscape;
    }
    else if (Character == '"')
    {
        return json_ParseKeyEnd;
    }
    else if (!JsonStringAddCharacter(&Parse->Element->Name, Character))
    {
        return json_ParseError;
    }

    return Parse->State;
}


static tJsonParseState JsonParseKeyStart(tJsonParse *Parse, uint8_t Character)
{
    if (Character == '"')
    {
        if (Parse->Element->Type == json_TypeObject)
        {
            Parse->Element->Child = JsonElementAllocate(json_TypeKey, Parse->Element);
            if (Parse->Element->Child == NULL)
            {
                return json_ParseError;
            }
            Parse->Element = Parse->Element->Child;
        }
        else
        {
            Parse->Element->Next = JsonElementAllocate(json_TypeKey, Parse->Element->Parent);
            if (Parse->Element->Next == NULL)
            {
                return json_ParseError;
            }
            Parse->Element = Parse->Element->Next;
        }
        return json_ParseKey;
    }
    else if (Character == '}')
    {
        return json_ParseValueEnd;
    }
    else if (!JsonCharacterIsWhitespace(Character))
    {
        return json_ParseError;
    }

    return Parse->State;
}


static tJsonParseState JsonParseKeyEnd(tJsonParse *Parse, uint8_t Character)
{
    if (Character == ':')
    {
        return json_ParseValueStart;
    }
    else if (!JsonCharacterIsWhitespace(Character))
    {
        return json_ParseError;
    }

    return Parse->State;
}


static tJsonParseState JsonParseValueStringEscape(tJsonParse *Parse, uint8_t Character)
{
    Character = JsonCharacterFromEscape(Character);

    if (!JsonStringAddCharacter(&Parse->Element->Name, Character))
    {
        return json_ParseError;
    }

    return json_ParseValueString;
}


static tJsonParseState JsonParseValueString(tJsonParse *Parse, uint8_t Character)
{
    if (Character == '\\')
    {
        return json_ParseValueStringEscape;
    }
    else if (Character == '"')
    {
        return json_ParseValueEnd;
    }
    else if (!JsonStringAddCharacter(&Parse->Element->Name, Character))
    {
        return json_ParseError;
    }

    return Parse->State;
}


static tJsonParseState JsonParseValueEnd(tJsonParse *Parse, uint8_t Character)
{
    if (Character == ',')
    {
        if (Parse->Element->Parent == NULL)
        {
            return json_ParseError;
        }
        else if (Parse->Element->Parent->Type == json_TypeKey)
        {
            Parse->Element = Parse->Element->Parent;
            return json_ParseKeyStart;
        }
        else if (Parse->Element->Parent->Type == json_TypeArray)
        {
            return json_ParseValueStart;
        }
        else
        {
            return json_ParseError;
        }
    }
    else if (Character == '}')
    {
        if ((Parse->Element->Parent == NULL) || (Parse->Element->Parent->Type != json_TypeKey))
        {
            return json_ParseError;
        }
        Parse->Element = Parse->Element->Parent;
        if ((Parse->Element->Parent == NULL) || (Parse->Element->Parent->Type != json_TypeObject))
        {
            return json_ParseError;
        }
        Parse->Element = Parse->Element->Parent;
        return json_ParseValueEnd;
    }
    else if (Character == ']')
    {
        if ((Parse->Element->Parent == NULL) || (Parse->Element->Parent->Type != json_TypeArray))
        {
            return json_ParseError;
        }
        Parse->Element = Parse->Element->Parent;
        return json_ParseValueEnd;
    }
    else if (Character == '\0')
    {
        if ((Parse->Element->Parent == NULL) || (Parse->Element->Parent->Type != json_TypeRoot))
        {
            return json_ParseError;
        }
        Parse->Element = Parse->Element->Parent;
        return json_ParseComplete;
    }
    else if (!JsonCharacterIsWhitespace(Character))
    {
        return json_ParseError;
    }

    return Parse->State;
}


static tJsonParseState JsonParseValueLiteral(tJsonParse *Parse, uint8_t Character)
{
    if (!JsonCharacterIsLiteral(Character))
    {
        return JsonParseValueEnd(Parse, Character);
    }
    else if (!JsonStringAddCharacter(&Parse->Element->Name, Character))
    {
        return json_ParseError;
    }

    return Parse->State;
}


static tJsonParseState JsonParseValueStart(tJsonParse *Parse, uint8_t Character)
{
    if (Character == '{')
    {
        if ((Parse->Element->Parent == NULL) || (Parse->Element->Parent->Type != json_TypeArray) || ((Parse->Element->Type == json_TypeArray) && (Parse->Element->Child == NULL)))
        {
            Parse->Element->Child = JsonElementAllocate(json_TypeObject, Parse->Element);
            if (Parse->Element->Child == NULL)
            {
                return json_ParseError;
            }
            Parse->Element = Parse->Element->Child;
        }
        else
        {
            Parse->Element->Next = JsonElementAllocate(json_TypeObject, Parse->Element->Parent);
            if (Parse->Element->Next == NULL)
            {
                return json_ParseError;
            }
            Parse->Element = Parse->Element->Next;
        }
        return json_ParseKeyStart;
    }
    else if (Character == '[')
    {
        if ((Parse->Element->Parent == NULL) || (Parse->Element->Parent->Type != json_TypeArray) || ((Parse->Element->Type == json_TypeArray) && (Parse->Element->Child == NULL)))
        {
            Parse->Element->Child = JsonElementAllocate(json_TypeArray, Parse->Element);
            if (Parse->Element->Child == NULL)
            {
                return json_ParseError;
            }
            Parse->Element = Parse->Element->Child;
        }
        else
        {
            Parse->Element->Next = JsonElementAllocate(json_TypeArray, Parse->Element->Parent);
            if (Parse->Element->Next == NULL)
            {
                return json_ParseError;
            }
            Parse->Element = Parse->Element->Next;
        }
        return json_ParseValueStart;
    }
    else if (Character == ']')
    {
        if (Parse->Element->Type != json_TypeArray)
        {
            return json_ParseError;
        }
        return json_ParseValueEnd;
    }
    else if (Character == '"')
    {
        if ((Parse->Element->Parent == NULL) || (Parse->Element->Parent->Type != json_TypeArray) || ((Parse->Element->Type == json_TypeArray) && (Parse->Element->Child == NULL)))
        {
            Parse->Element->Child = JsonElementAllocate(json_TypeValueString, Parse->Element);
            if (Parse->Element->Child == NULL)
            {
                return json_ParseError;
            }
            Parse->Element = Parse->Element->Child;
        }
        else
        {
            Parse->Element->Next = JsonElementAllocate(json_TypeValueString, Parse->Element->Parent);
            if (Parse->Element->Next == NULL)
            {
                return json_ParseError;
            }
            Parse->Element = Parse->Element->Next;
        }
        return json_ParseValueString;
    }
    else if (JsonCharacterIsLiteral(Character))
    {
        if ((Parse->Element->Parent == NULL) || (Parse->Element->Parent->Type != json_TypeArray) || ((Parse->Element->Type == json_TypeArray) && (Parse->Element->Child == NULL)))
        {
            Parse->Element->Child = JsonElementAllocate(json_TypeValueLiteral, Parse->Element);
            if (Parse->Element->Child == NULL)
            {
                return json_ParseError;
            }
            Parse->Element = Parse->Element->Child;
        }
        else
        {
            Parse->Element->Next = JsonElementAllocate(json_TypeValueLiteral, Parse->Element->Parent);
            if (Parse->Element->Next == NULL)
            {
                return json_ParseError;
            }
            Parse->Element = Parse->Element->Next;
        }
        if (!JsonStringAddCharacter(&Parse->Element->Name, Character))
        {
            return json_ParseError;
        }
        return json_ParseValueLiteral;
    }
    else if (!JsonCharacterIsWhitespace(Character))
    {
        return json_ParseError;
    }

    return Parse->State;
}


void JsonParseSetUp(tJsonParse *Parse, tJsonElement *RootElement)
{
    Parse->State = json_ParseValueStart;
    Parse->Element = RootElement;
}


void JsonParseCleanUp(tJsonParse *Parse)
{
    Parse->State = json_ParseComplete;
    Parse->Element = NULL;
}


int JsonParse(tJsonParse *Parse, uint8_t Character)
{
    if ((Parse->State != json_ParseComplete) && (Parse->State != json_ParseError))
    {
        if (Parse->Element == NULL)
        {
            Parse->State = json_ParseError;
        }
        else
        {
            switch (Parse->State)
            {
                case json_ParseKeyStart:
                    Parse->State = JsonParseKeyStart(Parse, Character);
                break;

                case json_ParseKey:
                    Parse->State = JsonParseKey(Parse, Character);
                break;

                case json_ParseKeyEscape:
                    Parse->State = JsonParseKeyEscape(Parse, Character);
                break;

                case json_ParseKeyEnd:
                    Parse->State = JsonParseKeyEnd(Parse, Character);
                break;

                case json_ParseValueStart:
                    Parse->State = JsonParseValueStart(Parse, Character);
                break;

                case json_ParseValueString:
                    Parse->State = JsonParseValueString(Parse, Character);
                break;

                case json_ParseValueStringEscape:
                    Parse->State = JsonParseValueStringEscape(Parse, Character);
                break;

                case json_ParseValueLiteral:
                    Parse->State = JsonParseValueLiteral(Parse, Character);
                break;

                case json_ParseValueEnd:
                    Parse->State = JsonParseValueEnd(Parse, Character);
                break;

                default:
                    Parse->State = json_ParseError;
                break;
            }
        }
    }

    if (Parse->State == json_ParseError)
    {
        return JSON_PARSE_ERROR;
    }
    else if (Parse->State == json_ParseComplete)
    {
        return JSON_PARSE_COMPLETE;
    }
    else
    {
        return JSON_PARSE_INCOMPLETE;
    }
}
