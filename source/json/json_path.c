#include <string.h>
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


size_t JsonPathGetNextCharacter(tJsonPath Path, size_t Offset, bool *IsEscaped, uint8_t *Character)
{
	size_t PathOffset = Offset;

	if (PathOffset < Path.Length)
	{
		*Character = Path.Value[PathOffset];
		PathOffset++;
		if (*Character != '\\')
		{
			*IsEscaped = false;
			return PathOffset - Offset;
		}
		
		if (PathOffset < Path.Length)
		{
			*IsEscaped = true;
			*Character = Path.Value[PathOffset];
			*Character = JsonCharacterFromEscape(*Character);
			PathOffset++;
			return PathOffset - Offset;
		}
	}

	*IsEscaped = false;
	*Character = '\0';
	return 0;
}


size_t JsonPathGetPreviousCharacter(tJsonPath Path, size_t Offset, bool *IsEscaped, uint8_t *Character)
{
	size_t PreviousOffset;
	size_t PathOffset = Offset;

	if ((PathOffset > 0) && (PathOffset <= Path.Length))
	{
		PathOffset--;
		*Character = Path.Value[PathOffset];
		if ((PathOffset == 0) || (Path.Value[PathOffset - 1] != '\\'))
		{
			*IsEscaped = false;
		}
		else
		{
			*IsEscaped = true;
			for (PreviousOffset = PathOffset - 1; (PreviousOffset > 0) && (Path.Value[PreviousOffset - 1] == '\\'); PreviousOffset--)
			{
				*IsEscaped = !*IsEscaped;
			}
			if (*IsEscaped)
			{
				PathOffset--;
			}
		}
	}

	if (*IsEscaped || (*Character != '\\'))
	{
		if (*IsEscaped)
		{
			*Character = JsonCharacterFromEscape(*Character);
		}
		
		return Offset - PathOffset;
	}

	*IsEscaped = false;
	*Character = '\0';
	return 0;
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
	uint8_t Character;
	size_t CharacterLength;
	size_t PathOffset;
	bool IsEscaped;

	JsonStringClear(String);

	for (PathOffset = 0; (PathOffset < Path.Length); PathOffset = PathOffset + CharacterLength)
	{
		CharacterLength = JsonPathGetNextCharacter(Path, PathOffset, &IsEscaped, &Character);
		if (CharacterLength == 0)
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
	uint8_t PathCharacter;
	uint8_t StringCharacter;
	size_t PathCharacterLength;
	size_t StringCharacterLength;
	size_t StringOffset;
	size_t StringLength;
	size_t PathOffset;
	bool IsEscaped;

	StringLength = JsonStringGetLength(String);

	for (StringOffset = 0, PathOffset = 0; (StringOffset < StringLength) && (PathOffset < Path.Length); StringOffset = StringOffset + StringCharacterLength, PathOffset = PathOffset + PathCharacterLength)
	{
		PathCharacterLength = JsonPathGetNextCharacter(Path, PathOffset, &IsEscaped, &PathCharacter);
		if (PathCharacterLength == 0)
		{
			return false;
		}

		if (!IsEscaped && ((PathCharacter == '/') || (PathCharacter == ':') || (PathCharacter == '[') || (PathCharacter == ']')))
		{
			return false;
		}

		StringCharacterLength = JsonStringGetCharacter(String, StringOffset, &StringCharacter);
		if ((StringCharacterLength == 0) || (PathCharacter != StringCharacter))
		{
			return false;
		}
	}

	return (StringOffset == StringLength) && (PathOffset == Path.Length);
}


static size_t JsonPathSkipSpaceLeft(tJsonPath Path)
{
	uint8_t Character;
	size_t CharacterLength;
	size_t Offset;
	bool IsEscaped;

	for (Offset = 0; Offset < Path.Length; Offset = Offset + CharacterLength)
	{
		CharacterLength = JsonPathGetNextCharacter(Path, Offset, &IsEscaped, &Character);
		if ((CharacterLength == 0) || IsEscaped || (Character != ' '))
		{
			break;
		}
	}

	return Offset;
}


static size_t JsonPathSkipSpaceRight(tJsonPath Path)
{
	uint8_t Character;
	size_t CharacterLength;
	size_t Offset;
	bool IsEscaped;

	for (Offset = Path.Length; Offset > 0; Offset = Offset - CharacterLength)
	{
		CharacterLength = JsonPathGetPreviousCharacter(Path, Offset, &IsEscaped, &Character);
		if ((CharacterLength == 0) || IsEscaped || (Character != ' '))
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
	uint8_t Character;
	size_t CharacterLength;
	size_t Length;
	bool IsEscaped;

	for (Length = 0; Length < Path.Length; Length = Length + CharacterLength)
	{
		CharacterLength = JsonPathGetNextCharacter(Path, Length, &IsEscaped, &Character);
		if (CharacterLength == 0)
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
	uint8_t FirstCharacter;
	uint8_t LastCharacter;
	size_t FirstCharacterLength;
	size_t LastCharacterLength;
	bool FirstIsEscaped;
	bool LastIsEscaped;

	*UnquotedPath = Path;

	if (Path.Length == 0)
	{
		return true;
	}

	FirstCharacterLength = JsonPathGetNextCharacter(Path, 0, &FirstIsEscaped, &FirstCharacter);
	if (FirstCharacterLength == 0)
	{
		return false;
	}

	if (FirstCharacterLength == Path.Length)
	{
		return FirstCharacter != '"';
	}

	LastCharacterLength = JsonPathGetPreviousCharacter(Path, Path.Length, &LastIsEscaped, &LastCharacter);
	if (LastCharacterLength == 0)
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
			*UnquotedPath = JsonPathMiddle(Path, FirstCharacterLength, Path.Length - LastCharacterLength);
		}
		return (FirstCharacter == '"') ? (LastCharacter == '"') : (LastCharacter != '"');
	}
}


size_t JsonPathGetComponent(tJsonPath Path, tJsonType *ComponentType, tJsonPath *Component)
{
	tJsonPath Name;
	uint8_t Character;
	size_t CharacterLength;
	size_t Start;
	size_t Length;
	size_t Offset;
	size_t NestedCount;
	bool IsEscaped;

	Start = JsonPathSkipSpaceLeft(Path);

	CharacterLength = JsonPathGetNextCharacter(Path, Start, &IsEscaped, &Character);
	if (CharacterLength == 0)
	{
		return 0;
	}
	
	if (!IsEscaped)
	{
		if (Character == '/')
		{
			*ComponentType = json_TypeObject;
			*Component = JsonPathUtf8(NULL);
			Offset = Start + CharacterLength;
			return Offset + JsonPathSkipSpaceLeft(JsonPathRight(Path, Offset));
		}
	
		if (Character == '[')
		{
			NestedCount = 0;
			for (Offset = Start + CharacterLength; Offset < Path.Length; Offset = Offset + Length)
			{
				Length = JsonPathGetNextCharacter(Path, Offset, &IsEscaped, &Character);
				if (!IsEscaped)
				{
					if (Character == ']')
					{
						if (NestedCount == 0)
						{
							*ComponentType = json_TypeArray;
							*Component = JsonPathTrimSpaces(JsonPathMiddle(Path, Start + CharacterLength, Offset));
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
			Offset = Start + CharacterLength;

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
