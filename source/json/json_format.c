#include "json_character.h"
#include "json_format.h"


#define JSON_FORMAT_INDENT_SPACE_COUNT 3


static bool JsonFormatUtf8IsComplete(tJsonFormat *Format, tJsonUtf8Unit *CodeUnit)
{
	if (Format->Utf8CodeIndex == 0)
	{
		return true;
	}

	*CodeUnit = JsonUtf8CodeGetUnit(Format->Utf8Code, Format->Utf8CodeIndex);
	Format->Utf8CodeIndex++;
	if (Format->Utf8CodeIndex >= JsonUtf8CodeGetUnitLength(Format->Utf8Code))
	{
		Format->Utf8CodeIndex = 0;
	}
	
	return false;
}


static void JsonFormatUtf8(tJsonFormat *Format, tJsonUtf8Code Code, tJsonUtf8Unit *CodeUnit)
{
	Format->Utf8Code = Code;
	Format->Utf8CodeIndex = 0;
	*CodeUnit = JsonUtf8CodeGetUnit(Format->Utf8Code, Format->Utf8CodeIndex);
	Format->Utf8CodeIndex++;
	if (Format->Utf8CodeIndex >= JsonUtf8CodeGetUnitLength(Format->Utf8Code))
	{
		Format->Utf8CodeIndex = 0;
	}
}


static tJsonFormatState JsonFormatValueStart(tJsonFormat *Format, tJsonUtf8Code *Code);
static tJsonFormatState JsonFormatValueEnd(tJsonFormat *Format, tJsonUtf8Code *Code);


static tJsonFormatState JsonFormatKeyUtf16Escape(tJsonFormat *Format, tJsonUtf8Code *Code)
{
	*Code = '\\';
	return json_FormatKeyUtf16;
}


static tJsonFormatState JsonFormatKeyUtf16(tJsonFormat *Format, tJsonUtf8Code *Code)
{
	*Code = 'u';
	return json_FormatKeyUtf16Digit1;
}


static tJsonFormatState JsonFormatKeyUtf16Digit1(tJsonFormat *Format, tJsonUtf8Code *Code)
{
	*Code = JsonCharacterFromHexDigit(JsonUtf16CodeGetNibble(Format->Utf16Code, 0));
	return json_FormatKeyUtf16Digit2;
}


static tJsonFormatState JsonFormatKeyUtf16Digit2(tJsonFormat *Format, tJsonUtf8Code *Code)
{
	*Code = JsonCharacterFromHexDigit(JsonUtf16CodeGetNibble(Format->Utf16Code, 1));
	return json_FormatKeyUtf16Digit3;
}


static tJsonFormatState JsonFormatKeyUtf16Digit3(tJsonFormat *Format, tJsonUtf8Code *Code)
{
	*Code = JsonCharacterFromHexDigit(JsonUtf16CodeGetNibble(Format->Utf16Code, 2));
	return json_FormatKeyUtf16Digit4;
}


static tJsonFormatState JsonFormatKeyUtf16Digit4(tJsonFormat *Format, tJsonUtf8Code *Code)
{
	*Code = JsonCharacterFromHexDigit(JsonUtf16CodeGetNibble(Format->Utf16Code, 3));
	Format->Utf16CodeIndex++;
	if (Format->Utf16CodeIndex < JsonUtf16CodeGetUnitLength(Format->Utf16Code))
	{
		return json_FormatKeyUtf16Escape;
	}

	return json_FormatKey;
}


static tJsonFormatState JsonFormatKeyEscape(tJsonFormat *Format, tJsonUtf8Code *Code)
{
	size_t CodeLength;

	CodeLength = JsonStringGetNextUtf8Code(&Format->Element->Name, Format->NameIndex, Code);
	if (CodeLength == 0)
	{
		return json_FormatError;
	}
	*Code = JsonCharacterToEscape(*Code);
	Format->NameIndex = Format->NameIndex + CodeLength;
	return json_FormatKey;
}


static tJsonFormatState JsonFormatKey(tJsonFormat *Format, tJsonUtf8Code *Code)
{
	size_t CodeLength;

	if (Format->NameIndex < Format->Element->Name.Length)
	{
		CodeLength = JsonStringGetNextUtf8Code(&Format->Element->Name, Format->NameIndex, Code);
		if (CodeLength == 0)
		{
			return json_FormatError;
		}
		else if (JsonCharacterIsEscapable(*Code))
		{
			*Code = '\\';
			return json_FormatKeyEscape;
		}
		else if (JsonCharacterIsControl(*Code))
		{
			Format->NameIndex = Format->NameIndex + CodeLength;
			Format->Utf16Code = JsonUtf16Code(JsonUtf8CodeGetCharacter(*Code));
			Format->Utf16CodeIndex = 0;
			*Code = '\\';
			return json_FormatKeyUtf16;
		}
		Format->NameIndex = Format->NameIndex + CodeLength;
		return json_FormatKey;
	}
	else
	{
		*Code = '"';
		return json_FormatKeyEnd;
	}
}


static tJsonFormatState JsonFormatKeyEnd(tJsonFormat *Format, tJsonUtf8Code *Code)
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

	*Code = ':';
	return json_FormatValueStart;
}


static tJsonFormatState JsonFormatValueNext(tJsonFormat *Format, tJsonUtf8Code *Code)
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

		if ((CurrentType != json_TypeComment) && (CurrentType != json_TypeKey) && (JsonElementGetNext(Format->Element->Parent->Type == json_TypeKey ? Format->Element->Parent : Format->Element, true) != NULL))
		{
			Format->Element = NextElement;
			*Code = ',';
			return json_FormatValueStart;
		}
		else
		{
			Format->Element = NextElement;
			return JsonFormatValueStart(Format, Code);
		}
	}
	else
	{
		Format->Element = Format->Element->Parent;
		if (Format->Element->Type == json_TypeRoot)
		{
			*Code = '\0';
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
			return JsonFormatValueEnd(Format, Code);
		}
		else if (Format->Element->Type == json_TypeKey)
		{
			if ((CurrentType != json_TypeComment) && (JsonElementGetNext(Format->Element, true) != NULL))
			{
				*Code = ',';
				return json_FormatValueNext;
			}
			else
			{
				return JsonFormatValueNext(Format, Code);
			}
		}
		else
		{
			return json_FormatError;
		}
	}
}


static tJsonFormatState JsonFormatValueEnd(tJsonFormat *Format, tJsonUtf8Code *Code)
{
	if (Format->NewLine)
	{
		*Code = '\n';
		Format->NewLine = false;
		Format->SpaceCount = Format->Indent * Format->IndentSize;
		return json_FormatValueEnd;
	}
	else if (Format->SpaceCount != 0)
	{
		*Code = ' ';
		Format->SpaceCount--;
		return json_FormatValueEnd;
	}
	else if (Format->Element->Type == json_TypeObject)
	{
		*Code = '}';
		return json_FormatValueNext;
	}
	else if (Format->Element->Type == json_TypeArray)
	{
		*Code = ']';
		return json_FormatValueNext;        
	}
	else if (Format->Element->Type == json_TypeKey)
	{
		return JsonFormatValueNext(Format, Code);
	}
	else if (Format->Element->Type == json_TypeComment)
	{
		*Code = '/';
		Format->NameIndex = 0;
		return json_FormatCommentStart;
	}

	return json_FormatError;
}


static tJsonFormatState JsonFormatValueStringUtf16Escape(tJsonFormat *Format, tJsonUtf8Code *Code)
{
	*Code = '\\';
	return json_FormatValueStringUtf16;
}


static tJsonFormatState JsonFormatValueStringUtf16(tJsonFormat *Format, tJsonUtf8Code *Code)
{
	*Code = 'u';
	return json_FormatValueStringUtf16Digit1;
}


static tJsonFormatState JsonFormatValueStringUtf16Digit1(tJsonFormat *Format, tJsonUtf8Code *Code)
{
	*Code = JsonCharacterFromHexDigit(JsonUtf16CodeGetNibble(Format->Utf16Code, 0));
	return json_FormatValueStringUtf16Digit2;
}


static tJsonFormatState JsonFormatValueStringUtf16Digit2(tJsonFormat *Format, tJsonUtf8Code *Code)
{
	*Code = JsonCharacterFromHexDigit(JsonUtf16CodeGetNibble(Format->Utf16Code, 1));
	return json_FormatValueStringUtf16Digit3;
}


static tJsonFormatState JsonFormatValueStringUtf16Digit3(tJsonFormat *Format, tJsonUtf8Code *Code)
{
	*Code = JsonCharacterFromHexDigit(JsonUtf16CodeGetNibble(Format->Utf16Code, 2));
	return json_FormatValueStringUtf16Digit4;
}


static tJsonFormatState JsonFormatValueStringUtf16Digit4(tJsonFormat *Format, tJsonUtf8Code *Code)
{
	*Code = JsonCharacterFromHexDigit(JsonUtf16CodeGetNibble(Format->Utf16Code, 3));
	Format->Utf16CodeIndex++;
	if (Format->Utf16CodeIndex < JsonUtf16CodeGetUnitLength(Format->Utf16Code))
	{
		return json_FormatValueStringUtf16Escape;
	}

	return json_FormatValueString;
}


static tJsonFormatState JsonFormatValueStringEscape(tJsonFormat *Format, tJsonUtf8Code *Code)
{
	size_t CodeLength;

	CodeLength = JsonStringGetNextUtf8Code(&Format->Element->Name, Format->NameIndex, Code);
	if (CodeLength == 0)
	{
		return json_FormatError;
	}
	*Code = JsonCharacterToEscape(*Code);
	Format->NameIndex = Format->NameIndex + CodeLength;
	return json_FormatValueString;
}


static tJsonFormatState JsonFormatValueString(tJsonFormat *Format, tJsonUtf8Code *Code)
{
	size_t CodeLength;

	if (Format->NameIndex < Format->Element->Name.Length)
	{
		CodeLength = JsonStringGetNextUtf8Code(&Format->Element->Name, Format->NameIndex, Code);
		if (CodeLength == 0)
		{
			return json_FormatError;
		}
		else if (JsonCharacterIsEscapable(*Code))
		{
			*Code = '\\';
			return json_FormatValueStringEscape;
		}
		else if (JsonCharacterIsControl(*Code))
		{
			Format->NameIndex = Format->NameIndex + CodeLength;
			Format->Utf16Code = JsonUtf16Code(JsonUtf8CodeGetCharacter(*Code));
			Format->Utf16CodeIndex = 0;
			*Code = '\\';
			return json_FormatValueStringUtf16;
		}
		Format->NameIndex = Format->NameIndex + CodeLength;
		return json_FormatValueString;
	}
	else
	{
		*Code = '"';
		return json_FormatValueNext;
	}
}


static tJsonFormatState JsonFormatValueLiteral(tJsonFormat *Format, tJsonUtf8Code *Code)
{
	size_t CodeLength;

	if (Format->NameIndex < Format->Element->Name.Length)
	{
		CodeLength = JsonStringGetNextUtf8Code(&Format->Element->Name, Format->NameIndex, Code);
		if (CodeLength == 0)
		{
			return json_FormatError;
		}
		Format->NameIndex = Format->NameIndex + CodeLength;
		return json_FormatValueLiteral;
	}
	else
	{
		return JsonFormatValueNext(Format, Code);
	}
}


static tJsonFormatState JsonFormatValueStart(tJsonFormat *Format, tJsonUtf8Code *Code)
{
	tJsonElement *Child;

	if (Format->NewLine)
	{
		*Code = '\n';
		Format->NewLine = false;
		Format->SpaceCount = Format->Indent * Format->IndentSize;
		return json_FormatValueStart;
	}
	else if (Format->SpaceCount != 0)
	{
		*Code = ' ';
		Format->SpaceCount--;
		return json_FormatValueStart;
	}
	else if (Format->Element->Type == json_TypeObject)
	{
		*Code = '{';
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
	else if (Format->Element->Type == json_TypeArray)
	{
		*Code = '[';
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
				Format->Indent++;
				Format->NewLine = true;
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
		*Code = '"';
		Format->NameIndex = 0;
		return json_FormatKey;
	}
	else if (Format->Element->Type == json_TypeValueString)
	{
		*Code = '"';
		Format->NameIndex = 0;
		return json_FormatValueString;
	}
	else if (Format->Element->Type == json_TypeValueLiteral)
	{
		Format->NameIndex = 0;
		return JsonFormatValueLiteral(Format, Code);
	}
	else if (Format->Element->Type == json_TypeComment)
	{
		*Code = '/';
		Format->NameIndex = 0;
		return json_FormatCommentStart;
	}

	return json_FormatError;
}


static tJsonFormatState JsonFormatCommentStart(tJsonFormat *Format, tJsonUtf8Code *Code)
{
	*Code = (Format->CommentType == json_CommentLine) ? '/' : '*';
	Format->SpaceCount = 0;
	return json_FormatComment;
}


static tJsonFormatState JsonFormatComment(tJsonFormat *Format, tJsonUtf8Code *Code)
{
	tJsonElement *NextElement;
	tJsonUtf8Code NextCode;
	size_t CodeLength;

	if (Format->SpaceCount > 0)
	{
		*Code = ' ';
		Format->SpaceCount--;
		return json_FormatComment;
	}
	else if (Format->NameIndex < Format->Element->Name.Length)
	{
		CodeLength = JsonStringGetNextUtf8Code(&Format->Element->Name, Format->NameIndex, Code);
		if (CodeLength == 0)
		{
			return json_FormatError;
		}
		Format->NameIndex = Format->NameIndex + CodeLength;
		if (Format->NameIndex == Format->Element->Name.Length)
		{
			if (Format->CommentType == json_CommentBlock)
			{
				NextElement = JsonElementGetNext(Format->Element, false);
				if ((NextElement == NULL) || (NextElement->Type != json_TypeComment))
				{
					if ((*Code != '*') && !JsonCharacterIsWhitespace(*Code))
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
			*Code = '\n';
			Format->Element = NextElement;
			Format->NameIndex = 0;
			Format->SpaceCount = Format->Indent * Format->IndentSize + 1;
			if ((JsonStringGetNextUtf8Code(&NextElement->Name, 0, &NextCode) != 0) && (NextCode != '*'))
			{
				Format->SpaceCount++;
			}
			return json_FormatComment;
		}
		else
		{
			*Code = '*';
			return json_FormatCommentBlockEnd;
		}
	}
	else
	{
		return JsonFormatValueNext(Format, Code);
	}
}


static tJsonFormatState JsonFormatCommentBlockEnd(tJsonFormat *Format, tJsonUtf8Code *Code)
{
	*Code = '/';
	return json_FormatValueNext;
}


static void JsonFormatSetUp(tJsonFormat *Format, tJsonFormatType Type, size_t IndentSize, tJsonCommentType CommentType, tJsonElement *RootElement)
{
	Format->Type = Type;
	Format->State = json_FormatValueStart;
	Format->Element = JsonElementGetChild(RootElement, CommentType == json_CommentNone);
	Format->Utf8Code = 0;
	Format->Utf16Code = 0;
	Format->CommentType = CommentType;
	Format->Utf8CodeIndex = 0;
	Format->Utf16CodeIndex = 0;
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
	Format->Utf8Code = 0;
	Format->Utf16Code = 0;
	Format->CommentType = json_CommentNone;
	Format->Utf8CodeIndex = 0;
	Format->Utf16CodeIndex = 0;
	Format->NameIndex = 0;
	Format->Indent = 0;
	Format->IndentSize = 0;
	Format->SpaceCount = 0;
	Format->NewLine = false;
}


int JsonFormat(tJsonFormat *Format, tJsonUtf8Unit *CodeUnit)
{
	tJsonUtf8Code Code = 0;

	*CodeUnit = '\0';

	if ((Format->State != json_FormatComplete) && (Format->State != json_FormatError) && JsonFormatUtf8IsComplete(Format, CodeUnit))
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
					Format->State = JsonFormatKey(Format, &Code);
				break;

				case json_FormatKeyEscape:
					Format->State = JsonFormatKeyEscape(Format, &Code);
				break;

				case json_FormatKeyUtf16Escape:
					Format->State = JsonFormatKeyUtf16Escape(Format, &Code);
				break;

				case json_FormatKeyUtf16:
					Format->State = JsonFormatKeyUtf16(Format, &Code);
				break;

				case json_FormatKeyUtf16Digit1:
					Format->State = JsonFormatKeyUtf16Digit1(Format, &Code);
				break;

				case json_FormatKeyUtf16Digit2:
					Format->State = JsonFormatKeyUtf16Digit2(Format, &Code);
				break;

				case json_FormatKeyUtf16Digit3:
					Format->State = JsonFormatKeyUtf16Digit3(Format, &Code);
				break;

				case json_FormatKeyUtf16Digit4:
					Format->State = JsonFormatKeyUtf16Digit4(Format, &Code);
				break;

				case json_FormatKeyEnd:
					Format->State = JsonFormatKeyEnd(Format, &Code);
				break;

				case json_FormatValueStart:
					Format->State = JsonFormatValueStart(Format, &Code);
				break;

				case json_FormatValueString:
					Format->State = JsonFormatValueString(Format, &Code);
				break;

				case json_FormatValueStringEscape:
					Format->State = JsonFormatValueStringEscape(Format, &Code);
				break;

				case json_FormatValueStringUtf16Escape:
					Format->State = JsonFormatValueStringUtf16Escape(Format, &Code);
				break;

				case json_FormatValueStringUtf16:
					Format->State = JsonFormatValueStringUtf16(Format, &Code);
				break;

				case json_FormatValueStringUtf16Digit1:
					Format->State = JsonFormatValueStringUtf16Digit1(Format, &Code);
				break;

				case json_FormatValueStringUtf16Digit2:
					Format->State = JsonFormatValueStringUtf16Digit2(Format, &Code);
				break;

				case json_FormatValueStringUtf16Digit3:
					Format->State = JsonFormatValueStringUtf16Digit3(Format, &Code);
				break;

				case json_FormatValueStringUtf16Digit4:
					Format->State = JsonFormatValueStringUtf16Digit4(Format, &Code);
				break;

				case json_FormatValueLiteral:
					Format->State = JsonFormatValueLiteral(Format, &Code);
				break;

				case json_FormatValueNext:
					Format->State = JsonFormatValueNext(Format, &Code);
				break;

				case json_FormatValueEnd:
					Format->State = JsonFormatValueEnd(Format, &Code);
				break;

				case json_FormatCommentStart:
					Format->State = JsonFormatCommentStart(Format, &Code);
				break;

				case json_FormatComment:
					Format->State = JsonFormatComment(Format, &Code);
				break;

				case json_FormatCommentBlockEnd:
					Format->State = JsonFormatCommentBlockEnd(Format, &Code);
				break;

				default:
					Format->State = json_FormatError;
				break;
			}

			JsonFormatUtf8(Format, Code, CodeUnit);
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
