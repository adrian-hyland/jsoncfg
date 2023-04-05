#include "json_character.h"
#include "json_format.h"


#define JSON_FORMAT_INDENT_SPACE_COUNT 3


static tJsonFormatState JsonFormatSetEscapeState(tJsonFormat *Format, tJsonFormatState State)
{
	tJsonFormatState PreviousState;

	PreviousState = Format->EscapeState;
	Format->EscapeState = State;

	return PreviousState;
}


static tJsonFormatState JsonFormatValueStart(tJsonFormat *Format, tJsonCharacter *Character);
static tJsonFormatState JsonFormatValueEnd(tJsonFormat *Format, tJsonCharacter *Character);


static tJsonFormatState JsonFormatUtf16Escape(tJsonFormat *Format, tJsonCharacter *Character)
{
	*Character = '\\';
	return json_FormatUtf16;
}


static tJsonFormatState JsonFormatUtf16(tJsonFormat *Format, tJsonCharacter *Character)
{
	*Character = 'u';
	return json_FormatUtf16Digit;
}


static tJsonFormatState JsonFormatUtf16Digit(tJsonFormat *Format, tJsonCharacter *Character)
{
	size_t Index = Format->Utf16Index / 2;

	if (Format->Utf16Index & 0x01)
	{
		*Character = JsonCharacterFromHexDigit(Format->Utf16[Index] & 0x0F);
	}
	else
	{
		*Character = JsonCharacterFromHexDigit(Format->Utf16[Index] >> 4);
	}

	Format->Utf16Index++;
	if (Format->Utf16Index == Format->Utf16Length * 2)
	{
		return JsonFormatSetEscapeState(Format, json_FormatError);
	}
	else if (Format->Utf16Index == sizeof(tJsonUtf16Unit) * 2)
	{
		return json_FormatUtf16Escape;
	}
	else
	{
		return json_FormatUtf16Digit;
	}
}


static tJsonFormatState JsonFormatEscape(tJsonFormat *Format, tJsonCharacter *Character)
{
	size_t Length;

	Length = JsonStringGetNextCharacter(&Format->Element->Name, Format->NameIndex, Character);
	if (Length == 0)
	{
		return json_FormatError;
	}
	*Character = JsonCharacterToEscape(*Character);
	Format->NameIndex = Format->NameIndex + Length;

	return JsonFormatSetEscapeState(Format, json_FormatError);
}


static tJsonFormatState JsonFormatKey(tJsonFormat *Format, tJsonCharacter *Character)
{
	size_t Length;

	if (Format->NameIndex < Format->Element->Name.Length)
	{
		Length = JsonStringGetNextCharacter(&Format->Element->Name, Format->NameIndex, Character);
		if (Length == 0)
		{
			return json_FormatError;
		}
		else if (JsonCharacterIsEscapable(*Character))
		{
			*Character = '\\';
			JsonFormatSetEscapeState(Format, Format->State);
			return json_FormatEscape;
		}
		else if (JsonCharacterIsControl(*Character))
		{
			Format->NameIndex = Format->NameIndex + Length;
			Format->Utf16Index = 0;
			Format->Utf16Length = JsonUtf16beEncode(Format->Utf16, sizeof(Format->Utf16), 0, *Character);
			if (Format->Utf16Length == 0)
			{
				return json_FormatError;
			}
			*Character = '\\';
			JsonFormatSetEscapeState(Format, Format->State);
			return json_FormatUtf16;
		}
		Format->NameIndex = Format->NameIndex + Length;
		return json_FormatKey;
	}
	else
	{
		*Character = '"';
		return json_FormatKeyEnd;
	}
}


static tJsonFormatState JsonFormatKeyEnd(tJsonFormat *Format, tJsonCharacter *Character)
{
	tJsonElement *Child;

	Child = JsonElementGetChild(Format->Element, Format->CommentType == json_CommentNone);
	if (Child == NULL)
	{
		return json_FormatError;
	}
	
	Format->Element = Child;
	if (Format->Element->Type == json_TypeComment)
	{
		Format->NewLine = true;
	}
	else if (Format->Type != json_FormatCompress)
	{
		Format->SpaceCount = 1;
	}

	*Character = ':';
	return json_FormatValueStart;
}


static tJsonFormatState JsonFormatValueNext(tJsonFormat *Format, tJsonCharacter *Character)
{
	tJsonType CurrentType;
	tJsonElement *NextElement;

	CurrentType = Format->Element->Type;

	if (Format->Element->Parent == NULL)
	{
		return json_FormatError;
	}

	NextElement = JsonElementGetNext(Format->Element, Format->CommentType == json_CommentNone);
	if (NextElement != NULL)
	{
		if (Format->Type == json_FormatIndent)
		{
			Format->NewLine = true;
		}
		else if (Format->Type == json_FormatSpace)
		{
			Format->SpaceCount = 1;
		}

		if ((CurrentType != json_TypeComment) && (CurrentType != json_TypeKey) && (JsonElementGetNext((Format->Element->Parent->Type == json_TypeKey) ? Format->Element->Parent : Format->Element, true) != NULL))
		{
			Format->Element = NextElement;
			*Character = ',';
			return json_FormatValueStart;
		}
		else
		{
			Format->Element = NextElement;
			return JsonFormatValueStart(Format, Character);
		}
	}
	else
	{
		Format->Element = Format->Element->Parent;
		if (Format->Element->Type == json_TypeRoot)
		{
			*Character = '\0';
			return json_FormatComplete;
		}
		else if ((Format->Element->Type == json_TypeObject) || (Format->Element->Type == json_TypeArray))
		{
			if (Format->Type == json_FormatIndent)
			{
				Format->Indent--;
				Format->NewLine = true;
			}
			else if (Format->Type == json_FormatSpace)
			{
				Format->SpaceCount = 1;
			}
			return JsonFormatValueEnd(Format, Character);
		}
		else if (Format->Element->Type == json_TypeKey)
		{
			if ((CurrentType != json_TypeComment) && (JsonElementGetNext(Format->Element, true) != NULL))
			{
				*Character = ',';
				return json_FormatValueNext;
			}
			else
			{
				return JsonFormatValueNext(Format, Character);
			}
		}
		else
		{
			return json_FormatError;
		}
	}
}


static tJsonFormatState JsonFormatValueEnd(tJsonFormat *Format, tJsonCharacter *Character)
{
	if (Format->NewLine)
	{
		*Character = '\n';
		Format->NewLine = false;
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

	return json_FormatError;
}


static tJsonFormatState JsonFormatValueString(tJsonFormat *Format, tJsonCharacter *Character)
{
	size_t Length;

	if (Format->NameIndex < Format->Element->Name.Length)
	{
		Length = JsonStringGetNextCharacter(&Format->Element->Name, Format->NameIndex, Character);
		if (Length == 0)
		{
			return json_FormatError;
		}
		else if (JsonCharacterIsEscapable(*Character))
		{
			*Character = '\\';
			JsonFormatSetEscapeState(Format, Format->State);
			return json_FormatEscape;
		}
		else if (JsonCharacterIsControl(*Character))
		{
			Format->NameIndex = Format->NameIndex + Length;
			Format->Utf16Index = 0;
			Format->Utf16Length = JsonUtf16beEncode(Format->Utf16, sizeof(Format->Utf16), 0, *Character);
			if (Format->Utf16Length == 0)
			{
				return json_FormatError;
			}
			*Character = '\\';
			JsonFormatSetEscapeState(Format, Format->State);
			return json_FormatUtf16;
		}
		Format->NameIndex = Format->NameIndex + Length;
		return json_FormatValueString;
	}
	else
	{
		*Character = '"';
		return json_FormatValueNext;
	}
}


static tJsonFormatState JsonFormatValueLiteral(tJsonFormat *Format, tJsonCharacter *Character)
{
	size_t Length;

	if (Format->NameIndex < Format->Element->Name.Length)
	{
		Length = JsonStringGetNextCharacter(&Format->Element->Name, Format->NameIndex, Character);
		if (Length == 0)
		{
			return json_FormatError;
		}
		Format->NameIndex = Format->NameIndex + Length;
		return json_FormatValueLiteral;
	}
	else
	{
		return JsonFormatValueNext(Format, Character);
	}
}


static tJsonFormatState JsonFormatValueStart(tJsonFormat *Format, tJsonCharacter *Character)
{
	tJsonElement *Child;

	if (Format->NewLine)
	{
		*Character = '\n';
		Format->NewLine = false;
		Format->SpaceCount = Format->Indent * Format->IndentSize;
		return json_FormatValueStart;
	}
	else if (Format->SpaceCount != 0)
	{
		*Character = ' ';
		Format->SpaceCount--;
		return json_FormatValueStart;
	}
	else if ((Format->Element->Type == json_TypeObject) || (Format->Element->Type == json_TypeArray))
	{
		*Character = (Format->Element->Type == json_TypeObject) ? '{' : '[';
		Child = JsonElementGetChild(Format->Element, Format->CommentType == json_CommentNone);
		if (Child == NULL)
		{
			return json_FormatValueEnd;
		}
		else
		{
			Format->Element = Child;
			if (Format->Type == json_FormatIndent)
			{
				Format->NewLine = true;
				Format->Indent++;
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
	else if (Format->Element->Type == json_TypeComment)
	{
		*Character = '/';
		Format->NameIndex = 0;
		return json_FormatCommentStart;
	}

	return json_FormatError;
}


static tJsonFormatState JsonFormatCommentStart(tJsonFormat *Format, tJsonCharacter *Character)
{
	*Character = (Format->CommentType == json_CommentLine) ? '/' : '*';
	Format->SpaceCount = 0;
	return json_FormatComment;
}


static tJsonFormatState JsonFormatComment(tJsonFormat *Format, tJsonCharacter *Character)
{
	tJsonElement *NextElement;
	tJsonCharacter NextCharacter;
	size_t Length;

	if (Format->SpaceCount > 0)
	{
		*Character = ' ';
		Format->SpaceCount--;
		return json_FormatComment;
	}
	else if (Format->NameIndex < Format->Element->Name.Length)
	{
		Length = JsonStringGetNextCharacter(&Format->Element->Name, Format->NameIndex, Character);
		if (Length == 0)
		{
			return json_FormatError;
		}
		Format->NameIndex = Format->NameIndex + Length;
		if (Format->NameIndex == Format->Element->Name.Length)
		{
			if (Format->CommentType == json_CommentBlock)
			{
				NextElement = JsonElementGetNext(Format->Element, false);
				if ((NextElement == NULL) || (NextElement->Type != json_TypeComment))
				{
					if ((*Character != '*') && !JsonCharacterIsWhitespace(*Character))
					{
						Format->SpaceCount = 1;
					}
				}
			}
		}
		return json_FormatComment;
	}
	else if (Format->CommentType == json_CommentBlock)
	{
		NextElement = JsonElementGetNext(Format->Element, false);
		if ((NextElement != NULL) && (NextElement->Type == json_TypeComment))
		{
			*Character = '\n';
			Format->Element = NextElement;
			Format->NameIndex = 0;
			Format->SpaceCount = Format->Indent * Format->IndentSize + 1;
			if ((JsonStringGetNextCharacter(&NextElement->Name, 0, &NextCharacter) != 0) && (NextCharacter != '*'))
			{
				Format->SpaceCount++;
			}
			return json_FormatComment;
		}
		else
		{
			*Character = '*';
			return json_FormatCommentBlockEnd;
		}
	}
	else
	{
		return JsonFormatValueNext(Format, Character);
	}
}


static tJsonFormatState JsonFormatCommentBlockEnd(tJsonFormat *Format, tJsonCharacter *Character)
{
	*Character = '/';
	return json_FormatValueNext;
}


static void JsonFormatSetUp(tJsonFormat *Format, tJsonFormatType Type, size_t IndentSize, tJsonCommentType CommentType, tJsonElement *RootElement)
{
	Format->Type = Type;
	Format->State = json_FormatValueStart;
	Format->Element = JsonElementGetChild(RootElement, CommentType == json_CommentNone);
	Format->CommentType = CommentType;
	Format->EscapeState = json_FormatError;
	Format->Utf16Index = 0;
	Format->Utf16Length = 0;
	Format->NameIndex = 0;
	Format->Indent = 0;
	Format->IndentSize = IndentSize;
	Format->SpaceCount = 0;
	Format->NewLine = false;
}


void JsonFormatSetUpCompress(tJsonFormat *Format, tJsonElement *RootElement)
{
	JsonFormatSetUp(Format, json_FormatCompress, 0, json_CommentNone, RootElement);
}


void JsonFormatSetUpSpace(tJsonFormat *Format, tJsonElement *RootElement)
{
	JsonFormatSetUp(Format, json_FormatSpace, 0, json_CommentNone, RootElement);
}


void JsonFormatSetUpIndent(tJsonFormat *Format, size_t IndentSize, tJsonCommentType CommentType, tJsonElement *RootElement)
{
	JsonFormatSetUp(Format, json_FormatIndent, (IndentSize == 0) ? JSON_FORMAT_INDENT_SPACE_COUNT : IndentSize, CommentType, RootElement);
}


void JsonFormatCleanUp(tJsonFormat *Format)
{
	Format->Type = json_FormatCompress;
	Format->State = json_FormatComplete;
	Format->Element = NULL;
	Format->CommentType = json_CommentNone;
	Format->EscapeState = json_FormatError;
	Format->Utf16Index = 0;
	Format->Utf16Length = 0;
	Format->NameIndex = 0;
	Format->Indent = 0;
	Format->IndentSize = 0;
	Format->SpaceCount = 0;
	Format->NewLine = false;
}


int JsonFormatCharacter(tJsonFormat *Format, tJsonCharacter *Character)
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
				case json_FormatEscape:
					Format->State = JsonFormatEscape(Format, Character);
				break;

				case json_FormatUtf16Escape:
					Format->State = JsonFormatUtf16Escape(Format, Character);
				break;

				case json_FormatUtf16:
					Format->State = JsonFormatUtf16(Format, Character);
				break;

				case json_FormatUtf16Digit:
					Format->State = JsonFormatUtf16Digit(Format, Character);
				break;

				case json_FormatKey:
					Format->State = JsonFormatKey(Format, Character);
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

				case json_FormatValueLiteral:
					Format->State = JsonFormatValueLiteral(Format, Character);
				break;

				case json_FormatValueNext:
					Format->State = JsonFormatValueNext(Format, Character);
				break;

				case json_FormatValueEnd:
					Format->State = JsonFormatValueEnd(Format, Character);
				break;

				case json_FormatCommentStart:
					Format->State = JsonFormatCommentStart(Format, Character);
				break;

				case json_FormatComment:
					Format->State = JsonFormatComment(Format, Character);
				break;

				case json_FormatCommentBlockEnd:
					Format->State = JsonFormatCommentBlockEnd(Format, Character);
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


int JsonFormat(tJsonFormat *Format, tJsonUtfType UtfType, uint8_t *Content, size_t Size, size_t *Offset)
{
	tJsonCharacter Character;
	size_t DiscardOffset = 0;
	size_t EncodeLength;
	int State= JSON_FORMAT_INCOMPLETE;

	if (Size < JSON_FORMAT_MIN_SIZE)
	{
		return JSON_FORMAT_ERROR;
	}

	if (Offset == NULL)
	{
		Offset = &DiscardOffset;
	}

	for (; *Offset <= Size - JSON_FORMAT_MIN_SIZE; *Offset = *Offset + EncodeLength)
	{
		State = JsonFormatCharacter(Format, &Character);
		if (State != JSON_FORMAT_INCOMPLETE)
		{
			break;
		}

		EncodeLength = JsonUtfEncode(UtfType, Content, Size, *Offset, Character);
		if (EncodeLength == 0)
		{
			break;
		}
	}

	return State;
}
