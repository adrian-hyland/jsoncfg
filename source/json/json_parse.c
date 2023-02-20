#include "json_character.h"
#include "json_parse.h"


static bool JsonParseAllocateElement(tJsonParse *Parse, tJsonType Type)
{
    bool ok;

    if (Parse->AllocateChild)
    {
        ok = JsonElementAllocateChild(Parse->Element, Type);
        Parse->Element = Parse->Element->Child;
    }
    else
    {
        ok = JsonElementAllocateNext(Parse->Element, Type);
        Parse->Element = Parse->Element->Next;
    }

    return ok;
}


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
        Parse->AllocateChild = true;
        return json_ParseKeyEnd;
    }
    else if (!JsonStringAddCharacter(&Parse->Element->Name, Character))
    {
        return json_ParseError;
    }

    return json_ParseKey;
}


static tJsonParseState JsonParseKeyStart(tJsonParse *Parse, uint8_t Character)
{
    if (Character == '"')
    {
        if (!JsonParseAllocateElement(Parse, json_TypeKey))
        {
            return json_ParseError;
        }

        return json_ParseKey;
    }
    else if (Character == '}')
    {
        if (Parse->Element->Type != json_TypeObject)
        {
            return json_ParseError;
        }

        return json_ParseValueEnd;
    }
    else if (Character == '/')
    {
        Parse->CommentState = Parse->State;
        return json_ParseCommentStart;
    }
    else if (!JsonCharacterIsWhitespace(Character))
    {
        return json_ParseError;
    }

    return json_ParseKeyStart;
}


static tJsonParseState JsonParseKeyEnd(tJsonParse *Parse, uint8_t Character)
{
    if (Character == ':')
    {
        return json_ParseValueStart;
    }
    else if (Character == '/')
    {
        Parse->CommentState = Parse->State;
        return json_ParseCommentStart;
    }
    else if (!JsonCharacterIsWhitespace(Character))
    {
        return json_ParseError;
    }

    return json_ParseKeyEnd;
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

    return json_ParseValueString;
}


static tJsonParseState JsonParseValueEnd(tJsonParse *Parse, uint8_t Character)
{
    Parse->AllocateChild = false;
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
    else if (Character == '/')
    {
        Parse->CommentState = Parse->State;
        return json_ParseCommentStart;
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

    return json_ParseValueEnd;
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

    return json_ParseValueLiteral;
}


static tJsonParseState JsonParseValueStart(tJsonParse *Parse, uint8_t Character)
{
    if (Character == '{')
    {
        if (!JsonParseAllocateElement(Parse, json_TypeObject))
        {
            return json_ParseError;
        }

        Parse->AllocateChild = true;
        return json_ParseKeyStart;
    }
    else if (Character == '[')
    {
        if (!JsonParseAllocateElement(Parse, json_TypeArray))
        {
            return json_ParseError;
        }

        Parse->AllocateChild = true;
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
        if (!JsonParseAllocateElement(Parse, json_TypeValueString))
        {
            return json_ParseError;
        }
        return json_ParseValueString;
    }
    else if (Character == '/')
    {
        Parse->CommentState = Parse->State;
        return json_ParseCommentStart;
    }
    else if (Character == '\0')
    {
        if (Parse->Element->Type != json_TypeRoot)
        {
            return json_ParseError;
        }
        return json_ParseComplete;
    }
    else if (JsonCharacterIsLiteral(Character))
    {
        if (!JsonParseAllocateElement(Parse, json_TypeValueLiteral) || !JsonStringAddCharacter(&Parse->Element->Name, Character))
        {
            return json_ParseError;
        }
        return json_ParseValueLiteral;
    }
    else if (!JsonCharacterIsWhitespace(Character))
    {
        return json_ParseError;
    }

    return json_ParseValueStart;
}


static tJsonParseState JsonParseCommentStart(tJsonParse *Parse, uint8_t Character)
{
    if (((Character != '/') && (Character != '*')) || (!Parse->StripComments && !JsonParseAllocateElement(Parse, json_TypeComment)))
    {
        return json_ParseError;
    }

    return (Character == '/') ? json_ParseCommentLine : json_ParseCommentBlock;
}


static tJsonParseState JsonParseCommentLine(tJsonParse *Parse, uint8_t Character)
{
    tJsonParseState State;

    if ((Character == '\r') || (Character == '\n'))
    {
        State = Parse->CommentState;
        Parse->CommentState = json_ParseError;
        if (!Parse->StripComments)
        {
            Parse->AllocateChild = false;
        }
        return State;
    }
    else if (!Parse->StripComments && !JsonStringAddCharacter(&Parse->Element->Name, Character))
    {
        return json_ParseError;
    }

    return json_ParseCommentLine;
}


static tJsonParseState JsonParseCommentBlock(tJsonParse *Parse, uint8_t Character)
{
    if (Character == '*')
    {
        return json_ParseCommentBlockEnd;
    }
    else if ((Character == '\r') || (Character == '\n'))
    {
        return json_ParseCommentBlockLine;
    }
    else if (!Parse->StripComments && !JsonStringAddCharacter(&Parse->Element->Name, Character))
    {
        return json_ParseError;
    }

    return json_ParseCommentBlock;
}


static tJsonParseState JsonParseCommentBlockLine(tJsonParse *Parse, uint8_t Character)
{
    if (!JsonCharacterIsWhitespace(Character))
    {
        if (!Parse->StripComments)
        {
            Parse->AllocateChild = false;
            if (!JsonParseAllocateElement(Parse, json_TypeComment))
            {
                return json_ParseError;
            }
        }

        return JsonParseCommentBlock(Parse, Character);
    }

    return json_ParseCommentBlockLine;
}


static tJsonParseState JsonParseCommentBlockEnd(tJsonParse *Parse, uint8_t Character)
{
    tJsonParseState State;

    if (Character == '/')
    {
        State = Parse->CommentState;
        Parse->CommentState = json_ParseError;
        if (!Parse->StripComments)
        {
            Parse->AllocateChild = false;
        }
        return State;
    }
    else if (!Parse->StripComments && !JsonStringAddCharacter(&Parse->Element->Name, '*'))
    {
        return json_ParseError;
    }

    return JsonParseCommentBlock(Parse, Character);
}


void JsonParseSetUp(tJsonParse *Parse, bool StripComments, tJsonElement *RootElement)
{
    Parse->State = json_ParseValueStart;
    Parse->Element = RootElement;
    Parse->CommentState = json_ParseError;
    Parse->AllocateChild = true;
    Parse->StripComments = StripComments;
}


void JsonParseCleanUp(tJsonParse *Parse)
{
    Parse->State = json_ParseComplete;
    Parse->Element = NULL;
    Parse->CommentState = json_ParseError;
    Parse->AllocateChild = false;
    Parse->StripComments = false;
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

                case json_ParseCommentStart:
                    Parse->State = JsonParseCommentStart(Parse, Character);
                break;

                case json_ParseCommentLine:
                    Parse->State = JsonParseCommentLine(Parse, Character);
                break;

                case json_ParseCommentBlock:
                    Parse->State = JsonParseCommentBlock(Parse, Character);
                break;

                case json_ParseCommentBlockLine:
                    Parse->State = JsonParseCommentBlockLine(Parse, Character);
                break;

                case json_ParseCommentBlockEnd:
                    Parse->State = JsonParseCommentBlockEnd(Parse, Character);
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
