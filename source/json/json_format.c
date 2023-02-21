#include "json_character.h"
#include "json_format.h"


#define JSON_FORMAT_INDENT_SPACE_COUNT 3


static tJsonFormatState JsonFormatValueStart(tJsonFormat *Format, uint8_t *Character);
static tJsonFormatState JsonFormatValueEnd(tJsonFormat *Format, uint8_t *Character);


static tJsonFormatState JsonFormatKeyEscape(tJsonFormat *Format, uint8_t *Character)
{
	size_t CharacterLength;

	CharacterLength = JsonStringGetCharacter(&Format->Element->Name, Format->NameIndex, Character);
	if (CharacterLength == 0)
	{
		return json_FormatError;
	}
	*Character = JsonCharacterToEscape(*Character);
	Format->NameIndex = Format->NameIndex + CharacterLength;
	return json_FormatKey;
}


static tJsonFormatState JsonFormatKey(tJsonFormat *Format, uint8_t *Character)
{
	size_t CharacterLength;
	if (Format->NameIndex < Format->Element->Name.Length)
	{
		CharacterLength = JsonStringGetCharacter(&Format->Element->Name, Format->NameIndex, Character);
		if (CharacterLength == 0)
		{
			return json_FormatError;
		}
		if (JsonCharacterIsEscapable(*Character))
		{
			*Character = '\\';
			return json_FormatKeyEscape;
		}
		Format->NameIndex = Format->NameIndex + CharacterLength;
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


static tJsonFormatState JsonFormatValueNext(tJsonFormat *Format, uint8_t *Character)
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


static tJsonFormatState JsonFormatValueEnd(tJsonFormat *Format, uint8_t *Character)
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
	else if (Format->Element->Type == json_TypeKey)
	{
		return JsonFormatValueNext(Format, Character);
	}
	else if (Format->Element->Type == json_TypeComment)
	{
		*Character = '/';
		Format->NameIndex = 0;
		return json_FormatCommentStart;
	}

	return json_FormatError;
}


static tJsonFormatState JsonFormatValueStringEscape(tJsonFormat *Format, uint8_t *Character)
{
	size_t CharacterLength;

	CharacterLength = JsonStringGetCharacter(&Format->Element->Name, Format->NameIndex, Character);
	if (CharacterLength == 0)
	{
		return json_FormatError;
	}
	*Character = JsonCharacterToEscape(*Character);
	Format->NameIndex = Format->NameIndex + CharacterLength;
	return json_FormatValueString;
}


static tJsonFormatState JsonFormatValueString(tJsonFormat *Format, uint8_t *Character)
{
	size_t CharacterLength;

	if (Format->NameIndex < Format->Element->Name.Length)
	{
		CharacterLength = JsonStringGetCharacter(&Format->Element->Name, Format->NameIndex, Character);
		if (CharacterLength == 0)
		{
			return json_FormatError;
		}
		if (JsonCharacterIsEscapable(*Character))
		{
			*Character = '\\';
			return json_FormatValueStringEscape;
		}
		Format->NameIndex = Format->NameIndex + CharacterLength;
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
	size_t CharacterLength;

	if (Format->NameIndex < Format->Element->Name.Length)
	{
		CharacterLength = JsonStringGetCharacter(&Format->Element->Name, Format->NameIndex, Character);
		if (CharacterLength == 0)
		{
			return json_FormatError;
		}
		Format->NameIndex = Format->NameIndex + CharacterLength;
		return json_FormatValueLiteral;
	}
	else
	{
		return JsonFormatValueNext(Format, Character);
	}
}


static tJsonFormatState JsonFormatValueStart(tJsonFormat *Format, uint8_t *Character)
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
	else if (Format->Element->Type == json_TypeObject)
	{
		*Character = '{';
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
		*Character = '[';
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


static tJsonFormatState JsonFormatCommentStart(tJsonFormat *Format, uint8_t *Character)
{
	*Character = (Format->CommentType == json_CommentLine) ? '/' : '*';
	Format->SpaceCount = 0;
	return json_FormatComment;
}


static tJsonFormatState JsonFormatComment(tJsonFormat *Format, uint8_t *Character)
{
	tJsonElement *NextElement;
	uint8_t NextCharacter;
	size_t CharacterLength;

	if (Format->SpaceCount > 0)
	{
		*Character = ' ';
		Format->SpaceCount--;
		return json_FormatComment;
	}
	else if (Format->NameIndex < Format->Element->Name.Length)
	{
		CharacterLength = JsonStringGetCharacter(&Format->Element->Name, Format->NameIndex, Character);
		Format->NameIndex = Format->NameIndex + CharacterLength;
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
			if ((JsonStringGetCharacter(&NextElement->Name, 0, &NextCharacter) != 0) && (NextCharacter != '*'))
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


static tJsonFormatState JsonFormatCommentBlockEnd(tJsonFormat *Format, uint8_t *Character)
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
	Format->NameIndex = 0;
	Format->Indent = 0;
	Format->IndentSize = 0;
	Format->SpaceCount = 0;
	Format->NewLine = false;
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
