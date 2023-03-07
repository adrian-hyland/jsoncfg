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


static tJsonParseState JsonParseSetEscapeState(tJsonParse *Parse, tJsonParseState State)
{
	tJsonParseState PreviousState;

	PreviousState = Parse->EscapeState;
	Parse->EscapeState = State;

	return PreviousState;
}


static tJsonParseState JsonParseSetCommentState(tJsonParse *Parse, tJsonParseState State)
{
	tJsonParseState PreviousState;

	PreviousState = Parse->CommentState;
	Parse->CommentState = State;

	return PreviousState;
}


static tJsonParseState JsonParseUtf16Escape(tJsonParse *Parse, tJsonUtf8Code Code)
{
	if (Code != '\\')
	{
		return json_ParseError;
	}

	return json_ParseUtf16;
}


static tJsonParseState JsonParseUtf16(tJsonParse *Parse, tJsonUtf8Code Code)
{
	if (Code != 'u')
	{
		return json_ParseError;
	}

	return json_ParseUtf16Digit1;
}


static tJsonParseState JsonParseUtf16Digit1(tJsonParse *Parse, tJsonUtf8Code Code)
{
	if (!JsonCharacterIsHexDigit(Code) || !JsonUtf16CodeAddNibble(&Parse->Utf16Code, JsonCharacterToHexDigit(Code)))
	{
		return json_ParseError;
	}

	return json_ParseUtf16Digit2;
}


static tJsonParseState JsonParseUtf16Digit2(tJsonParse *Parse, tJsonUtf8Code Code)
{
	if (!JsonCharacterIsHexDigit(Code) || !JsonUtf16CodeAddNibble(&Parse->Utf16Code, JsonCharacterToHexDigit(Code)))
	{
		return json_ParseError;
	}

	return json_ParseUtf16Digit3;
}


static tJsonParseState JsonParseUtf16Digit3(tJsonParse *Parse, tJsonUtf8Code Code)
{
	if (!JsonCharacterIsHexDigit(Code) || !JsonUtf16CodeAddNibble(&Parse->Utf16Code, JsonCharacterToHexDigit(Code)))
	{
		return json_ParseError;
	}

	return json_ParseUtf16Digit4;
}


static tJsonParseState JsonParseUtf16Digit4(tJsonParse *Parse, tJsonUtf8Code Code)
{
	if (JsonCharacterIsHexDigit(Code) && JsonUtf16CodeAddNibble(&Parse->Utf16Code, JsonCharacterToHexDigit(Code)))
	{
		if (JsonUtf16CodeIsValid(Parse->Utf16Code))
		{
			if (JsonStringAddCharacter(&Parse->Element->Name, JsonUtf16CodeGetCharacter(Parse->Utf16Code)))
			{
				return JsonParseSetEscapeState(Parse, json_ParseError);
			}
		}
		else if (JsonUtf16UnitIsHighSurrogate(Parse->Utf16Code))
		{
			return json_ParseUtf16Escape;
		}
	}

	return json_ParseError;
}


static tJsonParseState JsonParseEscape(tJsonParse *Parse, tJsonUtf8Code Code)
{
	Code = JsonCharacterFromEscape(Code);

	if (Code == 'u')
	{
		Parse->Utf16Code = 0;
		return json_ParseUtf16Digit1;
	}
	else if (!JsonStringAddUtf8Code(&Parse->Element->Name, Code))
	{
		return json_ParseError;
	}

	return JsonParseSetEscapeState(Parse, json_ParseError);
}


static tJsonParseState JsonParseKey(tJsonParse *Parse, tJsonUtf8Code Code)
{
	if (Code == '\\')
	{
		JsonParseSetEscapeState(Parse, Parse->State);
		return json_ParseEscape;
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
		JsonParseSetCommentState(Parse, Parse->State);
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
		JsonParseSetCommentState(Parse, Parse->State);
		return json_ParseCommentStart;
	}
	else if (!JsonCharacterIsWhitespace(Code))
	{
		return json_ParseError;
	}

	return json_ParseKeyEnd;
}


static tJsonParseState JsonParseValueString(tJsonParse *Parse, tJsonUtf8Code Code)
{
	if (Code == '\\')
	{
		JsonParseSetEscapeState(Parse, Parse->State);
		return json_ParseEscape;
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
		JsonParseSetCommentState(Parse, Parse->State);
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
		JsonParseSetCommentState(Parse, Parse->State);
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
	if ((Code == '\r') || (Code == '\n'))
	{
		if (!Parse->StripComments)
		{
			Parse->AllocateChild = false;
		}
		return JsonParseSetCommentState(Parse, json_ParseError);
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
	if (Code == '/')
	{
		if (!Parse->StripComments)
		{
			Parse->AllocateChild = false;
		}
		return JsonParseSetCommentState(Parse, json_ParseError);
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
	Parse->EscapeState = json_ParseError;
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
	Parse->EscapeState = json_ParseError;
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
				case json_ParseEscape:
					Parse->State = JsonParseEscape(Parse, Code);
				break;

				case json_ParseUtf16Escape:
					Parse->State = JsonParseUtf16Escape(Parse, Code);
				break;

				case json_ParseUtf16:
					Parse->State = JsonParseUtf16(Parse, Code);
				break;

				case json_ParseUtf16Digit1:
					Parse->State = JsonParseUtf16Digit1(Parse, Code);
				break;

				case json_ParseUtf16Digit2:
					Parse->State = JsonParseUtf16Digit2(Parse, Code);
				break;

				case json_ParseUtf16Digit3:
					Parse->State = JsonParseUtf16Digit3(Parse, Code);
				break;

				case json_ParseUtf16Digit4:
					Parse->State = JsonParseUtf16Digit4(Parse, Code);
				break;

				case json_ParseKeyStart:
					Parse->State = JsonParseKeyStart(Parse, Code);
				break;

				case json_ParseKey:
					Parse->State = JsonParseKey(Parse, Code);
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
