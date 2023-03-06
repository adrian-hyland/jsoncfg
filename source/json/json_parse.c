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


static bool JsonParseUtf8IsComplete(tJsonParse *Parse, tJsonUtf8Unit CodeUnit, tJsonUtf8Code *Code)
{
	if (!JsonUtf8CodeAddUnit(&Parse->Utf8Code, CodeUnit))
	{
		Parse->State = json_ParseError;
		return false;
	}
	
	if (!JsonUtf8CodeIsValid(Parse->Utf8Code))
	{
		return false;
	}

	*Code = Parse->Utf8Code;
	Parse->Utf8Code = 0;
	return true;
}


static tJsonParseState JsonParseKeyUtf16Escape(tJsonParse *Parse, tJsonUtf8Code Code)
{
	if (Code != '\\')
	{
		return json_ParseError;
	}

	return json_ParseKeyUtf16;
}


static tJsonParseState JsonParseKeyUtf16(tJsonParse *Parse, tJsonUtf8Code Code)
{
	if (Code != 'u')
	{
		return json_ParseError;
	}

	return json_ParseKeyUtf16Digit1;
}


static tJsonParseState JsonParseKeyUtf16Digit1(tJsonParse *Parse, tJsonUtf8Code Code)
{
	if (!JsonCharacterIsHexDigit(Code) || !JsonUtf16CodeAddNibble(&Parse->Utf16Code, JsonCharacterToHexDigit(Code)))
	{
		return json_ParseError;
	}

	return json_ParseKeyUtf16Digit2;
}


static tJsonParseState JsonParseKeyUtf16Digit2(tJsonParse *Parse, tJsonUtf8Code Code)
{
	if (!JsonCharacterIsHexDigit(Code) || !JsonUtf16CodeAddNibble(&Parse->Utf16Code, JsonCharacterToHexDigit(Code)))
	{
		return json_ParseError;
	}

	return json_ParseKeyUtf16Digit3;
}


static tJsonParseState JsonParseKeyUtf16Digit3(tJsonParse *Parse, tJsonUtf8Code Code)
{
	if (!JsonCharacterIsHexDigit(Code) || !JsonUtf16CodeAddNibble(&Parse->Utf16Code, JsonCharacterToHexDigit(Code)))
	{
		return json_ParseError;
	}

	return json_ParseKeyUtf16Digit4;
}


static tJsonParseState JsonParseKeyUtf16Digit4(tJsonParse *Parse, tJsonUtf8Code Code)
{
	if (JsonCharacterIsHexDigit(Code) && JsonUtf16CodeAddNibble(&Parse->Utf16Code, JsonCharacterToHexDigit(Code)))
	{
		if (JsonUtf16CodeIsValid(Parse->Utf16Code))
		{
			if (JsonStringAddCharacter(&Parse->Element->Name, JsonUtf16CodeGetCharacter(Parse->Utf16Code)))
			{
				return json_ParseKey;
			}
		}
		else if (JsonUtf16UnitIsHighSurrogate(Parse->Utf16Code))
		{
			return json_ParseKeyUtf16Escape;
		}
	}

	return json_ParseError;
}


static tJsonParseState JsonParseKeyEscape(tJsonParse *Parse, tJsonUtf8Code Code)
{
	Code = JsonCharacterFromEscape(Code);

	if (Code == 'u')
	{
		Parse->Utf16Code = 0;
		return json_ParseKeyUtf16Digit1;
	}
	else if (!JsonStringAddUtf8Code(&Parse->Element->Name, Code))
	{
		return json_ParseError;
	}

	return json_ParseKey;
}


static tJsonParseState JsonParseKey(tJsonParse *Parse, tJsonUtf8Code Code)
{
	if (Code == '\\')
	{
		return json_ParseKeyEscape;
	}
	else if (Code == '"')
	{
		Parse->AllocateChild = true;
		return json_ParseKeyEnd;
	}
	else if (!JsonStringAddUtf8Code(&Parse->Element->Name, Code))
	{
		return json_ParseError;
	}

	return json_ParseKey;
}


static tJsonParseState JsonParseKeyStart(tJsonParse *Parse, tJsonUtf8Code Code)
{
	if (Code == '"')
	{
		if (!JsonParseAllocateElement(Parse, json_TypeKey))
		{
			return json_ParseError;
		}

		return json_ParseKey;
	}
	else if (Code == '}')
	{
		if (Parse->Element->Type != json_TypeObject)
		{
			return json_ParseError;
		}

		return json_ParseValueEnd;
	}
	else if (Code == '/')
	{
		Parse->CommentState = Parse->State;
		return json_ParseCommentStart;
	}
	else if (!JsonCharacterIsWhitespace(Code))
	{
		return json_ParseError;
	}

	return json_ParseKeyStart;
}


static tJsonParseState JsonParseKeyEnd(tJsonParse *Parse, tJsonUtf8Code Code)
{
	if (Code == ':')
	{
		return json_ParseValueStart;
	}
	else if (Code == '/')
	{
		Parse->CommentState = Parse->State;
		return json_ParseCommentStart;
	}
	else if (!JsonCharacterIsWhitespace(Code))
	{
		return json_ParseError;
	}

	return json_ParseKeyEnd;
}


static tJsonParseState JsonParseValueStringUtf16Escape(tJsonParse *Parse, tJsonUtf8Code Code)
{
	if (Code != '\\')
	{
		return json_ParseError;
	}

	return json_ParseValueStringUtf16;
}


static tJsonParseState JsonParseValueStringUtf16(tJsonParse *Parse, tJsonUtf8Code Code)
{
	if (Code != 'u')
	{
		return json_ParseError;
	}

	return json_ParseValueStringUtf16Digit1;
}


static tJsonParseState JsonParseValueStringUtf16Digit1(tJsonParse *Parse, tJsonUtf8Code Code)
{
	if (!JsonCharacterIsHexDigit(Code) || !JsonUtf16CodeAddNibble(&Parse->Utf16Code, JsonCharacterToHexDigit(Code)))
	{
		return json_ParseError;
	}

	return json_ParseValueStringUtf16Digit2;
}


static tJsonParseState JsonParseValueStringUtf16Digit2(tJsonParse *Parse, tJsonUtf8Code Code)
{
	if (!JsonCharacterIsHexDigit(Code) || !JsonUtf16CodeAddNibble(&Parse->Utf16Code, JsonCharacterToHexDigit(Code)))
	{
		return json_ParseError;
	}

	return json_ParseValueStringUtf16Digit3;
}


static tJsonParseState JsonParseValueStringUtf16Digit3(tJsonParse *Parse, tJsonUtf8Code Code)
{
	if (!JsonCharacterIsHexDigit(Code) || !JsonUtf16CodeAddNibble(&Parse->Utf16Code, JsonCharacterToHexDigit(Code)))
	{
		return json_ParseError;
	}

	return json_ParseValueStringUtf16Digit4;
}


static tJsonParseState JsonParseValueStringUtf16Digit4(tJsonParse *Parse, tJsonUtf8Code Code)
{
	if (JsonCharacterIsHexDigit(Code) && JsonUtf16CodeAddNibble(&Parse->Utf16Code, JsonCharacterToHexDigit(Code)))
	{
		if (JsonUtf16CodeIsValid(Parse->Utf16Code))
		{
			if (JsonStringAddCharacter(&Parse->Element->Name, JsonUtf16CodeGetCharacter(Parse->Utf16Code)))
			{
				return json_ParseValueString;
			}
		}
		else if (JsonUtf16UnitIsHighSurrogate(Parse->Utf16Code))
		{
			return json_ParseValueStringUtf16Escape;
		}
	}

	return json_ParseError;
}


static tJsonParseState JsonParseValueStringEscape(tJsonParse *Parse, tJsonUtf8Code Code)
{
	Code = JsonCharacterFromEscape(Code);

	if (Code == 'u')
	{
		Parse->Utf16Code = 0;
		return json_ParseValueStringUtf16Digit1;
	}
	else if (!JsonStringAddUtf8Code(&Parse->Element->Name, Code))
	{
		return json_ParseError;
	}

	return json_ParseValueString;
}


static tJsonParseState JsonParseValueString(tJsonParse *Parse, tJsonUtf8Code Code)
{
	if (Code == '\\')
	{
		return json_ParseValueStringEscape;
	}
	else if (Code == '"')
	{
		return json_ParseValueEnd;
	}
	else if (!JsonStringAddUtf8Code(&Parse->Element->Name, Code))
	{
		return json_ParseError;
	}

	return json_ParseValueString;
}


static tJsonParseState JsonParseValueEnd(tJsonParse *Parse, tJsonUtf8Code Code)
{
	Parse->AllocateChild = false;
	if (Code == ',')
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
	else if (Code == '}')
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
	else if (Code == ']')
	{
		if ((Parse->Element->Parent == NULL) || (Parse->Element->Parent->Type != json_TypeArray))
		{
			return json_ParseError;
		}
		Parse->Element = Parse->Element->Parent;
		return json_ParseValueEnd;
	}
	else if (Code == '/')
	{
		Parse->CommentState = Parse->State;
		return json_ParseCommentStart;
	}
	else if (Code == '\0')
	{
		if ((Parse->Element->Parent == NULL) || (Parse->Element->Parent->Type != json_TypeRoot))
		{
			return json_ParseError;
		}
		Parse->Element = Parse->Element->Parent;
		return json_ParseComplete;
	}
	else if (!JsonCharacterIsWhitespace(Code))
	{
		return json_ParseError;
	}

	return json_ParseValueEnd;
}


static tJsonParseState JsonParseValueLiteral(tJsonParse *Parse, tJsonUtf8Code Code)
{
	if (!JsonCharacterIsLiteral(Code))
	{
		return JsonParseValueEnd(Parse, Code);
	}
	else if (!JsonStringAddUtf8Code(&Parse->Element->Name, Code))
	{
		return json_ParseError;
	}

	return json_ParseValueLiteral;
}


static tJsonParseState JsonParseValueStart(tJsonParse *Parse, tJsonUtf8Code Code)
{
	if (Code == '{')
	{
		if (!JsonParseAllocateElement(Parse, json_TypeObject))
		{
			return json_ParseError;
		}

		Parse->AllocateChild = true;
		return json_ParseKeyStart;
	}
	else if (Code == '[')
	{
		if (!JsonParseAllocateElement(Parse, json_TypeArray))
		{
			return json_ParseError;
		}

		Parse->AllocateChild = true;
		return json_ParseValueStart;
	}
	else if (Code == ']')
	{
		if (Parse->Element->Type != json_TypeArray)
		{
			return json_ParseError;
		}
		return json_ParseValueEnd;
	}
	else if (Code == '"')
	{
		if (!JsonParseAllocateElement(Parse, json_TypeValueString))
		{
			return json_ParseError;
		}
		return json_ParseValueString;
	}
	else if (Code == '/')
	{
		Parse->CommentState = Parse->State;
		return json_ParseCommentStart;
	}
	else if (Code == '\0')
	{
		if (Parse->Element->Type != json_TypeRoot)
		{
			return json_ParseError;
		}
		return json_ParseComplete;
	}
	else if (JsonCharacterIsLiteral(Code))
	{
		if (!JsonParseAllocateElement(Parse, json_TypeValueLiteral) || !JsonStringAddUtf8Code(&Parse->Element->Name, Code))
		{
			return json_ParseError;
		}
		return json_ParseValueLiteral;
	}
	else if (!JsonCharacterIsWhitespace(Code))
	{
		return json_ParseError;
	}

	return json_ParseValueStart;
}


static tJsonParseState JsonParseCommentStart(tJsonParse *Parse, tJsonUtf8Code Code)
{
	if (((Code != '/') && (Code != '*')) || (!Parse->StripComments && !JsonParseAllocateElement(Parse, json_TypeComment)))
	{
		return json_ParseError;
	}

	return (Code == '/') ? json_ParseCommentLine : json_ParseCommentBlock;
}


static tJsonParseState JsonParseCommentLine(tJsonParse *Parse, tJsonUtf8Code Code)
{
	tJsonParseState State;

	if ((Code == '\r') || (Code == '\n'))
	{
		State = Parse->CommentState;
		Parse->CommentState = json_ParseError;
		if (!Parse->StripComments)
		{
			Parse->AllocateChild = false;
		}
		return State;
	}
	else if (!Parse->StripComments && !JsonStringAddUtf8Code(&Parse->Element->Name, Code))
	{
		return json_ParseError;
	}

	return json_ParseCommentLine;
}


static tJsonParseState JsonParseCommentBlock(tJsonParse *Parse, tJsonUtf8Code Code)
{
	if (Code == '*')
	{
		return json_ParseCommentBlockEnd;
	}
	else if ((Code == '\r') || (Code == '\n'))
	{
		return json_ParseCommentBlockLine;
	}
	else if (!Parse->StripComments && !JsonStringAddUtf8Code(&Parse->Element->Name, Code))
	{
		return json_ParseError;
	}

	return json_ParseCommentBlock;
}


static tJsonParseState JsonParseCommentBlockLine(tJsonParse *Parse, tJsonUtf8Code Code)
{
	if (!JsonCharacterIsWhitespace(Code))
	{
		if (!Parse->StripComments)
		{
			Parse->AllocateChild = false;
			if (!JsonParseAllocateElement(Parse, json_TypeComment))
			{
				return json_ParseError;
			}
		}

		return JsonParseCommentBlock(Parse, Code);
	}

	return json_ParseCommentBlockLine;
}


static tJsonParseState JsonParseCommentBlockEnd(tJsonParse *Parse, tJsonUtf8Code Code)
{
	tJsonParseState State;

	if (Code == '/')
	{
		State = Parse->CommentState;
		Parse->CommentState = json_ParseError;
		if (!Parse->StripComments)
		{
			Parse->AllocateChild = false;
		}
		return State;
	}
	else if (!Parse->StripComments && !JsonStringAddUtf8Code(&Parse->Element->Name, '*'))
	{
		return json_ParseError;
	}

	return JsonParseCommentBlock(Parse, Code);
}


void JsonParseSetUp(tJsonParse *Parse, bool StripComments, tJsonElement *RootElement)
{
	Parse->State = json_ParseValueStart;
	Parse->Element = RootElement;
	Parse->Utf8Code = 0;
	Parse->Utf16Code = 0;
	Parse->CommentState = json_ParseError;
	Parse->AllocateChild = true;
	Parse->StripComments = StripComments;
}


void JsonParseCleanUp(tJsonParse *Parse)
{
	Parse->State = json_ParseComplete;
	Parse->Element = NULL;
	Parse->Utf8Code = 0;
	Parse->Utf16Code = 0;
	Parse->CommentState = json_ParseError;
	Parse->AllocateChild = false;
	Parse->StripComments = false;
}


int JsonParse(tJsonParse *Parse, tJsonUtf8Unit CodeUnit)
{
	tJsonUtf8Code Code;

	if ((Parse->State != json_ParseComplete) && (Parse->State != json_ParseError) && JsonParseUtf8IsComplete(Parse, CodeUnit, &Code))
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
					Parse->State = JsonParseKeyStart(Parse, Code);
				break;

				case json_ParseKey:
					Parse->State = JsonParseKey(Parse, Code);
				break;

				case json_ParseKeyEscape:
					Parse->State = JsonParseKeyEscape(Parse, Code);
				break;

				case json_ParseKeyUtf16Escape:
					Parse->State = JsonParseKeyUtf16Escape(Parse, Code);
				break;

				case json_ParseKeyUtf16:
					Parse->State = JsonParseKeyUtf16(Parse, Code);
				break;

				case json_ParseKeyUtf16Digit1:
					Parse->State = JsonParseKeyUtf16Digit1(Parse, Code);
				break;

				case json_ParseKeyUtf16Digit2:
					Parse->State = JsonParseKeyUtf16Digit2(Parse, Code);
				break;

				case json_ParseKeyUtf16Digit3:
					Parse->State = JsonParseKeyUtf16Digit3(Parse, Code);
				break;

				case json_ParseKeyUtf16Digit4:
					Parse->State = JsonParseKeyUtf16Digit4(Parse, Code);
				break;

				case json_ParseKeyEnd:
					Parse->State = JsonParseKeyEnd(Parse, Code);
				break;

				case json_ParseValueStart:
					Parse->State = JsonParseValueStart(Parse, Code);
				break;

				case json_ParseValueString:
					Parse->State = JsonParseValueString(Parse, Code);
				break;

				case json_ParseValueStringEscape:
					Parse->State = JsonParseValueStringEscape(Parse, Code);
				break;


				case json_ParseValueStringUtf16Escape:
					Parse->State = JsonParseValueStringUtf16Escape(Parse, Code);
				break;

				case json_ParseValueStringUtf16:
					Parse->State = JsonParseValueStringUtf16(Parse, Code);
				break;

				case json_ParseValueStringUtf16Digit1:
					Parse->State = JsonParseValueStringUtf16Digit1(Parse, Code);
				break;

				case json_ParseValueStringUtf16Digit2:
					Parse->State = JsonParseValueStringUtf16Digit2(Parse, Code);
				break;

				case json_ParseValueStringUtf16Digit3:
					Parse->State = JsonParseValueStringUtf16Digit3(Parse, Code);
				break;

				case json_ParseValueStringUtf16Digit4:
					Parse->State = JsonParseValueStringUtf16Digit4(Parse, Code);
				break;

				case json_ParseValueLiteral:
					Parse->State = JsonParseValueLiteral(Parse, Code);
				break;

				case json_ParseValueEnd:
					Parse->State = JsonParseValueEnd(Parse, Code);
				break;

				case json_ParseCommentStart:
					Parse->State = JsonParseCommentStart(Parse, Code);
				break;

				case json_ParseCommentLine:
					Parse->State = JsonParseCommentLine(Parse, Code);
				break;

				case json_ParseCommentBlock:
					Parse->State = JsonParseCommentBlock(Parse, Code);
				break;

				case json_ParseCommentBlockLine:
					Parse->State = JsonParseCommentBlockLine(Parse, Code);
				break;

				case json_ParseCommentBlockEnd:
					Parse->State = JsonParseCommentBlockEnd(Parse, Code);
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
