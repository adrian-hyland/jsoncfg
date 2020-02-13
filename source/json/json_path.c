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


static size_t JsonPathGetName(const uint8_t *Path, size_t PathLength, const uint8_t **Name, size_t *NameLength)
{
    size_t Length;
    int Escape;

    Escape = 0;

    for (Length = 0; (PathLength > 0) && (*Path == ' '); Path++, PathLength--, Length++)
        ;

    if (Name != NULL)
    {
        *Name = Path;
    }
    if (NameLength != NULL)
    {
        *NameLength = 0;
    }

    for (; Length < PathLength; Length++)
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
    else if (NameLength != NULL)
    {
        for (*NameLength = Length; (*NameLength > 0) && (Path[*NameLength - 1] == ' '); *NameLength = *NameLength - 1)
            ;
    }

    return Length;
}


size_t JsonPathGetComponent(const uint8_t *Path, size_t PathLength, tJsonType *ComponentType, const uint8_t **Component, size_t *ComponentLength)
{
    const uint8_t *Name;
    size_t NameLength;
    size_t Length;
    size_t PathIndex;
    size_t NestedCount;

    if (PathLength == 0)
    {
        return 0;
    }
    else if (*Path == '/')
    {
        *ComponentType = json_TypeObject;
        *Component = NULL;
        *ComponentLength = 0;
        return 1;
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
                    return PathIndex + 1;
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
        if (Length == 0)
        {
            return 0;
        }

        if (*Name == '"')
        {
            if ((NameLength < 2) || (Name[NameLength - 1] != '"'))
            {
                return 0;
            }

            *ComponentType = json_TypeValueString;
            *Component = &Name[1];
            *ComponentLength = NameLength - 2;
        }
        else
        {
            *ComponentType = json_TypeValueLiteral;
            *Component = Name;
            *ComponentLength = NameLength;
        }

        return Length + 1;
    }
    else
    {
        *ComponentType = json_TypeKey;

        Length = JsonPathGetName(Path, PathLength, &Name, &NameLength);
        if (Length == 0)
        {
            return 0;
        }

        if (*Name == '"')
        {
            if ((NameLength < 2) || (Name[NameLength - 1] != '"'))
            {
                return 0;
            }

            *Component = &Name[1];
            *ComponentLength = NameLength - 2;
        }
        else
        {
            *Component = Name;
            *ComponentLength = NameLength;
        }

        return Length;
    }
}
