#include "json_character.h"
#include "test_json.h"


static bool TestJsonCharacterIsLiteral(void)
{
    unsigned int Character;
    bool IsLiteral;
    bool ok;

    for (ok = true, Character = 0; ok && (Character <= UINT8_MAX); Character++)
    {
        IsLiteral = JsonCharacterIsLiteral(Character);
        if (((Character >= '0') && (Character <= '9')) ||
            ((Character >= 'a') && (Character <= 'z')) ||
            ((Character >= 'A') && (Character <= 'Z')) ||
            (Character == '-') || (Character == '+') || (Character == '.'))
        {
            ok = IsLiteral;
        }
        else
        {
            ok = !IsLiteral;
        }
    }

    return ok;
}


static bool TestJsonCharacterIsWhitespace(void)
{
    unsigned int Character;
    bool IsWhitespace;
    bool ok;

    for (ok = true, Character = 0; ok && (Character <= UINT8_MAX); Character++)
    {
        IsWhitespace = JsonCharacterIsWhitespace(Character);
        if ((Character == ' ') || (Character == '\t') || (Character == '\r') || (Character == '\n'))
        {
            ok = IsWhitespace;
        }
        else
        {
            ok = !IsWhitespace;
        }
    }

    return ok;
}


static bool TestJsonCharacterIsEscapable(void)
{
    unsigned int Character;
    bool IsEscapable;
    bool ok;

    for (ok = true, Character = 0; ok && (Character <= UINT8_MAX); Character++)
    {
        IsEscapable = JsonCharacterIsEscapable(Character);
        if ((Character == '\b') || (Character == '\f') || (Character == '\t') || (Character == '\r') || (Character == '\n') || (Character == '"') || (Character == '\\'))
        {
            ok = IsEscapable;
        }
        else
        {
            ok = !IsEscapable;
        }
    }

    return ok;
}


static bool TestJsonCharacterToEscape(void)
{
    unsigned int Character;
    uint8_t EscapedCharacter;
    bool ok;

    for (ok = true, Character = 0; ok && (Character <= UINT8_MAX); Character++)
    {
        EscapedCharacter = JsonCharacterToEscape(Character);
        if (Character == '\b')
        {
            ok = (EscapedCharacter == 'b');
        }
        else if (Character == '\f')
        {
            ok = (EscapedCharacter == 'f');
        }
        else if (Character == '\t')
        {
            ok = (EscapedCharacter == 't');
        }
        else if (Character == '\r')
        {
            ok = (EscapedCharacter == 'r');
        }
        else if (Character == '\n')
        {
            ok = (EscapedCharacter = 'n');
        }
        else
        {
            ok = (EscapedCharacter == Character);
        }
    }

    return ok;
}


static bool TestJsonCharacterFromEscape(void)
{
    unsigned int Character;
    uint8_t UnescapedCharacter;
    bool ok;

    for (ok = true, Character = 0; ok && (Character <= UINT8_MAX); Character++)
    {
        UnescapedCharacter = JsonCharacterFromEscape(Character);
        if (Character == 'b')
        {
            ok = (UnescapedCharacter == '\b');
        }
        else if (Character == 'f')
        {
            ok = (UnescapedCharacter == '\f');
        }
        else if (Character == 't')
        {
            ok = (UnescapedCharacter == '\t');
        }
        else if (Character == 'r')
        {
            ok = (UnescapedCharacter == '\r');
        }
        else if (Character == 'n')
        {
            ok = (UnescapedCharacter = '\n');
        }
        else
        {
            ok = (UnescapedCharacter == Character);
        }
    }

    return ok;
}


static const tTestCase TestCaseJsonCharacter[] =
{
    { "JsonCharacterIsLiteral",    TestJsonCharacterIsLiteral    },
    { "JsonCharacterIsWhitespace", TestJsonCharacterIsWhitespace },
    { "JsonCharacterIsEscapable",  TestJsonCharacterIsEscapable  },
    { "JsonCharacterToEscape",     TestJsonCharacterToEscape     },
    { "JsonCharacterFromEscape",   TestJsonCharacterFromEscape   }
};


const tTest TestJsonCharacter =
{
    "JsonCharacter",
    TestCaseJsonCharacter,
    sizeof(TestCaseJsonCharacter) / sizeof(TestCaseJsonCharacter[0])
};
