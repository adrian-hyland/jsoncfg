#include <string.h>
#include "json_utf16.h"
#include "json_character.h"
#include "json_path.h"


tJsonPath JsonPathUtf8(const tJsonUtf8Unit *PathString)
{
	tJsonPath Path;

	Path.Value = PathString;
	Path.Length = (PathString != NULL) ? strlen((const char *)PathString) : 0;

	return Path;
}


tJsonPath JsonPathAscii(const char *PathString)
{
	tJsonPath Path;

	Path.Value = (const tJsonUtf8Unit *)PathString;
	Path.Length = (PathString != NULL) ? strlen(PathString) : 0;

	return Path;
}


static size_t JsonPathGetNextUtf16Unit(tJsonPath Path, size_t Offset, tJsonUtf16Unit *Unit)
{
	tJsonUtf8Code Utf8Code;
	size_t NextOffset;
	size_t CodeLength;
	size_t n;

	CodeLength = JsonUtf8GetNextCode(Path.Value, Path.Length, Offset, &Utf8Code);
	if ((CodeLength == 0) || (Utf8Code != '\\'))
	{
		return 0;
	}
	NextOffset = Offset + CodeLength;

	CodeLength = JsonUtf8GetNextCode(Path.Value, Path.Length, NextOffset, &Utf8Code);
	if ((CodeLength == 0) || (Utf8Code != 'u'))
	{
		return 0;
	}
	NextOffset = NextOffset + CodeLength;

	for (n = 0; n < 4; n++)
	{
		CodeLength = JsonUtf8GetNextCode(Path.Value, Path.Length, NextOffset, &Utf8Code);
		if ((CodeLength == 0) || !JsonCharacterIsHexDigit(Utf8Code) || !JsonUtf16UnitSetNibble(Unit, n, JsonCharacterToHexDigit(Utf8Code)))
		{
			return 0;
		}
		NextOffset = NextOffset + CodeLength;
	}

	return NextOffset - Offset;
}


static size_t JsonPathGetPreviousUtf16Unit(tJsonPath Path, size_t Offset, tJsonUtf16Unit *Unit)
{
	tJsonUtf8Code Utf8Code;
	size_t PreviousOffset = Offset;
	size_t CodeLength;
	size_t n;

	for (n = 0; n < 4; n++)
	{
		CodeLength = JsonUtf8GetPreviousCode(Path.Value, Path.Length, PreviousOffset, &Utf8Code);
		if ((CodeLength == 0) || !JsonCharacterIsHexDigit(Utf8Code) || !JsonUtf16UnitSetNibble(Unit, 3 - n, JsonCharacterToHexDigit(Utf8Code)))
		{
			return 0;
		}
		PreviousOffset = PreviousOffset - CodeLength;
	}

	CodeLength = JsonUtf8GetPreviousCode(Path.Value, Path.Length, PreviousOffset, &Utf8Code);
	if ((CodeLength == 0) || (Utf8Code != 'u'))
	{
		return 0;
	}
	PreviousOffset = PreviousOffset - CodeLength;

	CodeLength = JsonUtf8GetPreviousCode(Path.Value, Path.Length, PreviousOffset, &Utf8Code);
	if ((CodeLength == 0) || (Utf8Code != '\\'))
	{
		return 0;
	}
	PreviousOffset = PreviousOffset - CodeLength;

	return Offset - PreviousOffset;
}


static size_t JsonPathGetNextUtf16Code(tJsonPath Path, size_t Offset, tJsonUtf16Code *Code)
{
	tJsonUtf16Unit LowUnit;
	tJsonUtf16Unit HighUnit;
	size_t NextOffset;
	size_t CodeLength;

	*Code = 0;

	CodeLength = JsonPathGetNextUtf16Unit(Path, Offset, &HighUnit);
	if (CodeLength == 0)
	{
		return 0;
	}
	NextOffset = Offset + CodeLength;

	if (JsonUtf16UnitIsHighSurrogate(HighUnit))
	{
		CodeLength = JsonPathGetNextUtf16Unit(Path, NextOffset, &LowUnit);
		if (CodeLength == 0)
		{
			return 0;
		}
		NextOffset = NextOffset + CodeLength;
	}
	else
	{
		LowUnit = HighUnit;
		HighUnit = 0;
	}

	*Code = HighUnit;
	if (!JsonUtf16CodeAddUnit(Code, LowUnit))
	{
		return 0;
	}

	if (!JsonUtf16CodeIsValid(*Code))
	{
		return 0;
	}

	return NextOffset - Offset;
}


static size_t JsonPathGetPreviousUtf16Code(tJsonPath Path, size_t Offset, tJsonUtf16Code *Code)
{
	tJsonUtf16Unit LowUnit;
	tJsonUtf16Unit HighUnit;
	size_t PreviousOffset = Offset;
	size_t CodeLength;

	*Code = 0;

	CodeLength = JsonPathGetPreviousUtf16Unit(Path, PreviousOffset, &LowUnit);
	if (CodeLength == 0)
	{
		return 0;
	}
	PreviousOffset = PreviousOffset - CodeLength;

	if (JsonUtf16UnitIsLowSurrogate(LowUnit))
	{
		CodeLength = JsonPathGetPreviousUtf16Unit(Path, PreviousOffset, &HighUnit);
		if (CodeLength == 0)
		{
			return 0;
		}
		PreviousOffset = PreviousOffset - CodeLength;
	}
	else
	{
		HighUnit = 0;
	}

	*Code = HighUnit;
	if (!JsonUtf16CodeAddUnit(Code, LowUnit))
	{
		return 0;
	}

	if (!JsonUtf16CodeIsValid(*Code))
	{
		return 0;
	}

	return Offset - PreviousOffset;
}


size_t JsonPathGetNextUtf8Code(tJsonPath Path, size_t Offset, bool *IsEscaped, tJsonUtf8Code *Code)
{
	tJsonUtf16Code Utf16Code;
	tJsonUtf8Code Utf8Code;
	size_t NextOffset;
	size_t CodeLength;

	*IsEscaped = false;
	*Code = 0;

	CodeLength = JsonUtf8GetNextCode(Path.Value, Path.Length, Offset, &Utf8Code);
	if (CodeLength == 0)
	{
		return 0;
	}
	NextOffset = Offset + CodeLength;
	
	if (Utf8Code != '\\')
	{
		*Code = Utf8Code;
		return NextOffset - Offset;
	}

	CodeLength = JsonUtf8GetNextCode(Path.Value, Path.Length, NextOffset, &Utf8Code);
	if (CodeLength == 0)
	{
		return 0;
	}
	NextOffset = NextOffset + CodeLength;

	if (Utf8Code != 'u')
	{
		*IsEscaped = true;
		*Code = JsonCharacterFromEscape(Utf8Code);
		return NextOffset - Offset;
	}

	CodeLength = JsonPathGetNextUtf16Code(Path, Offset, &Utf16Code);
	if (CodeLength == 0)
	{
		return 0;
	}
	NextOffset = Offset + CodeLength;

	*IsEscaped = true;
	*Code = JsonUtf8Code(JsonUtf16CodeGetCharacter(Utf16Code));
	return NextOffset - Offset;
}


size_t JsonPathGetPreviousUtf8Code(tJsonPath Path, size_t Offset, bool *IsEscaped, tJsonUtf8Code *Code)
{
	tJsonUtf16Code Utf16Code;
	tJsonUtf8Code Utf8Code;
	tJsonUtf8Code PreviousCode;
	size_t UnescapedOffset;
	size_t EscapedOffset;
	size_t PreviousOffset;
	size_t CodeLength;
	bool IsEscapedUtf16;

	*IsEscaped = false;
	*Code = 0;

	CodeLength = JsonUtf8GetPreviousCode(Path.Value, Path.Length, Offset, &Utf8Code);
	if (CodeLength == 0)
	{
		return 0;
	}
	UnescapedOffset = Offset - CodeLength;

	IsEscapedUtf16 = false;
	EscapedOffset = UnescapedOffset;
	if (JsonCharacterIsHexDigit(Utf8Code))
	{
		CodeLength = JsonPathGetPreviousUtf16Code(Path, Offset, &Utf16Code);
		IsEscapedUtf16 = (CodeLength != 0);
		if (IsEscapedUtf16)
		{
			*IsEscaped = true;
			EscapedOffset = Offset - CodeLength;
		}
	}

	if (!*IsEscaped)
	{
		if (UnescapedOffset == 0)
		{
			if (Utf8Code == '\\')
			{
				return 0;
			}

			*Code = Utf8Code;
			return Offset;
		}

		CodeLength = JsonUtf8GetPreviousCode(Path.Value, Path.Length, UnescapedOffset, &PreviousCode);
		if (CodeLength == 0)
		{
			return 0;
		}

		if (PreviousCode != '\\')
		{
			if (Utf8Code == '\\')
			{
				return 0;
			}

			*Code = Utf8Code;
			return Offset - UnescapedOffset;
		}

		*IsEscaped = true;
		EscapedOffset = UnescapedOffset - CodeLength;
	}

	for (PreviousOffset = EscapedOffset; PreviousOffset > 0; PreviousOffset = PreviousOffset - CodeLength)
	{
		CodeLength = JsonUtf8GetPreviousCode(Path.Value, Path.Length, PreviousOffset, &PreviousCode);
		if (CodeLength == 0)
		{
			*IsEscaped = false;
			return 0;
		}
		if (PreviousCode != '\\')
		{
			break;
		}
		*IsEscaped = !*IsEscaped;
	}

	if (!*IsEscaped)
	{
		if (Utf8Code == '\\')
		{
			return 0;
		}

		*Code = Utf8Code;
		
		return Offset - UnescapedOffset;
	}
	else
	{
		if (Utf8Code == 'u')
		{
			return 0;
		}

		*Code = IsEscapedUtf16 ? JsonUtf8Code(JsonUtf16CodeGetCharacter(Utf16Code)) : JsonCharacterFromEscape(Utf8Code);

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


bool JsonPathSetString(tJsonPath Path, tJsonString *String)
{
	tJsonUtf8Code Code;
	size_t CodeLength;
	size_t PathOffset;
	bool IsEscaped;

	JsonStringClear(String);

	for (PathOffset = 0; (PathOffset < Path.Length); PathOffset = PathOffset + CodeLength)
	{
		CodeLength = JsonPathGetNextUtf8Code(Path, PathOffset, &IsEscaped, &Code);
		if (CodeLength == 0)
		{
			return false;
		}

		if (!IsEscaped && ((Code == '/') || (Code == ':') || (Code == '[') || (Code == ']')))
		{
			return false;
		}

		if (!JsonStringAddUtf8Code(String, Code))
		{
			return false;
		}
	}

	return true;
}


bool JsonPathCompareString(tJsonPath Path, tJsonString *String)
{
	tJsonUtf8Code PathCode;
	tJsonUtf8Code StringCode;
	size_t PathCodeLength;
	size_t StringCodeLength;
	size_t StringOffset;
	size_t StringLength;
	size_t PathOffset;
	bool IsEscaped;

	StringLength = JsonStringGetLength(String);

	for (StringOffset = 0, PathOffset = 0; (StringOffset < StringLength) && (PathOffset < Path.Length); StringOffset = StringOffset + StringCodeLength, PathOffset = PathOffset + PathCodeLength)
	{
		PathCodeLength = JsonPathGetNextUtf8Code(Path, PathOffset, &IsEscaped, &PathCode);
		if (PathCodeLength == 0)
		{
			return false;
		}

		if (!IsEscaped && ((PathCode == '/') || (PathCode == ':') || (PathCode == '[') || (PathCode == ']')))
		{
			return false;
		}

		StringCodeLength = JsonStringGetNextUtf8Code(String, StringOffset, &StringCode);
		if ((StringCodeLength == 0) || (PathCode != StringCode))
		{
			return false;
		}
	}

	return (StringOffset == StringLength) && (PathOffset == Path.Length);
}


static size_t JsonPathSkipSpaceLeft(tJsonPath Path)
{
	tJsonUtf8Code Code;
	size_t CodeLength;
	size_t Offset;
	bool IsEscaped;

	for (Offset = 0; Offset < Path.Length; Offset = Offset + CodeLength)
	{
		CodeLength = JsonPathGetNextUtf8Code(Path, Offset, &IsEscaped, &Code);
		if ((CodeLength == 0) || IsEscaped || (Code != ' '))
		{
			break;
		}
	}

	return Offset;
}


static size_t JsonPathSkipSpaceRight(tJsonPath Path)
{
	tJsonUtf8Code Code;
	size_t CodeLength;
	size_t Offset;
	bool IsEscaped;

	for (Offset = Path.Length; Offset > 0; Offset = Offset - CodeLength)
	{
		CodeLength = JsonPathGetPreviousUtf8Code(Path, Offset, &IsEscaped, &Code);
		if ((CodeLength == 0) || IsEscaped || (Code != ' '))
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
	tJsonUtf8Code Code;
	size_t CodeLength;
	size_t Length;
	bool IsEscaped;

	for (Length = 0; Length < Path.Length; Length = Length + CodeLength)
	{
		CodeLength = JsonPathGetNextUtf8Code(Path, Length, &IsEscaped, &Code);
		if (CodeLength == 0)
		{
			return 0;
		}

		if (!IsEscaped && ((Code == '/') || (Code == ':') || (Code == '[') || (Code == ']')))
		{
			break;
		}
	}

	*Name = JsonPathTrimSpaces(JsonPathLeft(Path, Length));

	return Length;
}


static bool JsonPathTrimQuotes(tJsonPath Path, tJsonPath *UnquotedPath)
{
	tJsonUtf8Code FirstCode;
	tJsonUtf8Code LastCode;
	size_t FirstCodeLength;
	size_t LastCodeLength;
	bool FirstIsEscaped;
	bool LastIsEscaped;

	*UnquotedPath = Path;

	if (Path.Length == 0)
	{
		return true;
	}

	FirstCodeLength = JsonPathGetNextUtf8Code(Path, 0, &FirstIsEscaped, &FirstCode);
	if (FirstCodeLength == 0)
	{
		return false;
	}

	if (FirstCodeLength == Path.Length)
	{
		return FirstCode != '"';
	}

	LastCodeLength = JsonPathGetPreviousUtf8Code(Path, Path.Length, &LastIsEscaped, &LastCode);
	if (LastCodeLength == 0)
	{
		return false;
	}

	if (FirstIsEscaped || LastIsEscaped)
	{
		return FirstIsEscaped ? LastIsEscaped || (LastCode != '"') : (FirstCode != '"');
	}
	else
	{
		if ((FirstCode == '"') && (LastCode == '"'))
		{
			*UnquotedPath = JsonPathMiddle(Path, FirstCodeLength, Path.Length - LastCodeLength);
		}
		return (FirstCode == '"') ? (LastCode == '"') : (LastCode != '"');
	}
}


size_t JsonPathGetComponent(tJsonPath Path, tJsonType *ComponentType, tJsonPath *Component)
{
	tJsonPath Name;
	tJsonUtf8Code Code;
	size_t CodeLength;
	size_t Start;
	size_t Length;
	size_t Offset;
	size_t NestedCount;
	bool IsEscaped;

	Start = JsonPathSkipSpaceLeft(Path);

	CodeLength = JsonPathGetNextUtf8Code(Path, Start, &IsEscaped, &Code);
	if (CodeLength == 0)
	{
		return 0;
	}
	
	if (!IsEscaped)
	{
		if (Code == '/')
		{
			*ComponentType = json_TypeObject;
			*Component = JsonPathUtf8(NULL);
			Offset = Start + CodeLength;
			return Offset + JsonPathSkipSpaceLeft(JsonPathRight(Path, Offset));
		}
	
		if (Code == '[')
		{
			NestedCount = 0;
			for (Offset = Start + CodeLength; Offset < Path.Length; Offset = Offset + Length)
			{
				Length = JsonPathGetNextUtf8Code(Path, Offset, &IsEscaped, &Code);
				if (!IsEscaped)
				{
					if (Code == ']')
					{
						if (NestedCount == 0)
						{
							*ComponentType = json_TypeArray;
							*Component = JsonPathTrimSpaces(JsonPathMiddle(Path, Start + CodeLength, Offset));
							Offset = Offset + Length;
							return Offset + JsonPathSkipSpaceLeft(JsonPathRight(Path, Offset));
						}
						NestedCount--;
					}
					else if (Code == '[')
					{
						NestedCount++;
					}
				}
			}

			return 0;
		}
	
		if (Code == ']')
		{
			return 0;
		}

		if (Code == ':')
		{
			Offset = Start + CodeLength;

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
