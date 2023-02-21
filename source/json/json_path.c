#include "json_character.h"
#include "json_path.h"


static size_t JsonPathGetNextCharacter(const uint8_t *Path, size_t PathLength, size_t PathIndex, bool *IsEscaped, uint8_t *Character)
{
	size_t Index = PathIndex;

	if (Index < PathLength)
	{
		*Character = Path[Index];
		Index++;
		if (*Character != '\\')
		{
			*IsEscaped = false;
			return Index - PathIndex;
		}
		
		if (Index < PathLength)
		{
			*IsEscaped = true;
			*Character = Path[Index];
			*Character = JsonCharacterFromEscape(*Character);
			Index++;
			return Index - PathIndex;
		}
	}

	*IsEscaped = false;
	*Character = '\0';
	return 0;
}


static size_t JsonPathGetPreviousCharacter(const uint8_t *Path, size_t PathLength, size_t PathIndex, bool *IsEscaped, uint8_t *Character)
{
	size_t PreviousIndex;
	size_t Index = PathIndex;

	if ((Index > 0) && (Index <= PathLength))
	{
		Index--;
		*Character = Path[Index];
		if ((Index == 0) || (Path[Index - 1] != '\\'))
		{
			*IsEscaped = false;
		}
		else
		{
			*IsEscaped = true;
			for (PreviousIndex = Index - 1; (PreviousIndex > 0) && (Path[PreviousIndex - 1] == '\\'); PreviousIndex--)
			{
				*IsEscaped = !*IsEscaped;
			}
		}
	}

	if (*IsEscaped || (*Character != '\\'))
	{
		if (*IsEscaped)
		{
			*Character = JsonCharacterFromEscape(*Character);
		}
		
		return PathIndex - Index;
	}

	*IsEscaped = false;
	*Character = '\0';
	return 0;
}


bool JsonPathSetString(const uint8_t *Path, size_t PathLength, tJsonString *String)
{
	uint8_t Character;
	size_t CharacterLength;
	size_t PathIndex;
	bool IsEscaped;

	JsonStringClear(String);

	for (PathIndex = 0; (PathIndex < PathLength); PathIndex = PathIndex + CharacterLength)
	{
		CharacterLength = JsonPathGetNextCharacter(Path, PathLength, PathIndex, &IsEscaped, &Character);
		if (CharacterLength == 0)
		{
			return false;
		}

		if (!IsEscaped && ((Path[PathIndex] == '/') || (Path[PathIndex] == ':') || (Path[PathIndex] == '[') || (Path[PathIndex] == ']')))
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


bool JsonPathCompareString(const uint8_t *Path, size_t PathLength, tJsonString *String)
{
	uint8_t PathCharacter;
	uint8_t StringCharacter;
	size_t PathCharacterLength;
	size_t StringCharacterLength;
	size_t StringIndex;
	size_t StringLength;
	size_t PathIndex;
	bool IsEscaped;

	StringLength = JsonStringGetLength(String);

	for (StringIndex = 0, PathIndex = 0; (StringIndex < StringLength) && (PathIndex < PathLength); PathIndex = PathIndex + PathCharacterLength, StringIndex = StringIndex + StringCharacterLength)
	{
		PathCharacterLength = JsonPathGetNextCharacter(Path, PathLength, PathIndex, &IsEscaped, &PathCharacter);
		if (PathCharacterLength == 0)
		{
			return false;
		}

		if (!IsEscaped && ((PathCharacter == '/') || (PathCharacter == ':') || (PathCharacter == '[') || (PathCharacter == ']')))
		{
			return false;
		}

		StringCharacterLength = JsonStringGetCharacter(String, StringIndex, &StringCharacter);
		if ((StringCharacterLength == 0) || (PathCharacter != StringCharacter))
		{
			return false;
		}
	}

	return (StringIndex == StringLength) && (PathIndex == PathLength);
}


static size_t JsonPathTrimSpaces(const uint8_t *Path, size_t PathLength, const uint8_t **TrimmedPath, size_t *TrimmedPathLength)
{
	uint8_t Character;
	size_t CharacterLength;
	size_t Length;
	bool IsEscaped;

	Length = 0;

	for (;;)
	{
		CharacterLength = JsonPathGetNextCharacter(Path, PathLength, 0, &IsEscaped, &Character);
		if ((CharacterLength == 0) || IsEscaped || (Character != ' '))
		{
			break;
		}
		Path = Path + CharacterLength;
		PathLength = PathLength - CharacterLength;
		Length = Length + CharacterLength;
	}

	for (;;)
	{
		CharacterLength = JsonPathGetPreviousCharacter(Path, PathLength, PathLength, &IsEscaped, &Character);
		if ((CharacterLength == 0) || IsEscaped || (Character != ' '))
		{
			break;
		}
		PathLength = PathLength - CharacterLength;
		Length = Length + CharacterLength;
	}

	*TrimmedPath = Path;
	*TrimmedPathLength = PathLength;

	return Length;
}


static size_t JsonPathGetName(const uint8_t *Path, size_t PathLength, const uint8_t **Name, size_t *NameLength)
{
	uint8_t Character;
	size_t CharacterLength;
	size_t Length;
	bool IsEscaped;

	for (Length = 0; Length < PathLength; Length = Length + CharacterLength)
	{
		CharacterLength = JsonPathGetNextCharacter(Path, PathLength, Length, &IsEscaped, &Character);
		if (CharacterLength == 0)
		{
			return 0;
		}

		if (!IsEscaped && ((Path[Length] == '/') || (Path[Length] == ':') || (Path[Length] == '[') || (Path[Length] == ']')))
		{
			break;
		}
	}

	JsonPathTrimSpaces(Path, Length, Name, NameLength);

	return Length;
}


static bool JsonPathTrimQuotes(const uint8_t *Path, size_t PathLength, const uint8_t **UnquotedPath, size_t *UnquotedPathLength)
{
	uint8_t FirstCharacter;
	uint8_t LastCharacter;
	size_t FirstCharacterLength;
	size_t LastCharacterLength;
	bool FirstIsEscaped;
	bool LastIsEscaped;

	*UnquotedPath = Path;
	*UnquotedPathLength = PathLength;

	if (PathLength == 0)
	{
		return true;
	}

	FirstCharacterLength = JsonPathGetNextCharacter(Path, PathLength, 0, &FirstIsEscaped, &FirstCharacter);
	if (FirstCharacterLength == 0)
	{
		return false;
	}

	if (FirstCharacterLength == PathLength)
	{
		return FirstCharacter != '"';
	}

	LastCharacterLength = JsonPathGetPreviousCharacter(&Path[FirstCharacterLength], PathLength - FirstCharacterLength, PathLength - FirstCharacterLength, &LastIsEscaped, &LastCharacter);
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
			*UnquotedPath = &Path[FirstCharacterLength];
			*UnquotedPathLength = PathLength - FirstCharacterLength - LastCharacterLength;
		}
		return (FirstCharacter == '"') ? (LastCharacter == '"') : (LastCharacter != '"');
	}
}


size_t JsonPathGetComponent(const uint8_t *Path, size_t PathLength, tJsonType *ComponentType, const uint8_t **Component, size_t *ComponentLength)
{
	const uint8_t *Name;
	uint8_t Character;
	size_t CharacterLength;
	size_t NameLength;
	size_t Length;
	size_t PathIndex;
	size_t NestedCount;
	size_t SpaceCount;
	bool IsEscaped;

	SpaceCount = JsonPathTrimSpaces(Path, PathLength, &Path, &PathLength);

	CharacterLength = JsonPathGetNextCharacter(Path, PathLength, 0, &IsEscaped, &Character);
	if (CharacterLength == 0)
	{
		return 0;
	}
	
	if (!IsEscaped)
	{
		if (Character == '/')
		{
			*ComponentType = json_TypeObject;
			*Component = NULL;
			*ComponentLength = 0;
			return SpaceCount + CharacterLength;
		}
	
		if (Character == '[')
		{
			NestedCount = 0;
			for (PathIndex = CharacterLength; PathIndex < PathLength; PathIndex = PathIndex + Length)
			{
				Length = JsonPathGetNextCharacter(Path, PathLength, PathIndex, &IsEscaped, &Character);
				if (!IsEscaped)
				{
					if (Character == ']')
					{
						if (NestedCount == 0)
						{
							*ComponentType = json_TypeArray;
							JsonPathTrimSpaces(&Path[CharacterLength], PathIndex - CharacterLength, Component, ComponentLength);
							return SpaceCount + PathIndex + Length;
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
			Length = JsonPathGetName(&Path[CharacterLength], PathLength - CharacterLength, &Name, &NameLength);
			if ((Length == 0) || !JsonPathTrimQuotes(Name, NameLength, Component, ComponentLength))
			{
				return 0;
			}

			*ComponentType = (NameLength == *ComponentLength) ? json_TypeValueLiteral : json_TypeValueString;

			return SpaceCount + Length + CharacterLength;
		}
	}

	Length = JsonPathGetName(Path, PathLength, &Name, &NameLength);
	if ((Length == 0) || !JsonPathTrimQuotes(Name, NameLength, Component, ComponentLength))
	{
		return 0;
	}

	*ComponentType = json_TypeKey;

	return SpaceCount + Length;
}
