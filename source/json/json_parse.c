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


static tJsonParseState JsonParseUtf16Escape(tJsonParse *Parse, tJsonCharacter Character)
{
	if (Character == '\\')
	{
		return json_ParseUtf16;
	}

	return json_ParseError;
}


static tJsonParseState JsonParseUtf16(tJsonParse *Parse, tJsonCharacter Character)
{
	if (Character == 'u')
	{
		return json_ParseUtf16Digit;
	}

	return json_ParseError;
}


static tJsonParseState JsonParseUtf16Digit(tJsonParse *Parse, tJsonCharacter Character)
{
	size_t Index = Parse->Utf16Length / 2;

	if (JsonCharacterIsHexDigit(Character) && (Index < sizeof(Parse->Utf16)))
	{
		if (Parse->Utf16Length & 0x01)
		{
			Parse->Utf16[Index] = Parse->Utf16[Index] + JsonCharacterToHexDigit(Character);
		}
		else
		{
			Parse->Utf16[Index] = JsonCharacterToHexDigit(Character) << 4;
		}

		Parse->Utf16Length++;
		if (Parse->Utf16Length != sizeof(tJsonUtf16Unit) * 2)
		{
			if (Parse->Utf16Length != sizeof(tJsonUtf16) * 2)
			{
				return json_ParseUtf16Digit;
			}
		}
		else if (JsonUtf16beIsHighSurrogate(Parse->Utf16))
		{
			return json_ParseUtf16Escape;
		}

		if (JsonUtf16beDecodeNext(Parse->Utf16, Parse->Utf16Length, 0, &Character) && JsonStringAddCharacter(&Parse->Element->Name, Character))
		{
			return JsonParseSetEscapeState(Parse, json_ParseError);
		}
	}

	return json_ParseError;
}


static tJsonParseState JsonParseEscape(tJsonParse *Parse, tJsonCharacter Character)
{
	Character = JsonCharacterFromEscape(Character);

	if (Character == 'u')
	{
		Parse->Utf16Length = 0;
		return json_ParseUtf16Digit;
	}
	else if (JsonStringAddCharacter(&Parse->Element->Name, Character))
	{
		return JsonParseSetEscapeState(Parse, json_ParseError);
	}

	return json_ParseError;
}


static tJsonParseState JsonParseKey(tJsonParse *Parse, tJsonCharacter Character)
{
	if (Character == '\\')
	{
		JsonParseSetEscapeState(Parse, Parse->State);
		return json_ParseEscape;
	}
	else if (Character == '"')
	{
		Parse->AllocateChild = true;
		return json_ParseKeyEnd;
	}
	else if (JsonStringAddCharacter(&Parse->Element->Name, Character))
	{
		return json_ParseKey;
	}

	return json_ParseError;
}


static tJsonParseState JsonParseKeyStart(tJsonParse *Parse, tJsonCharacter Character)
{
	if (Character == '"')
	{
		if (JsonParseAllocateElement(Parse, json_TypeKey))
		{
			return json_ParseKey;
		}
	}
	else if (Character == '}')
	{
		if (Parse->Element->Type == json_TypeComment)
		{
			if (Parse->Element->Parent == NULL)
			{
				return json_ParseError;
			}
			Parse->Element = Parse->Element->Parent;
		}
		if (Parse->Element->Type == json_TypeObject)
		{
			return json_ParseValueEnd;
		}
	}
	else if (Character == '/')
	{
		JsonParseSetCommentState(Parse, Parse->State);
		return json_ParseCommentStart;
	}
	else if (JsonCharacterIsWhitespace(Character))
	{
		return json_ParseKeyStart;
	}

	return json_ParseError;
}


static tJsonParseState JsonParseKeyEnd(tJsonParse *Parse, tJsonCharacter Character)
{
	if (Character == ':')
	{
		return json_ParseValueStart;
	}
	else if (Character == '/')
	{
		JsonParseSetCommentState(Parse, Parse->State);
		return json_ParseCommentStart;
	}
	else if (JsonCharacterIsWhitespace(Character))
	{
		return json_ParseKeyEnd;
	}

	return json_ParseError;
}


static tJsonParseState JsonParseValueString(tJsonParse *Parse, tJsonCharacter Character)
{
	if (Character == '\\')
	{
		JsonParseSetEscapeState(Parse, Parse->State);
		return json_ParseEscape;
	}
	else if (Character == '"')
	{
		return json_ParseValueEnd;
	}	
	else if (JsonStringAddCharacter(&Parse->Element->Name, Character))
	{
		return json_ParseValueString;
	}

	return json_ParseError;
}


static tJsonParseState JsonParseValueEnd(tJsonParse *Parse, tJsonCharacter Character)
{
	Parse->AllocateChild = false;
	if (Character == ',')
	{
		if (Parse->Element->Parent != NULL)
		{
			if (Parse->Element->Parent->Type == json_TypeKey)
			{
				Parse->Element = Parse->Element->Parent;
				return json_ParseKeyStart;
			}
			else if (Parse->Element->Parent->Type == json_TypeArray)
			{
				return json_ParseValueStart;
			}
		}
	}
	else if (Character == '}')
	{
		if ((Parse->Element->Parent != NULL) && (Parse->Element->Parent->Type == json_TypeKey))
		{
			Parse->Element = Parse->Element->Parent;
			if ((Parse->Element->Parent != NULL) && (Parse->Element->Parent->Type == json_TypeObject))
			{
				Parse->Element = Parse->Element->Parent;
				return json_ParseValueEnd;
			}
		}
	}
	else if (Character == ']')
	{
		if ((Parse->Element->Parent != NULL) && (Parse->Element->Parent->Type == json_TypeArray))
		{
			Parse->Element = Parse->Element->Parent;
			return json_ParseValueEnd;
		}
	}
	else if (Character == '/')
	{
		JsonParseSetCommentState(Parse, Parse->State);
		return json_ParseCommentStart;
	}
	else if (Character == '\0')
	{
		if ((Parse->Element->Parent != NULL) && (Parse->Element->Parent->Type == json_TypeRoot))
		{
			Parse->Element = Parse->Element->Parent;
			return json_ParseComplete;
		}
	}
	else if (JsonCharacterIsWhitespace(Character))
	{
		return json_ParseValueEnd;
	}

	return json_ParseError;
}


static tJsonParseState JsonParseValueLiteral(tJsonParse *Parse, tJsonCharacter Character)
{
	if (!JsonCharacterIsLiteral(Character))
	{
		return JsonParseValueEnd(Parse, Character);
	}
	else if (JsonStringAddCharacter(&Parse->Element->Name, Character))
	{
		return json_ParseValueLiteral;
	}

	return json_ParseError;
}


static tJsonParseState JsonParseValueStart(tJsonParse *Parse, tJsonCharacter Character)
{
	if (Character == '{')
	{
		if (JsonParseAllocateElement(Parse, json_TypeObject))
		{
			Parse->AllocateChild = true;
			return json_ParseKeyStart;
		}
	}
	else if (Character == '[')
	{
		if (JsonParseAllocateElement(Parse, json_TypeArray))
		{
			Parse->AllocateChild = true;
			return json_ParseValueStart;
		}
	}
	else if (Character == ']')
	{
		if (Parse->Element->Type == json_TypeArray)
		{
			return json_ParseValueEnd;
		}
	}
	else if (Character == '"')
	{
		if (JsonParseAllocateElement(Parse, json_TypeValueString))
		{
			return json_ParseValueString;
		}
	}
	else if (Character == '/')
	{
		JsonParseSetCommentState(Parse, Parse->State);
		return json_ParseCommentStart;
	}
	else if (Character == '\0')
	{
		if (Parse->Element->Type == json_TypeRoot)
		{
			return json_ParseComplete;
		}
	}
	else if (JsonCharacterIsLiteral(Character))
	{
		if (JsonParseAllocateElement(Parse, json_TypeValueLiteral) && JsonStringAddCharacter(&Parse->Element->Name, Character))
		{
			return json_ParseValueLiteral;
		}
	}
	else if (JsonCharacterIsWhitespace(Character))
	{
		return json_ParseValueStart;
	}

	return json_ParseError;
}


static tJsonParseState JsonParseCommentStart(tJsonParse *Parse, tJsonCharacter Character)
{
	if ((Character == '/') || (Character == '*'))
	{
		if (Parse->StripComments || JsonParseAllocateElement(Parse, json_TypeComment))
		{
			return (Character == '/') ? json_ParseCommentLine : json_ParseCommentBlock;
		}
	}
	
	return json_ParseError;
}


static tJsonParseState JsonParseCommentLine(tJsonParse *Parse, tJsonCharacter Character)
{
	if ((Character == '\r') || (Character == '\n'))
	{
		if (!Parse->StripComments)
		{
			Parse->AllocateChild = false;
		}
		return JsonParseSetCommentState(Parse, json_ParseError);
	}
	else if (Parse->StripComments || JsonStringAddCharacter(&Parse->Element->Name, Character))
	{
		return json_ParseCommentLine;
	}

	return json_ParseError;
}


static tJsonParseState JsonParseCommentBlock(tJsonParse *Parse, tJsonCharacter Character)
{
	if (Character == '*')
	{
		return json_ParseCommentBlockEnd;
	}
	else if ((Character == '\r') || (Character == '\n'))
	{
		return json_ParseCommentBlockLine;
	}
	else if (Parse->StripComments || JsonStringAddCharacter(&Parse->Element->Name, Character))
	{
		return json_ParseCommentBlock;
	}

	return json_ParseError;
}


static tJsonParseState JsonParseCommentBlockLine(tJsonParse *Parse, tJsonCharacter Character)
{
	if (JsonCharacterIsWhitespace(Character))
	{
		return json_ParseCommentBlockLine;
	}
	else if (!Parse->StripComments)
	{
		Parse->AllocateChild = false;
		if (!JsonParseAllocateElement(Parse, json_TypeComment))
		{
			return json_ParseError;
		}
	}

	return JsonParseCommentBlock(Parse, Character);
}


static tJsonParseState JsonParseCommentBlockEnd(tJsonParse *Parse, tJsonCharacter Character)
{
	if (Character == '/')
	{
		if (!Parse->StripComments)
		{
			Parse->AllocateChild = false;
		}
		return JsonParseSetCommentState(Parse, json_ParseError);
	}
	
	if (Parse->StripComments || JsonStringAddCharacter(&Parse->Element->Name, '*'))
	{
		return JsonParseCommentBlock(Parse, Character);
	}

	return json_ParseError;
}


void JsonParseSetUp(tJsonParse *Parse, bool StripComments, tJsonElement *RootElement)
{
	JsonElementClear(RootElement);
	Parse->State = json_ParseValueStart;
	Parse->Element = RootElement;
	Parse->Utf16Length = 0;
	Parse->CommentState = json_ParseError;
	Parse->EscapeState = json_ParseError;
	Parse->AllocateChild = true;
	Parse->StripComments = StripComments;
}


void JsonParseCleanUp(tJsonParse *Parse)
{
	Parse->State = json_ParseComplete;
	Parse->Element = NULL;
	Parse->Utf16Length = 0;
	Parse->CommentState = json_ParseError;
	Parse->EscapeState = json_ParseError;
	Parse->AllocateChild = false;
	Parse->StripComments = false;
}


int JsonParseCharacter(tJsonParse *Parse, tJsonCharacter Character)
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
				case json_ParseEscape:
					Parse->State = JsonParseEscape(Parse, Character);
				break;

				case json_ParseUtf16Escape:
					Parse->State = JsonParseUtf16Escape(Parse, Character);
				break;

				case json_ParseUtf16:
					Parse->State = JsonParseUtf16(Parse, Character);
				break;

				case json_ParseUtf16Digit:
					Parse->State = JsonParseUtf16Digit(Parse, Character);
				break;

				case json_ParseKeyStart:
					Parse->State = JsonParseKeyStart(Parse, Character);
				break;

				case json_ParseKey:
					Parse->State = JsonParseKey(Parse, Character);
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


int JsonParse(tJsonParse *Parse, tJsonUtfType UtfType, const uint8_t *Content, size_t Size, size_t *Offset)
{
	tJsonCharacter Character;
	size_t DiscardOffset = 0;
	size_t DecodeLength;
	int State = JSON_PARSE_INCOMPLETE;

	if (Offset == NULL)
	{
		Offset = &DiscardOffset;
	}

	for (; (State == JSON_PARSE_INCOMPLETE) && (*Offset < Size); *Offset = *Offset + DecodeLength)
	{
		DecodeLength = JsonUtfDecode(UtfType, Content, Size, *Offset, &Character);
		if (DecodeLength == 0)
		{
			break;
		}

		State = JsonParseCharacter(Parse, Character);
	}

	return State;
}
