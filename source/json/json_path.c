#include <string.h>
#include "json_utf16.h"
#include "json_character.h"
#include "json_path.h"


tJsonPath JsonPathUtf8(const uint8_t *PathString)
{
	tJsonPath Path;

	Path.Value = PathString;
	Path.Length = (PathString != NULL) ? strlen((const char *)PathString) : 0;

	return Path;
}


tJsonPath JsonPathAscii(const char *PathString)
{
	tJsonPath Path;

	Path.Value = (const uint8_t *)PathString;
	Path.Length = (PathString != NULL) ? strlen(PathString) : 0;

	return Path;
}


static size_t JsonPathGetNextUtf16Unit(tJsonPath Path, size_t Offset, tJsonUtf16Unit Unit)
{
	tJsonCharacter Character1;
	tJsonCharacter Character2;
	size_t NextOffset;
	size_t DecodeLength;
	size_t n;

	DecodeLength = JsonUtf8DecodeNext(Path.Value, Path.Length, Offset, &Character1);
	if ((DecodeLength == 0) || (Character1 != '\\'))
	{
		return 0;
	}
	NextOffset = Offset + DecodeLength;

	DecodeLength = JsonUtf8DecodeNext(Path.Value, Path.Length, NextOffset, &Character1);
	if ((DecodeLength == 0) || (Character1 != 'u'))
	{
		return 0;
	}
	NextOffset = NextOffset + DecodeLength;

	for (n = 0; n < JSON_UTF16_UNIT_SIZE; n++)
	{
		DecodeLength = JsonUtf8DecodeNext(Path.Value, Path.Length, NextOffset, &Character1);
		if ((DecodeLength == 0) || !JsonCharacterIsHexDigit(Character1))
		{
			return 0;
		}
		NextOffset = NextOffset + DecodeLength;
		DecodeLength = JsonUtf8DecodeNext(Path.Value, Path.Length, NextOffset, &Character2);
		if ((DecodeLength == 0) || !JsonCharacterIsHexDigit(Character2))
		{
			return 0;
		}
		NextOffset = NextOffset + DecodeLength;
		Unit[n] = (JsonCharacterToHexDigit(Character1) << 4) + JsonCharacterToHexDigit(Character2);
	}

	return NextOffset - Offset;
}


static size_t JsonPathGetPreviousUtf16Unit(tJsonPath Path, size_t Offset, tJsonUtf16Unit Unit)
{
	tJsonCharacter Character1;
	tJsonCharacter Character2;
	size_t PreviousOffset = Offset;
	size_t DecodeLength;
	size_t n;

	for (n = 0; n < JSON_UTF16_UNIT_SIZE; n++)
	{
		DecodeLength = JsonUtf8DecodePrevious(Path.Value, Path.Length, PreviousOffset, &Character1);
		if ((DecodeLength == 0) || !JsonCharacterIsHexDigit(Character1))
		{
			return 0;
		}
		PreviousOffset = PreviousOffset - DecodeLength;
		DecodeLength = JsonUtf8DecodePrevious(Path.Value, Path.Length, PreviousOffset, &Character2);
		if ((DecodeLength == 0) || !JsonCharacterIsHexDigit(Character2))
		{
			return 0;
		}
		PreviousOffset = PreviousOffset - DecodeLength;
		Unit[JSON_UTF16_UNIT_SIZE - n - 1] = (JsonCharacterToHexDigit(Character2) << 4) + JsonCharacterToHexDigit(Character1);
	}

	DecodeLength = JsonUtf8DecodePrevious(Path.Value, Path.Length, PreviousOffset, &Character1);
	if ((DecodeLength == 0) || (Character1 != 'u'))
	{
		return 0;
	}
	PreviousOffset = PreviousOffset - DecodeLength;

	DecodeLength = JsonUtf8DecodePrevious(Path.Value, Path.Length, PreviousOffset, &Character1);
	if ((DecodeLength == 0) || (Character1 != '\\'))
	{
		return 0;
	}
	PreviousOffset = PreviousOffset - DecodeLength;

	return Offset - PreviousOffset;
}


static size_t JsonPathGetNextUtf16Character(tJsonPath Path, size_t Offset, tJsonCharacter *Character)
{
	tJsonUtf16 Utf16;
	size_t NextOffset;
	size_t DecodeLength;
	size_t Length;

	Length = JsonPathGetNextUtf16Unit(Path, Offset, Utf16);
	if (Length == 0)
	{
		return 0;
	}
	NextOffset = Offset + Length;

	if (!JsonUtf16beIsHighSurrogate(Utf16))
	{
		DecodeLength = JsonUtf16beDecodeNext(Utf16, JSON_UTF16_UNIT_SIZE, 0, Character);
		return (DecodeLength != 0) ? NextOffset - Offset : 0;
	}

	Length = JsonPathGetNextUtf16Unit(Path, NextOffset, &Utf16[JSON_UTF16_UNIT_SIZE]);
	if (Length == 0)
	{
		return 0;
	}
	NextOffset = NextOffset + Length;

	DecodeLength = JsonUtf16beDecodeNext(Utf16, 2 * JSON_UTF16_UNIT_SIZE, 0, Character);
	return (DecodeLength != 0) ? NextOffset - Offset : 0;
}


static size_t JsonPathGetPreviousUtf16Character(tJsonPath Path, size_t Offset, tJsonCharacter *Character)
{
	tJsonUtf16 Utf16;
	size_t PreviousOffset;
	size_t DecodeLength;
	size_t Length;

	Length = JsonPathGetPreviousUtf16Unit(Path, Offset, &Utf16[JSON_UTF16_UNIT_SIZE]);
	if (Length == 0)
	{
		return 0;
	}
	PreviousOffset = Offset - Length;

	if (!JsonUtf16beIsLowSurrogate(&Utf16[JSON_UTF16_UNIT_SIZE]))
	{
		DecodeLength = JsonUtf16beDecodeNext(&Utf16[JSON_UTF16_UNIT_SIZE], JSON_UTF16_UNIT_SIZE, 0, Character);
		return (DecodeLength != 0) ? Offset - PreviousOffset : 0;
	}

	Length = JsonPathGetPreviousUtf16Unit(Path, PreviousOffset, Utf16);
	if (Length == 0)
	{
		return 0;
	}
	PreviousOffset = PreviousOffset - Length;

	DecodeLength = JsonUtf16beDecodeNext(Utf16, 2 * JSON_UTF16_UNIT_SIZE, 0, Character);
	return (DecodeLength != 0) ? Offset - PreviousOffset : 0;
}


size_t JsonPathGetNextCharacter(tJsonPath Path, size_t Offset, bool *IsEscaped, tJsonCharacter *Character)
{
	size_t NextOffset;
	size_t Length;

	*IsEscaped = false;
	*Character = '\0';

	Length = JsonUtf8DecodeNext(Path.Value, Path.Length, Offset, Character);
	if (Length == 0)
	{
		return 0;
	}
	NextOffset = Offset + Length;
	
	if (*Character != '\\')
	{
		return NextOffset - Offset;
	}

	*IsEscaped = true;

	Length = JsonUtf8DecodeNext(Path.Value, Path.Length, NextOffset, Character);
	if (Length == 0)
	{
		return 0;
	}
	NextOffset = NextOffset + Length;

	if (*Character != 'u')
	{
		*Character = JsonCharacterFromEscape(*Character);
		return NextOffset - Offset;
	}

	Length = JsonPathGetNextUtf16Character(Path, Offset, Character);
	if (Length == 0)
	{
		return 0;
	}
	NextOffset = Offset + Length;

	return NextOffset - Offset;
}


size_t JsonPathGetPreviousCharacter(tJsonPath Path, size_t Offset, bool *IsEscaped, tJsonCharacter *Character)
{
	tJsonCharacter Utf16Character;
	tJsonCharacter PreviousCharacter;
	size_t UnescapedOffset;
	size_t EscapedOffset;
	size_t PreviousOffset;
	size_t DecodeLength;
	bool IsEscapedUtf16;

	*IsEscaped = false;
	*Character = '\0';

	DecodeLength = JsonUtf8DecodePrevious(Path.Value, Path.Length, Offset, Character);
	if (DecodeLength == 0)
	{
		return 0;
	}
	UnescapedOffset = Offset - DecodeLength;

	IsEscapedUtf16 = false;
	EscapedOffset = UnescapedOffset;
	if (JsonCharacterIsHexDigit(*Character))
	{
		DecodeLength = JsonPathGetPreviousUtf16Character(Path, Offset, &Utf16Character);
		IsEscapedUtf16 = (DecodeLength != 0);
		if (IsEscapedUtf16)
		{
			*IsEscaped = true;
			EscapedOffset = Offset - DecodeLength;
		}
	}

	if (!*IsEscaped)
	{
		if (UnescapedOffset == 0)
		{
			if (*Character == '\\')
			{
				return 0;
			}

			return Offset;
		}

		DecodeLength = JsonUtf8DecodePrevious(Path.Value, Path.Length, UnescapedOffset, &PreviousCharacter);
		if (DecodeLength == 0)
		{
			return 0;
		}

		if (PreviousCharacter != '\\')
		{
			if (*Character == '\\')
			{
				return 0;
			}

			return Offset - UnescapedOffset;
		}

		*IsEscaped = true;
		EscapedOffset = UnescapedOffset - DecodeLength;
	}

	for (PreviousOffset = EscapedOffset; PreviousOffset > 0; PreviousOffset = PreviousOffset - DecodeLength)
	{
		DecodeLength = JsonUtf8DecodePrevious(Path.Value, Path.Length, PreviousOffset, &PreviousCharacter);
		if (DecodeLength == 0)
		{
			*IsEscaped = false;
			return 0;
		}
		if (PreviousCharacter != '\\')
		{
			break;
		}
		*IsEscaped = !*IsEscaped;
	}

	if (!*IsEscaped)
	{
		if (*Character == '\\')
		{
			return 0;
		}

		return Offset - UnescapedOffset;
	}
	else
	{
		if (*Character == 'u')
		{
			return 0;
		}

		*Character = IsEscapedUtf16 ? Utf16Character : JsonCharacterFromEscape(*Character);

		return Offset - EscapedOffset;
	}
}


tJsonPath JsonPathLeft(tJsonPath Path, size_t ToOffset)
{
	tJsonPath SubPath;

	if (ToOffset > Path.Length)
	{
		ToOffset = Path.Length;
	}

	SubPath.Value = Path.Value;
	SubPath.Length = ToOffset;

	return SubPath;
}


tJsonPath JsonPathRight(tJsonPath Path, size_t FromOffset)
{
	tJsonPath SubPath;

	if (FromOffset > Path.Length)
	{
		FromOffset = Path.Length;
	}

	SubPath.Value = &Path.Value[FromOffset];
	SubPath.Length = Path.Length - FromOffset;

	return SubPath;
}


tJsonPath JsonPathMiddle(tJsonPath Path, size_t FromOffset, size_t ToOffset)
{
	tJsonPath SubPath;

	if (FromOffset > Path.Length)
	{
		FromOffset = Path.Length;
	}
	if (ToOffset > Path.Length)
	{
		ToOffset = Path.Length;
	}
	if (FromOffset > ToOffset)
	{
		ToOffset = FromOffset;
	}

	SubPath.Value = &Path.Value[FromOffset];
	SubPath.Length = ToOffset - FromOffset;

	return SubPath;
}


bool JsonPathGetString(tJsonPath Path, tJsonString *String)
{
	tJsonCharacter Character;
	size_t DecodeLength;
	size_t PathOffset;
	bool IsEscaped;

	JsonStringClear(String);

	for (PathOffset = 0; (PathOffset < Path.Length); PathOffset = PathOffset + DecodeLength)
	{
		DecodeLength = JsonPathGetNextCharacter(Path, PathOffset, &IsEscaped, &Character);
		if (DecodeLength == 0)
		{
			return false;
		}

		if (!IsEscaped && ((Character == '/') || (Character == ':') || (Character == '[') || (Character == ']')))
		{
			return false;
		}

		if (!JsonStringAddCharacter(String, Character))
		{
			return false;
		}
	}

	return true;
}


bool JsonPathCompareString(tJsonPath Path, tJsonString *String)
{
	tJsonCharacter PathCharacter;
	tJsonCharacter StringCharacter;
	size_t PathDecodeLength;
	size_t StringDecodeLength;
	size_t StringOffset;
	size_t StringLength;
	size_t PathOffset;
	bool IsEscaped;

	StringLength = JsonStringGetLength(String);

	for (StringOffset = 0, PathOffset = 0; (StringOffset < StringLength) && (PathOffset < Path.Length); StringOffset = StringOffset + StringDecodeLength, PathOffset = PathOffset + PathDecodeLength)
	{
		PathDecodeLength = JsonPathGetNextCharacter(Path, PathOffset, &IsEscaped, &PathCharacter);
		if (PathDecodeLength == 0)
		{
			return false;
		}

		if (!IsEscaped && ((PathCharacter == '/') || (PathCharacter == ':') || (PathCharacter == '[') || (PathCharacter == ']')))
		{
			return false;
		}

		StringDecodeLength = JsonStringGetNextCharacter(String, StringOffset, &StringCharacter);
		if ((StringDecodeLength == 0) || (PathCharacter != StringCharacter))
		{
			return false;
		}
	}

	return (StringOffset == StringLength) && (PathOffset == Path.Length);
}


static size_t JsonPathSkipSpaceLeft(tJsonPath Path)
{
	tJsonCharacter Character;
	size_t DecodeLength;
	size_t Offset;
	bool IsEscaped;

	for (Offset = 0; Offset < Path.Length; Offset = Offset + DecodeLength)
	{
		DecodeLength = JsonPathGetNextCharacter(Path, Offset, &IsEscaped, &Character);
		if ((DecodeLength == 0) || IsEscaped || (Character != ' '))
		{
			break;
		}
	}

	return Offset;
}


static size_t JsonPathSkipSpaceRight(tJsonPath Path)
{
	tJsonCharacter Character;
	size_t DecodeLength;
	size_t Offset;
	bool IsEscaped;

	for (Offset = Path.Length; Offset > 0; Offset = Offset - DecodeLength)
	{
		DecodeLength = JsonPathGetPreviousCharacter(Path, Offset, &IsEscaped, &Character);
		if ((DecodeLength == 0) || IsEscaped || (Character != ' '))
		{
			break;
		}
	}

	return Offset;
}


static tJsonPath JsonPathTrimSpaces(tJsonPath Path)
{
	return JsonPathMiddle(Path, JsonPathSkipSpaceLeft(Path), JsonPathSkipSpaceRight(Path));
}


static size_t JsonPathGetName(tJsonPath Path, tJsonPath *Name)
{
	tJsonCharacter Character;
	size_t DecodeLength;
	size_t Length;
	bool IsEscaped;

	for (Length = 0; Length < Path.Length; Length = Length + DecodeLength)
	{
		DecodeLength = JsonPathGetNextCharacter(Path, Length, &IsEscaped, &Character);
		if (DecodeLength == 0)
		{
			return 0;
		}

		if (!IsEscaped && ((Character == '/') || (Character == ':') || (Character == '[') || (Character == ']')))
		{
			break;
		}
	}

	*Name = JsonPathTrimSpaces(JsonPathLeft(Path, Length));

	return Length;
}


static bool JsonPathTrimQuotes(tJsonPath Path, tJsonPath *UnquotedPath)
{
	tJsonCharacter FirstCharacter;
	tJsonCharacter LastCharacter;
	size_t FirstDecodeLength;
	size_t LastDecodeLength;
	bool FirstIsEscaped;
	bool LastIsEscaped;

	*UnquotedPath = Path;

	if (Path.Length == 0)
	{
		return true;
	}

	FirstDecodeLength = JsonPathGetNextCharacter(Path, 0, &FirstIsEscaped, &FirstCharacter);
	if (FirstDecodeLength == 0)
	{
		return false;
	}

	if (FirstDecodeLength == Path.Length)
	{
		return FirstCharacter != '"';
	}

	LastDecodeLength = JsonPathGetPreviousCharacter(Path, Path.Length, &LastIsEscaped, &LastCharacter);
	if (LastDecodeLength == 0)
	{
		return false;
	}

	if (FirstIsEscaped || LastIsEscaped)
	{
		return FirstIsEscaped ? LastIsEscaped || (LastCharacter != '"') : (FirstCharacter != '"');
	}
	else
	{
		if ((FirstCharacter == '"') && (LastCharacter == '"'))
		{
			*UnquotedPath = JsonPathMiddle(Path, FirstDecodeLength, Path.Length - LastDecodeLength);
		}
		return (FirstCharacter == '"') ? (LastCharacter == '"') : (LastCharacter != '"');
	}
}


size_t JsonPathGetComponent(tJsonPath Path, tJsonType *ComponentType, tJsonPath *Component)
{
	tJsonPath Name;
	tJsonCharacter Character;
	size_t DecodeLength;
	size_t Start;
	size_t Length;
	size_t Offset;
	size_t NestedCount;
	bool IsEscaped;

	Start = JsonPathSkipSpaceLeft(Path);

	DecodeLength = JsonPathGetNextCharacter(Path, Start, &IsEscaped, &Character);
	if (DecodeLength == 0)
	{
		return 0;
	}
	
	if (!IsEscaped)
	{
		if (Character == '/')
		{
			*ComponentType = json_TypeObject;
			*Component = JsonPathUtf8(NULL);
			Offset = Start + DecodeLength;
			return Offset + JsonPathSkipSpaceLeft(JsonPathRight(Path, Offset));
		}
	
		if (Character == '[')
		{
			NestedCount = 0;
			for (Offset = Start + DecodeLength; Offset < Path.Length; Offset = Offset + Length)
			{
				Length = JsonPathGetNextCharacter(Path, Offset, &IsEscaped, &Character);
				if (!IsEscaped)
				{
					if (Character == ']')
					{
						if (NestedCount == 0)
						{
							*ComponentType = json_TypeArray;
							*Component = JsonPathTrimSpaces(JsonPathMiddle(Path, Start + DecodeLength, Offset));
							Offset = Offset + Length;
							return Offset + JsonPathSkipSpaceLeft(JsonPathRight(Path, Offset));
						}
						NestedCount--;
					}
					else if (Character == '[')
					{
						NestedCount++;
					}
				}
			}

			return 0;
		}
	
		if (Character == ']')
		{
			return 0;
		}

		if (Character == ':')
		{
			Offset = Start + DecodeLength;

			Length = JsonPathGetName(JsonPathRight(Path, Offset), &Name);
			if ((Length == 0) || !JsonPathTrimQuotes(Name, Component))
			{
				return 0;
			}

			*ComponentType = (Name.Length == Component->Length) ? json_TypeValueLiteral : json_TypeValueString;

			return Offset + Length;
		}
	}

	Length = JsonPathGetName(JsonPathRight(Path, Start), &Name);
	if ((Length == 0) || !JsonPathTrimQuotes(Name, Component))
	{
		return 0;
	}

	*ComponentType = json_TypeKey;

	return Start + Length;
}
