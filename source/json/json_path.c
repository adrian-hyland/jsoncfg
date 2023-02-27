#include <string.h>
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


size_t JsonPathGetNextUtf8Code(tJsonPath Path, size_t Offset, bool *IsEscaped, tJsonUtf8Code *Code)
{
	size_t Length1;
	size_t Length2;

	*IsEscaped = false;

	Length1 = JsonUtf8GetNextCode(Path.Value, Path.Length, Offset, Code);
	if (Length1 == 0)
	{
		*Code = '\0';
		return 0;
	}

	if (*Code != '\\')
	{
		return Length1;
	}

	Length2 = JsonUtf8GetNextCode(Path.Value, Path.Length, Offset + Length1, Code);
	if (Length2 == 0)
	{
		*Code = '\0';
		return 0;
	}

	*IsEscaped = true;
	*Code = JsonCharacterFromEscape(*Code);
	return Length1 + Length2;
}


size_t JsonPathGetPreviousUtf8Code(tJsonPath Path, size_t Offset, bool *IsEscaped, tJsonUtf8Code *Code)
{
	tJsonUtf8Code PreviousCode;
	size_t PreviousOffset;
	size_t Length;
	size_t Length1;
	size_t Length2;

	*IsEscaped = false;

	Length1 = JsonUtf8GetPreviousCode(Path.Value, Path.Length, Offset, Code);
	if (Length1 == 0)
	{
		*Code = '\0';
		return 0;
	}

	for (Length2 = 0, PreviousOffset = Offset - Length1; (PreviousOffset > 0); PreviousOffset = PreviousOffset - Length)
	{
		Length = JsonUtf8GetPreviousCode(Path.Value, Path.Length, PreviousOffset, &PreviousCode);
		if (Length == 0)
		{
			*IsEscaped = false;
			*Code = '\0';
			return 0;
		}
		if (Length2 == 0)
		{
			Length2 = Length;
		}
		if (PreviousCode != '\\')
		{
			break;
		}
		*IsEscaped = !*IsEscaped;
	}

	if (*IsEscaped)
	{
		*Code = JsonCharacterFromEscape(*Code);
		return Length1 + Length2;
	}

	if (*Code == '\\')
	{
		*IsEscaped = false;
		*Code = '\0';
		return 0;
	}
		
	return Length1;
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
