#include "json_character.h"
#include "json_path.h"


int JsonPathSetString(const uint8_t *Path, size_t PathLength, tJsonString *String)
{
    uint8_t Character;
    size_t PathIndex;
    int Escape;

    JsonStringClear(String);
    Escape = 0;

    for (PathIndex = 0; (PathIndex < PathLength); PathIndex++)
    {
        Character = Path[PathIndex];
        if (!Escape && (Character == '\\'))
        {
            Escape = !Escape;
        }
        else
        {
            if (Escape)
            {
                Character = JsonCharacterFromEscape(Character);
                Escape = !Escape;
            }
            else if ((Path[PathIndex] == '/') || (Path[PathIndex] == ':') || (Path[PathIndex] == '[') || (Path[PathIndex] == ']'))
            {
                return 0;
            }

            if (!JsonStringAddCharacter(String, Character))
            {
                return 0;
            }
        }
    }

    return !Escape;
}


int JsonPathCompareString(const uint8_t *Path, size_t PathLength, tJsonString *String)
{
    uint8_t Character;
    size_t StringIndex;
    size_t StringLength;
    size_t PathIndex;
    int Escape;

    StringLength = JsonStringGetLength(String);
    Escape = 0;

    for (StringIndex = 0, PathIndex = 0; (StringIndex < StringLength) && (PathIndex < PathLength); PathIndex++)
    {
        Character = Path[PathIndex];
        if (!Escape && (Character == '\\'))
        {
            Escape = !Escape;
        }
        else
        {
            if (Escape)
            {
                Character = JsonCharacterFromEscape(Character);
                Escape = !Escape;
            }
            else if ((Path[PathIndex] == '/') || (Path[PathIndex] == ':') || (Path[PathIndex] == '[') || (Path[PathIndex] == ']'))
            {
                return 0;
            }

            if (Character != JsonStringGetCharacter(String, StringIndex))
            {
                return 0;
            }

            StringIndex++;
        }
    }

    return !Escape && (StringIndex == StringLength) && (PathIndex == PathLength);
}


static size_t JsonPathTrimSpace(const uint8_t **Path, size_t *PathLength)
{
    const uint8_t *Value;
    size_t Length;
    size_t Count;

    if ((Path != NULL) && (PathLength != NULL))
    {
        Value = *Path;
        Length = *PathLength;

        while ((Length > 0) && (*Value == ' '))
        {
            Value++;
            Length--;
        }

        while ((Length > 0) && (Value[Length - 1] == ' '))
        {
            Length--;
        }

        Count = *PathLength - Length;
        *Path = Value;
        *PathLength = Length;
    }
    else
    {
        Count = 0;
    }

    return Count;
}


static size_t JsonPathGetName(const uint8_t *Path, size_t PathLength, const uint8_t **Name, size_t *NameLength)
{
    size_t Length;
    int Escape;

    Escape = 0;

    for (Length = 0; Length < PathLength; Length++)
    {
        if (Escape || (Path[Length] == '\\'))
        {
            Escape = !Escape;
        }
        else if ((Path[Length] == '/') || (Path[Length] == ':') || (Path[Length] == '[') || (Path[Length] == ']'))
        {
            break;
        }
    }

    if (Escape)
    {
        Length = 0;
    }
    else if ((Name != NULL) && (NameLength != NULL))
    {
        *Name = Path;
        *NameLength = Length;

        JsonPathTrimSpace(Name, NameLength);
    }

    return Length;
}


static int JsonPathNameCheckQuote(const uint8_t *Path, size_t PathLength, int *IsQuoted)
{
    int Escape;
    size_t Index;

    *IsQuoted = (PathLength >= 2) && (Path[0] == '"') && (Path[PathLength - 1] == '"');

    if (*IsQuoted)
    {
        Escape = 0;
        for (Index = PathLength - 1; (Index > 0) && (Path[Index - 1] == '\\'); Index--)
        {
            Escape = !Escape;
        }

        if (Escape)
        {
            return 0;
        }
    }
    else if (PathLength > 0)
    {
        if (*Path == '"')
        {
            return 0;
        }
        else if (Path[PathLength - 1] == '"')
        {
            Escape = 0;
            for (Index = PathLength - 1; (Index > 0) && (Path[Index - 1] == '\\'); Index--)
            {
                Escape = !Escape;
            }

            if (!Escape)
            {
                return 0;
            }
        }
    }

    return 1;
}


size_t JsonPathGetComponent(const uint8_t *Path, size_t PathLength, tJsonType *ComponentType, const uint8_t **Component, size_t *ComponentLength)
{
    const uint8_t *Name;
    size_t NameLength;
    size_t Length;
    size_t PathIndex;
    size_t NestedCount;
    size_t SpaceCount;
    int IsQuoted;

    SpaceCount = JsonPathTrimSpace(&Path, &PathLength);

    if (PathLength == 0)
    {
        return 0;
    }
    else if (*Path == '/')
    {
        *ComponentType = json_TypeObject;
        *Component = NULL;
        *ComponentLength = 0;
        return SpaceCount + 1;
    }
    else if (*Path == '[')
    {
        NestedCount = 0;
        for (PathIndex = 1; PathIndex < PathLength; PathIndex = PathIndex + Length)
        {
            if (Path[PathIndex] == ']')
            {
                if (NestedCount == 0)
                {
                    *ComponentType = json_TypeArray;
                    *Component = &Path[1];
                    *ComponentLength = PathIndex - 1;
                    JsonPathTrimSpace(Component, ComponentLength);
                    return SpaceCount + PathIndex + 1;
                }
                NestedCount--;
                Length = 1;
            }
            else if (Path[PathIndex] == '[')
            {
                NestedCount++;
                Length = 1;
            }
            else if ((Path[PathIndex] == '/') || (Path[PathIndex] == ':'))
            {
                Length = 1;
            }
            else
            {
                Length = JsonPathGetName(&Path[PathIndex], PathLength - PathIndex, NULL, NULL);
                if (Length == 0)
                {
                    return 0;
                }
            }
        }
        return 0;
    }
    else if (*Path == ':')
    {
        Length = JsonPathGetName(&Path[1], PathLength - 1, &Name, &NameLength);
        if ((Length == 0) || !JsonPathNameCheckQuote(Name, NameLength, &IsQuoted))
        {
            return 0;
        }

        *ComponentType = IsQuoted ? json_TypeValueString : json_TypeValueLiteral;
        *Component = IsQuoted ? &Name[1] : Name;
        *ComponentLength = IsQuoted ? NameLength - 2 : NameLength;

        return SpaceCount + Length + 1;
    }
    else
    {
        Length = JsonPathGetName(Path, PathLength, &Name, &NameLength);
        if ((Length == 0) || !JsonPathNameCheckQuote(Name, NameLength, &IsQuoted))
        {
            return 0;
        }

        *ComponentType = json_TypeKey;
        *Component = IsQuoted ? &Name[1] : Name;
        *ComponentLength = IsQuoted ? NameLength - 2 : NameLength;

        return SpaceCount + Length;
    }
}
