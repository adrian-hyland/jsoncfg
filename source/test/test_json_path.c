#include <string.h>
#include "json_utf16.h"
#include "json_path.h"
#include "test_json.h"


static bool TestJsonPathUtf8(void)
{
	tJsonPath Path;
	const tJsonUtf8Unit EmptyString[] = "";
	const tJsonUtf8Unit NonEmptyString[] = "a/b/c";
	bool ok;

	Path = JsonPathUtf8(NULL);

	ok = (Path.Value == NULL) && (Path.Length == 0);

	Path = JsonPathUtf8(EmptyString);

	ok = ok && (Path.Value == EmptyString) && (Path.Length == 0);

	Path = JsonPathUtf8(NonEmptyString);

	ok = ok && (Path.Value == NonEmptyString) && (Path.Length == strlen((const char *)NonEmptyString));

	return ok;
}


static bool TestJsonPathAscii(void)
{
	tJsonPath Path;
	const char EmptyString[] = "";
	const char NonEmptyString[] = "a/b/c";
	bool ok;

	Path = JsonPathAscii(NULL);

	ok = (Path.Value == NULL) && (Path.Length == 0);

	Path = JsonPathAscii(EmptyString);

	ok = ok && (Path.Value == (const tJsonUtf8Unit *)EmptyString) && (Path.Length == 0);

	Path = JsonPathAscii(NonEmptyString);

	ok = ok && (Path.Value == (const tJsonUtf8Unit *)NonEmptyString) && (Path.Length == strlen(NonEmptyString));

	return ok;
}


static bool TestJsonPathLeft(void)
{
	const tJsonUtf8Unit String[] = "0123456789";
	tJsonPath Path;
	size_t n;
	bool ok;

	Path = JsonPathUtf8(String);

	ok = (Path.Value == String) && (Path.Length == sizeof(String) - 1);

	Path = JsonPathLeft(Path, sizeof(String));

	ok = ok && (Path.Value == String) && (Path.Length == sizeof(String) - 1);

	for (n = 0; ok && (n < sizeof(String) - 1); n++)
	{
		Path = JsonPathLeft(Path, Path.Length - 1);

		ok = (Path.Value == String) && (Path.Length == sizeof(String) - n - 2);
	}

	Path = JsonPathLeft(Path, 1);

	ok = ok && (Path.Value == String) && (Path.Length == 0);

	return ok;
}


static bool TestJsonPathRight(void)
{
	const tJsonUtf8Unit String[] = "0123456789";
	tJsonPath Path;
	size_t n;
	bool ok;

	Path = JsonPathUtf8(String);

	ok = (Path.Value == String) && (Path.Length == sizeof(String) - 1);

	Path = JsonPathRight(Path, sizeof(String));

	ok = ok && (Path.Value == &String[sizeof(String) - 1]) && (Path.Length == 0);

	Path = JsonPathUtf8(String);

	for (n = 0; ok && (n < sizeof(String) - 1); n++)
	{
		Path = JsonPathRight(Path, 1);

		ok = (Path.Value == &String[n + 1]) && (Path.Length == sizeof(String) - n - 2);
	}

	Path = JsonPathRight(Path, 1);

	ok = ok && (Path.Value == &String[sizeof(String) - 1]) && (Path.Length == 0);

	return ok;
}


static bool TestJsonPathMiddle(void)
{
	const tJsonUtf8Unit String[] = "0123456789";
	tJsonPath Path;
	size_t n;
	bool ok;

	Path = JsonPathUtf8(String);

	ok = (Path.Value == String) && (Path.Length == sizeof(String) - 1);

	Path = JsonPathMiddle(Path, 0, sizeof(String));

	ok = (Path.Value == String) && (Path.Length == sizeof(String) - 1);

	for (n = 0; ok && (n < sizeof(String) / 2); n++)
	{
		Path = JsonPathMiddle(Path, 1, Path.Length - 1);

		ok = (Path.Value == &String[n + 1]) && (Path.Length == sizeof(String) - n * 2 - 3);
	}

	Path = JsonPathMiddle(Path, 1, 2);

	ok = ok && (Path.Value == &String[5]) && (Path.Length == 0);

	Path = JsonPathUtf8(String);

	Path = JsonPathMiddle(Path, Path.Length, 0);

	ok = ok && (Path.Value == &String[sizeof(String) - 1]) && (Path.Length == 0);

	return ok;
}


static bool TestJsonPathGetNextUtf8Code(void)
{
	const tJsonUtf8Unit Escaped[] = "\\b\\f\\n\\r\\t\\\\\\\"";
	const tJsonUtf8Unit Unescaped[] = "\b\f\n\r\t\\\"";
	const tJsonUtf8Unit Invalid[] = "\\";
	tJsonUtf8Unit Valid[JSON_UTF8_MAX_SIZE + 1];
	tJsonUtf8Unit EscapedUnicode[6 + 6 + 1 + 1];
	tJsonPath Path;
	tJsonCharacter Character;
	tJsonUtf8Code Utf8Code;
	tJsonUtf16Code Utf16Code;
	size_t Offset;
	size_t Length;
	bool IsEscaped;
	size_t n;
	bool ok = true;

	for (Character = 0x20; ok && (Character < 0xD800); Character++)
	{
		if (Character != '\\')
		{
			Utf8Code = JsonUtf8Code(Character);
			Length = JsonUtf8CodeGetUnitLength(Utf8Code);
			for (n = 0; ok && (n < Length); n++)
			{
				Valid[n] = JsonUtf8CodeGetUnit(Utf8Code, n);
			}
			Valid[n] = '\0';
			Length = JsonPathGetNextUtf8Code(JsonPathUtf8(Valid), 0, &IsEscaped, &Utf8Code);
			ok = (Length != 0) && !IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == Character);
		}
	}

	for (Character = 0xE000; ok && (Character < 0x110000); Character++)
	{
		Utf8Code = JsonUtf8Code(Character);
		Length = JsonUtf8CodeGetUnitLength(Utf8Code);
		for (n = 0; ok && (n < Length); n++)
		{
			Valid[n] = JsonUtf8CodeGetUnit(Utf8Code, n);
		}
		Valid[n] = '\0';
		Length = JsonPathGetNextUtf8Code(JsonPathUtf8(Valid), 0, &IsEscaped, &Utf8Code);
		ok = (Length != 0) && !IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == Character);
	}

	Path = JsonPathUtf8(Escaped);
	for (Offset = 0, n = 0; ok && (n < sizeof(Unescaped) - 1); Offset = Offset + Length, n++)
	{
		Length = JsonPathGetNextUtf8Code(Path, Offset, &IsEscaped, &Utf8Code);
		ok = (Length != 0) && IsEscaped && (Utf8Code == Unescaped[n]);
	}
	ok = ok && (JsonPathGetNextUtf8Code(Path, Offset, &IsEscaped, &Utf8Code) == 0);

	Path = JsonPathUtf8(Invalid);
	ok = ok && (JsonPathGetNextUtf8Code(Path, 0, &IsEscaped, &Utf8Code) == 0);

	for (Character = 0; ok && (Character < 0xD800); Character++)
	{
		Utf16Code = JsonUtf16Code(Character);
		EscapedUnicode[0] = '\\';
		EscapedUnicode[1] = '\0';
		ok = (JsonPathGetNextUtf8Code(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &Utf8Code) == 0);

		EscapedUnicode[1] = 'u';
		EscapedUnicode[2] = '\0';
		ok = ok && (JsonPathGetNextUtf8Code(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &Utf8Code) == 0);

		EscapedUnicode[2] = JsonCharacterFromHexDigit((Utf16Code >> 12) & 0x0F);
		EscapedUnicode[3] = '\0';
		ok = ok && (JsonPathGetNextUtf8Code(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &Utf8Code) == 0);

		EscapedUnicode[3] = JsonCharacterFromHexDigit((Utf16Code >> 8) & 0x0F);
		EscapedUnicode[4] = '\0';
		ok = ok && (JsonPathGetNextUtf8Code(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &Utf8Code) == 0);

		EscapedUnicode[4] = JsonCharacterFromHexDigit((Utf16Code >> 4) & 0x0F);
		EscapedUnicode[5] = '\0';
		ok = ok && (JsonPathGetNextUtf8Code(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &Utf8Code) == 0);

		EscapedUnicode[5] = JsonCharacterFromHexDigit(Utf16Code & 0x0F);
		EscapedUnicode[6] = '\0';
		Length = JsonPathGetNextUtf8Code(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &Utf8Code);
		ok = ok && (Length == strlen((char *)EscapedUnicode)) && IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == Character);

		EscapedUnicode[6] = '0';
		EscapedUnicode[7] = '\0';
		Length = JsonPathGetNextUtf8Code(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &Utf8Code);
		ok = ok && (Length == strlen((char *)EscapedUnicode) - 1) && IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == Character);
	}

	for (Character = 0xD800; ok && (Character < 0xE000); Character++)
	{
		Utf16Code = Character;
		EscapedUnicode[0] = '\\';
		EscapedUnicode[1] = '\0';
		ok = (JsonPathGetNextUtf8Code(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &Utf8Code) == 0);

		EscapedUnicode[1] = 'u';
		EscapedUnicode[2] = '\0';
		ok = ok && (JsonPathGetNextUtf8Code(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &Utf8Code) == 0);

		EscapedUnicode[2] = JsonCharacterFromHexDigit((Utf16Code >> 12) & 0x0F);
		EscapedUnicode[3] = '\0';
		ok = ok && (JsonPathGetNextUtf8Code(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &Utf8Code) == 0);

		EscapedUnicode[3] = JsonCharacterFromHexDigit((Utf16Code >> 8) & 0x0F);
		EscapedUnicode[4] = '\0';
		ok = ok && (JsonPathGetNextUtf8Code(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &Utf8Code) == 0);

		EscapedUnicode[4] = JsonCharacterFromHexDigit((Utf16Code >> 4) & 0x0F);
		EscapedUnicode[5] = '\0';
		ok = ok && (JsonPathGetNextUtf8Code(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &Utf8Code) == 0);

		EscapedUnicode[5] = JsonCharacterFromHexDigit(Utf16Code & 0x0F);
		EscapedUnicode[6] = '\0';
		ok = ok && (JsonPathGetNextUtf8Code(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &Utf8Code) == 0);

		EscapedUnicode[6] = '0';
		EscapedUnicode[7] = '\0';
		ok = ok && (JsonPathGetNextUtf8Code(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &Utf8Code) == 0);
	}

	for (Character = 0xE000; ok && (Character < 0x10000); Character++)
	{
		Utf16Code = JsonUtf16Code(Character);
		EscapedUnicode[0] = '\\';
		EscapedUnicode[1] = '\0';
		ok = (JsonPathGetNextUtf8Code(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &Utf8Code) == 0);

		EscapedUnicode[1] = 'u';
		EscapedUnicode[2] = '\0';
		ok = ok && (JsonPathGetNextUtf8Code(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &Utf8Code) == 0);

		EscapedUnicode[2] = JsonCharacterFromHexDigit((Utf16Code >> 12) & 0x0F);
		EscapedUnicode[3] = '\0';
		ok = ok && (JsonPathGetNextUtf8Code(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &Utf8Code) == 0);

		EscapedUnicode[3] = JsonCharacterFromHexDigit((Utf16Code >> 8) & 0x0F);
		EscapedUnicode[4] = '\0';
		ok = ok && (JsonPathGetNextUtf8Code(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &Utf8Code) == 0);

		EscapedUnicode[4] = JsonCharacterFromHexDigit((Utf16Code >> 4) & 0x0F);
		EscapedUnicode[5] = '\0';
		ok = ok && (JsonPathGetNextUtf8Code(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &Utf8Code) == 0);

		EscapedUnicode[5] = JsonCharacterFromHexDigit(Utf16Code & 0x0F);
		EscapedUnicode[6] = '\0';
		Length = JsonPathGetNextUtf8Code(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &Utf8Code);
		ok = ok && (Length = strlen((char *)EscapedUnicode)) && IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == Character);

		EscapedUnicode[6] = '0';
		EscapedUnicode[7] = '\0';
		Length = JsonPathGetNextUtf8Code(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &Utf8Code);
		ok = ok && (Length == strlen((char *)EscapedUnicode) - 1) && IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == Character);
	}

	for (Character = 0x10000; ok && (Character < 0x110000); Character++)
	{
		Utf16Code = JsonUtf16Code(Character);
		EscapedUnicode[0] = '\\';
		EscapedUnicode[1] = 'u';
		EscapedUnicode[2] = JsonCharacterFromHexDigit((Utf16Code >> 28) & 0x0F);
		EscapedUnicode[3] = JsonCharacterFromHexDigit((Utf16Code >> 24) & 0x0F);
		EscapedUnicode[4] = JsonCharacterFromHexDigit((Utf16Code >> 20) & 0x0F);
		EscapedUnicode[5] = JsonCharacterFromHexDigit((Utf16Code >> 16) & 0x0F);
		EscapedUnicode[6] = '\\';
		EscapedUnicode[7] = '\0';
		ok = (JsonPathGetNextUtf8Code(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &Utf8Code) == 0);

		EscapedUnicode[7] = 'u';
		EscapedUnicode[8] = '\0';
		ok = ok && (JsonPathGetNextUtf8Code(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &Utf8Code) == 0);

		EscapedUnicode[8] = JsonCharacterFromHexDigit((Utf16Code >> 12) & 0x0F);
		EscapedUnicode[9] = '\0';
		ok = ok && (JsonPathGetNextUtf8Code(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &Utf8Code) == 0);

		EscapedUnicode[9] = JsonCharacterFromHexDigit((Utf16Code >> 8) & 0x0F);
		EscapedUnicode[10] = '\0';
		ok = ok && (JsonPathGetNextUtf8Code(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &Utf8Code) == 0);

		EscapedUnicode[10] = JsonCharacterFromHexDigit((Utf16Code >> 4) & 0x0F);
		EscapedUnicode[11] = '\0';
		ok = ok && (JsonPathGetNextUtf8Code(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &Utf8Code) == 0);

		EscapedUnicode[11] = JsonCharacterFromHexDigit(Utf16Code & 0x0F);
		EscapedUnicode[12] = '\0';
		Length = JsonPathGetNextUtf8Code(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &Utf8Code);
		ok = ok && (Length == strlen((char *)EscapedUnicode)) && IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == Character);

		EscapedUnicode[12] = '0';
		EscapedUnicode[13] = '\0';
		Length = JsonPathGetNextUtf8Code(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &Utf8Code);
		ok = ok && (Length == strlen((char *)EscapedUnicode) - 1) && IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == Character);
	}

	return ok;
}


static bool TestJsonPathGetPreviousUtf8Code(void)
{
	const tJsonUtf8Unit Escaped[] = "\\b\\f\\n\\r\\t\\\\\\\"";
	const tJsonUtf8Unit Unescaped[] = "\b\f\n\r\t\\\"";
	const tJsonUtf8Unit Invalid[] = "\\";
	tJsonUtf8Unit Valid[JSON_UTF8_MAX_SIZE + 1];
	tJsonUtf8Unit EscapedUnicode[6 + 6 + 1 + 1];
	tJsonPath Path;
	tJsonCharacter Character;
	tJsonUtf8Code Utf8Code;
	tJsonUtf16Code Utf16Code;
	size_t Offset;
	size_t Length;
	bool IsEscaped;
	size_t n;
	bool ok = true;

	for (Character = 0x20; ok && (Character < 0xD800); Character++)
	{
		if (Character != '\\')
		{
			Utf8Code = JsonUtf8Code(Character);
			Length = JsonUtf8CodeGetUnitLength(Utf8Code);
			for (n = 0; ok && (n < Length); n++)
			{
				Valid[n] = JsonUtf8CodeGetUnit(Utf8Code, n);
			}
			Valid[n] = '\0';
			Length = JsonPathGetPreviousUtf8Code(JsonPathUtf8(Valid), Length, &IsEscaped, &Utf8Code);
			ok = (Length != 0) && !IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == Character);
		}
	}

	for (Character = 0xE000; ok && (Character < 0x110000); Character++)
	{
		Utf8Code = JsonUtf8Code(Character);
		Length = JsonUtf8CodeGetUnitLength(Utf8Code);
		for (n = 0; ok && (n < Length); n++)
		{
			Valid[n] = JsonUtf8CodeGetUnit(Utf8Code, n);
		}
		Valid[n] = '\0';
		Length = JsonPathGetPreviousUtf8Code(JsonPathUtf8(Valid), Length, &IsEscaped, &Utf8Code);
		ok = (Length != 0) && !IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == Character);
	}

	Path = JsonPathUtf8(Escaped);
	for (Offset = Path.Length, n = sizeof(Unescaped) - 1; ok && (n > 0); Offset = Offset - Length, n--)
	{
		Length = JsonPathGetPreviousUtf8Code(Path, Offset, &IsEscaped, &Utf8Code);
		ok = (Length != 0) && IsEscaped && (Utf8Code == Unescaped[n - 1]);
	}
	ok = ok && (JsonPathGetPreviousUtf8Code(Path, Offset, &IsEscaped, &Utf8Code) == 0);

	Path = JsonPathUtf8(Invalid);
	ok = ok && (JsonPathGetPreviousUtf8Code(Path, Path.Length, &IsEscaped, &Utf8Code) == 0);

	for (Character = 0; ok && (Character < 0xD800); Character++)
	{
		Utf16Code = JsonUtf16Code(Character);
		EscapedUnicode[0] = '\\';
		EscapedUnicode[1] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		ok = (JsonPathGetPreviousUtf8Code(Path, Path.Length, &IsEscaped, &Utf8Code) == 0);

		EscapedUnicode[1] = 'u';
		EscapedUnicode[2] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		ok = ok && (JsonPathGetPreviousUtf8Code(Path, Path.Length, &IsEscaped, &Utf8Code) == 0);

		EscapedUnicode[2] = JsonCharacterFromHexDigit((Utf16Code >> 12) & 0x0F);
		EscapedUnicode[3] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousUtf8Code(Path, Path.Length, &IsEscaped, &Utf8Code);
		ok = ok && (Length == 1) && !IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == EscapedUnicode[2]);

		EscapedUnicode[3] = JsonCharacterFromHexDigit((Utf16Code >> 8) & 0x0F);
		EscapedUnicode[4] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousUtf8Code(Path, Path.Length, &IsEscaped, &Utf8Code);
		ok = ok && (Length == 1) && !IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == EscapedUnicode[3]);

		EscapedUnicode[4] = JsonCharacterFromHexDigit((Utf16Code >> 4) & 0x0F);
		EscapedUnicode[5] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousUtf8Code(Path, Path.Length, &IsEscaped, &Utf8Code);
		ok = ok && (Length == 1) && !IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == EscapedUnicode[4]);

		EscapedUnicode[5] = JsonCharacterFromHexDigit(Utf16Code & 0x0F);
		EscapedUnicode[6] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousUtf8Code(Path, Path.Length, &IsEscaped, &Utf8Code);
		ok = ok && (Length == Path.Length) && IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == Character);

		EscapedUnicode[6] = '0';
		EscapedUnicode[7] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousUtf8Code(Path, Path.Length, &IsEscaped, &Utf8Code);
		ok = ok && (Length == 1) && !IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == EscapedUnicode[6]);
	}

	for (Character = 0xD800; ok && (Character < 0xE000); Character++)
	{
		Utf16Code = Character;
		EscapedUnicode[0] = '\\';
		EscapedUnicode[1] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		ok = (JsonPathGetPreviousUtf8Code(Path, Path.Length, &IsEscaped, &Utf8Code) == 0);

		EscapedUnicode[1] = 'u';
		EscapedUnicode[2] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		ok = ok && (JsonPathGetPreviousUtf8Code(Path, Path.Length, &IsEscaped, &Utf8Code) == 0);

		EscapedUnicode[2] = JsonCharacterFromHexDigit((Utf16Code >> 12) & 0x0F);
		EscapedUnicode[3] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousUtf8Code(Path, Path.Length, &IsEscaped, &Utf8Code);
		ok = ok && (Length == 1) && !IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == EscapedUnicode[2]);

		EscapedUnicode[3] = JsonCharacterFromHexDigit((Utf16Code >> 8) & 0x0F);
		EscapedUnicode[4] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousUtf8Code(Path, Path.Length, &IsEscaped, &Utf8Code);
		ok = ok && (Length == 1) && !IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == EscapedUnicode[3]);

		EscapedUnicode[4] = JsonCharacterFromHexDigit((Utf16Code >> 4) & 0x0F);
		EscapedUnicode[5] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousUtf8Code(Path, Path.Length, &IsEscaped, &Utf8Code);
		ok = ok && (Length == 1) && !IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == EscapedUnicode[4]);

		EscapedUnicode[5] = JsonCharacterFromHexDigit(Utf16Code & 0x0F);
		EscapedUnicode[6] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousUtf8Code(Path, Path.Length, &IsEscaped, &Utf8Code);
		ok = ok && (Length == 1) && !IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == EscapedUnicode[5]);

		EscapedUnicode[6] = '0';
		EscapedUnicode[7] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousUtf8Code(Path, Path.Length, &IsEscaped, &Utf8Code);
		ok = ok && (Length == 1) && !IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == EscapedUnicode[6]);
	}

	for (Character = 0xE000; ok && (Character < 0x10000); Character++)
	{
		Utf16Code = JsonUtf16Code(Character);
		EscapedUnicode[0] = '\\';
		EscapedUnicode[1] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		ok = (JsonPathGetPreviousUtf8Code(Path, Path.Length, &IsEscaped, &Utf8Code) == 0);

		EscapedUnicode[1] = 'u';
		EscapedUnicode[2] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		ok = ok && (JsonPathGetPreviousUtf8Code(Path, Path.Length, &IsEscaped, &Utf8Code) == 0);

		EscapedUnicode[2] = JsonCharacterFromHexDigit((Utf16Code >> 12) & 0x0F);
		EscapedUnicode[3] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousUtf8Code(Path, Path.Length, &IsEscaped, &Utf8Code);
		ok = ok && (Length == 1) && !IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == EscapedUnicode[2]);

		EscapedUnicode[3] = JsonCharacterFromHexDigit((Utf16Code >> 8) & 0x0F);
		EscapedUnicode[4] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousUtf8Code(Path, Path.Length, &IsEscaped, &Utf8Code);
		ok = ok && (Length == 1) && !IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == EscapedUnicode[3]);

		EscapedUnicode[4] = JsonCharacterFromHexDigit((Utf16Code >> 4) & 0x0F);
		EscapedUnicode[5] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousUtf8Code(Path, Path.Length, &IsEscaped, &Utf8Code);
		ok = ok && (Length == 1) && !IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == EscapedUnicode[4]);

		EscapedUnicode[5] = JsonCharacterFromHexDigit(Utf16Code & 0x0F);
		EscapedUnicode[6] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousUtf8Code(Path, Path.Length, &IsEscaped, &Utf8Code);
		ok = ok && (Length = Path.Length) && IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == Character);

		EscapedUnicode[6] = '0';
		EscapedUnicode[7] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousUtf8Code(Path, Path.Length, &IsEscaped, &Utf8Code);
		ok = ok && (Length == 1) && !IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == EscapedUnicode[6]);
	}

	for (Character = 0x10000; ok && (Character < 0x110000); Character++)
	{
		Utf16Code = JsonUtf16Code(Character);
		EscapedUnicode[0] = '\\';
		EscapedUnicode[1] = 'u';
		EscapedUnicode[2] = JsonCharacterFromHexDigit((Utf16Code >> 28) & 0x0F);
		EscapedUnicode[3] = JsonCharacterFromHexDigit((Utf16Code >> 24) & 0x0F);
		EscapedUnicode[4] = JsonCharacterFromHexDigit((Utf16Code >> 20) & 0x0F);
		EscapedUnicode[5] = JsonCharacterFromHexDigit((Utf16Code >> 16) & 0x0F);
		EscapedUnicode[6] = '\\';
		EscapedUnicode[7] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		ok = (JsonPathGetPreviousUtf8Code(Path, Path.Length, &IsEscaped, &Utf8Code) == 0);

		EscapedUnicode[7] = 'u';
		EscapedUnicode[8] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		ok = ok && (JsonPathGetPreviousUtf8Code(Path, Path.Length, &IsEscaped, &Utf8Code) == 0);

		EscapedUnicode[8] = JsonCharacterFromHexDigit((Utf16Code >> 12) & 0x0F);
		EscapedUnicode[9] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousUtf8Code(Path, Path.Length, &IsEscaped, &Utf8Code);
		ok = ok && (Length == 1) && !IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == EscapedUnicode[8]);

		EscapedUnicode[9] = JsonCharacterFromHexDigit((Utf16Code >> 8) & 0x0F);
		EscapedUnicode[10] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousUtf8Code(Path, Path.Length, &IsEscaped, &Utf8Code);
		ok = ok && (Length == 1) && !IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == EscapedUnicode[9]);

		EscapedUnicode[10] = JsonCharacterFromHexDigit((Utf16Code >> 4) & 0x0F);
		EscapedUnicode[11] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousUtf8Code(Path, Path.Length, &IsEscaped, &Utf8Code);
		ok = ok && (Length == 1) && !IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == EscapedUnicode[10]);

		EscapedUnicode[11] = JsonCharacterFromHexDigit(Utf16Code & 0x0F);
		EscapedUnicode[12] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousUtf8Code(Path, Path.Length, &IsEscaped, &Utf8Code);
		ok = ok && (Length == Path.Length) && IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == Character);

		EscapedUnicode[12] = '0';
		EscapedUnicode[13] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousUtf8Code(Path, Path.Length, &IsEscaped, &Utf8Code);
		ok = ok && (Length == 1) && !IsEscaped && (JsonUtf8CodeGetCharacter(Utf8Code) == EscapedUnicode[12]);
	}

	return ok;
}


static bool TestJsonPathSetString(void)
{
	static const char *ValidPaths[] =
	{
		"path value",
		"escaped \\:\\/\\[\\]\\\\\\uD83D\\uDE00 path",
	};
	static const char *InvalidPaths[] =
	{
		"path 1/path 2",
		"path 1:path 2",
		"path 1[path 2",
		"path 1]path 2",
		"path 1\\"
	};
	tJsonString String;
	tJsonPath Path;
	tJsonUtf8Code StringCode;
	tJsonUtf8Code PathCode;
	size_t StringOffset;
	size_t PathOffset;
	size_t StringCodeLength;
	size_t PathCodeLength;
	size_t n;
	bool IsEscaped;
	bool ok;

	JsonStringSetUp(&String);

	for (ok = true, n = 0; ok && (n < sizeof(ValidPaths) / sizeof(ValidPaths[0])); n++)
	{
		Path = JsonPathAscii(ValidPaths[n]);

		ok = JsonPathSetString(Path, &String);

		for (StringOffset = 0, PathOffset = 0; ok && (PathOffset < Path.Length); StringOffset = StringOffset + StringCodeLength, PathOffset = PathOffset + PathCodeLength)
		{
			PathCodeLength = JsonPathGetNextUtf8Code(Path, PathOffset, &IsEscaped, &PathCode);
			StringCodeLength = JsonStringGetNextUtf8Code(&String, StringOffset, &StringCode);

			ok = (PathCodeLength != 0) && (StringCodeLength != 0);
			ok = ok && (StringCode == PathCode);
		}
	}

	for (n = 0; ok && (n < sizeof(InvalidPaths) / sizeof(InvalidPaths[0])); n++)
	{
		ok = !JsonPathSetString(JsonPathAscii(InvalidPaths[n]), &String);
	}

	JsonStringCleanUp(&String);

	return ok;
}


static bool TestJsonPathCompareString(void)
{
	static const char *Paths[] =
	{
		"path value",
		"escaped \\:\\/\\[\\]\\\\\\uD83D\\uDE00 path",
	};
	tJsonString String;
	size_t PathLength;
	size_t n;
	bool ok;

	JsonStringSetUp(&String);

	for (ok = true, n = 0; ok && (n < sizeof(Paths) / sizeof(Paths[0])); n++)
	{
		PathLength = strlen(Paths[n]) - 1;

		ok = JsonPathSetString(JsonPathLeft(JsonPathAscii(Paths[n]), PathLength), &String);

		ok = ok && !JsonPathCompareString(JsonPathLeft(JsonPathAscii(Paths[n]), PathLength - 1), &String);

		ok = ok && JsonPathCompareString(JsonPathLeft(JsonPathAscii(Paths[n]), PathLength), &String);

		ok = ok && !JsonPathCompareString(JsonPathLeft(JsonPathAscii(Paths[n]), PathLength + 1), &String);
	}

	JsonStringCleanUp(&String);

	return ok;
}


static bool TestJsonPathGetComponent(void)
{
	static const struct
	{
		const char *Path;
		size_t      Length;
		tJsonType   Type;
		const char *Component;
	} TestPaths[] =
	{
		{ "/",                                        1,  json_TypeObject,       NULL                                   },
		{ "/object",                                  1,  json_TypeObject,       NULL                                   },
		{ "  /",                                      3,  json_TypeObject,       NULL                                   },
		{ "/  ",                                      3,  json_TypeObject,       NULL                                   },
		{ "  /  ",                                    5,  json_TypeObject,       NULL                                   },
		{ "  /object",                                3,  json_TypeObject,       NULL                                   },
		{ "  /object  ",                              3,  json_TypeObject,       NULL                                   },
		{ "/  object",                                3,  json_TypeObject,       NULL                                   },
		{ "/  object  ",                              3,  json_TypeObject,       NULL                                   },
		{ "  /  object",                              5,  json_TypeObject,       NULL                                   },
		{ "  /  object  ",                            5,  json_TypeObject,       NULL                                   },
		{ "key",                                      3,  json_TypeKey,          "key"                                  },
		{ "key/object",                               3,  json_TypeKey,          "key"                                  },
		{ "key[array]",                               3,  json_TypeKey,          "key"                                  },
		{ "key:value",                                3,  json_TypeKey,          "key"                                  },
		{ "key\\/\\:\\[\\]\\\\\\uD83D\\uDE00",        25,  json_TypeKey,         "key\\/\\:\\[\\]\\\\\\uD83D\\uDE00"    },
		{ "  key",                                    5,  json_TypeKey,          "key"                                  },
		{ "key  ",                                    5,  json_TypeKey,          "key"                                  },
		{ "  key  ",                                  7,  json_TypeKey,          "key"                                  },
		{ "  key/object",                             5,  json_TypeKey,          "key"                                  },
		{ "  key/object  ",                           5,  json_TypeKey,          "key"                                  },
		{ "key  /object",                             5,  json_TypeKey,          "key"                                  },
		{ "key  /object  ",                           5,  json_TypeKey,          "key"                                  },
		{ "  key  /object",                           7,  json_TypeKey,          "key"                                  },
		{ "  key  /object  ",                         7,  json_TypeKey,          "key"                                  },
		{ "key  [array]",                             5,  json_TypeKey,          "key"                                  },
		{ "key  [array]  ",                           5,  json_TypeKey,          "key"                                  },
		{ "  key[array]",                             5,  json_TypeKey,          "key"                                  },
		{ "  key[array]  ",                           5,  json_TypeKey,          "key"                                  },
		{ "  key  [array]",                           7,  json_TypeKey,          "key"                                  },
		{ "  key  [array]  ",                         7,  json_TypeKey,          "key"                                  },
		{ "  key:value",                              5,  json_TypeKey,          "key"                                  },
		{ "  key:value  ",                            5,  json_TypeKey,          "key"                                  },
		{ "key  :value",                              5,  json_TypeKey,          "key"                                  },
		{ "key  :value  ",                            5,  json_TypeKey,          "key"                                  },
		{ "  key  :value",                            7,  json_TypeKey,          "key"                                  },
		{ "  key  :value  ",                          7,  json_TypeKey,          "key"                                  },
		{ "\"key\"",                                  5,  json_TypeKey,          "key"                                  },
		{ "\"key\"/object",                           5,  json_TypeKey,          "key"                                  },
		{ "\"key\"/object  ",                         5,  json_TypeKey,          "key"                                  },
		{ "\"key\"[array]",                           5,  json_TypeKey,          "key"                                  },
		{ "\"key\"[array]  ",                         5,  json_TypeKey,          "key"                                  },
		{ "\"key\":value",                            5,  json_TypeKey,          "key"                                  },
		{ "\"key\":value  ",                          5,  json_TypeKey,          "key"                                  },
		{ "\"key\\/\\:\\[\\]\\\\\\uD83D\\uDE00\"",    27, json_TypeKey,          "key\\/\\:\\[\\]\\\\\\uD83D\\uDE00"    },
		{ "  \"key\"",                                7,  json_TypeKey,          "key"                                  },
		{ "\"key\"  ",                                7,  json_TypeKey,          "key"                                  },
		{ "  \"key\"  ",                              9,  json_TypeKey,          "key"                                  },
		{ "  \"key\"/object",                         7,  json_TypeKey,          "key"                                  },
		{ "  \"key\"/object  ",                       7,  json_TypeKey,          "key"                                  },
		{ "\"key\"  /object",                         7,  json_TypeKey,          "key"                                  },
		{ "\"key\"  /object  ",                       7,  json_TypeKey,          "key"                                  },
		{ "  \"key\"  /object",                       9,  json_TypeKey,          "key"                                  },
		{ "  \"key\"  /object  ",                     9,  json_TypeKey,          "key"                                  },
		{ "  \"key\"[array]",                         7,  json_TypeKey,          "key"                                  },
		{ "  \"key\"[array]  ",                       7,  json_TypeKey,          "key"                                  },
		{ "\"key\"  [array]",                         7,  json_TypeKey,          "key"                                  },
		{ "\"key\"  [array]  ",                       7,  json_TypeKey,          "key"                                  },
		{ "  \"key\"  [array]",                       9,  json_TypeKey,          "key"                                  },
		{ "  \"key\"  [array]  ",                     9,  json_TypeKey,          "key"                                  },
		{ "  \"key\":value",                          7,  json_TypeKey,          "key"                                  },
		{ "  \"key\":value  ",                        7,  json_TypeKey,          "key"                                  },
		{ "\"key\"  :value",                          7,  json_TypeKey,          "key"                                  },
		{ "\"key\"  :value  ",                        7,  json_TypeKey,          "key"                                  },
		{ "  \"key\"  :value",                        9,  json_TypeKey,          "key"                                  },
		{ "  \"key\"  :value  ",                      9,  json_TypeKey,          "key"                                  },
		{ ":value",                                   6,  json_TypeValueLiteral, "value"                                },
		{ ":value\\/\\:\\[\\]\\\\\\uD83D\\uDE00",     28, json_TypeValueLiteral, "value\\/\\:\\[\\]\\\\\\uD83D\\uDE00"  },
		{ ":  value",                                 8,  json_TypeValueLiteral, "value"                                },
		{ ":value  ",                                 8,  json_TypeValueLiteral, "value"                                },
		{ ":  value  ",                               10, json_TypeValueLiteral, "value"                                },
		{ "  :value",                                 8,  json_TypeValueLiteral, "value"                                },
		{ "  :value  ",                               10, json_TypeValueLiteral, "value"                                },
		{ "  :  value",                               10, json_TypeValueLiteral, "value"                                },
		{ "  :  value  ",                             12, json_TypeValueLiteral, "value"                                },
		{ ":\"value\"",                               8,  json_TypeValueString,  "value"                                },
		{ ":\"value\\/\\:\\[\\]\\\\\\uD83D\\uDE00\"", 30, json_TypeValueString,  "value\\/\\:\\[\\]\\\\\\uD83D\\uDE00"  },
		{ ":  \"value\"",                             10, json_TypeValueString,  "value"                                },
		{ ":\"value\"  ",                             10, json_TypeValueString,  "value"                                },
		{ ":  \"value\"  ",                           12, json_TypeValueString,  "value"                                },
		{ "  :\"value\"",                             10, json_TypeValueString,  "value"                                },
		{ "  :\"value\"  ",                           12, json_TypeValueString,  "value"                                },
		{ "  :  \"value\"",                           12, json_TypeValueString,  "value"                                },
		{ "  :  \"value\"  ",                         14, json_TypeValueString,  "value"                                },
		{ "[value]",                                  7,  json_TypeArray,        "value"                                },
		{ "[value\\/\\:\\[\\]\\\\\\uD83D\\uDE00]",    29, json_TypeArray,        "value\\/\\:\\[\\]\\\\\\uD83D\\uDE00"  },
		{ "[  value]",                                9,  json_TypeArray,        "value"                                },
		{ "[value  ]",                                9,  json_TypeArray,        "value"                                },
		{ "[  value  ]",                              11, json_TypeArray,        "value"                                },
		{ "[/array[key]]",                            13, json_TypeArray,        "/array[key]"                          },
		{ "[  /array[key]]",                          15, json_TypeArray,        "/array[key]"                          },
		{ "[/array[key]  ]",                          15, json_TypeArray,        "/array[key]"                          },
		{ "[  /array[key]  ]",                        17, json_TypeArray,        "/array[key]"                          },
		{ "  [value]",                                9,  json_TypeArray,        "value"                                },
		{ "[value]  ",                                9,  json_TypeArray,        "value"                                },
		{ "  [value]  ",                              11, json_TypeArray,        "value"                                },
		{ "  [  value]",                              11, json_TypeArray,        "value"                                },
		{ "[  value]  ",                              11, json_TypeArray,        "value"                                },
		{ "  [  value]  ",                            13, json_TypeArray,        "value"                                },
		{ "  [value  ]",                              11, json_TypeArray,        "value"                                },
		{ "[value  ]  ",                              11, json_TypeArray,        "value"                                },
		{ "  [value  ]  ",                            13, json_TypeArray,        "value"                                },
		{ "  [  value  ]",                            13, json_TypeArray,        "value"                                },
		{ "[  value  ]  ",                            13, json_TypeArray,        "value"                                },
		{ "  [  value  ]  ",                          15, json_TypeArray,        "value"                                },
		{ "  [/array[key]]",                          15, json_TypeArray,        "/array[key]"                          },
		{ "[/array[key]]  ",                          15, json_TypeArray,        "/array[key]"                          },
		{ "  [/array[key]]  ",                        17, json_TypeArray,        "/array[key]"                          },
		{ "  [  /array[key]]",                        17, json_TypeArray,        "/array[key]"                          },
		{ "[  /array[key]]  ",                        17, json_TypeArray,        "/array[key]"                          },
		{ "  [  /array[key]]  ",                      19, json_TypeArray,        "/array[key]"                          },
		{ "  [/array[key]  ]",                        17, json_TypeArray,        "/array[key]"                          },
		{ "[/array[key]  ]  ",                        17, json_TypeArray,        "/array[key]"                          },
		{ "  [/array[key]  ]  ",                      19, json_TypeArray,        "/array[key]"                          },
		{ "  [  /array[key]  ]",                      19, json_TypeArray,        "/array[key]"                          },
		{ "[  /array[key]  ]  ",                      19, json_TypeArray,        "/array[key]"                          },
		{ "  [  /array[key]  ]  ",                    21, json_TypeArray,        "/array[key]"                          },
		{ "[",                                        0                                                                 },
		{ "  [",                                      0                                                                 },
		{ "[  ",                                      0                                                                 },
		{ "  [  ",                                    0                                                                 },
		{ "[array\\]",                                0                                                                 },
		{ "  [array\\]",                              0                                                                 },
		{ "[array\\]  ",                              0                                                                 },
		{ "  [array\\]  ",                            0                                                                 },
		{ "[array\\\\\\]",                            0                                                                 },
		{ "  [array\\\\\\]",                          0                                                                 },
		{ "[array\\\\\\]  ",                          0                                                                 },
		{ "  [array\\\\\\]  ",                        0                                                                 },
		{ "]",                                        0                                                                 },
		{ "  ]",                                      0                                                                 },
		{ "]  ",                                      0                                                                 },
		{ "  ]  ",                                    0                                                                 },
		{ "]/key",                                    0                                                                 },
		{ "  ]/key",                                  0                                                                 },
		{ "]/key  ",                                  0                                                                 },
		{ "  ]/key  ",                                0                                                                 },
		{ "\"",                                       0                                                                 },
		{ "  \"",                                     0                                                                 },
		{ "\"  ",                                     0                                                                 },
		{ "  \"  ",                                   0                                                                 },
		{ "value\"",                                  0                                                                 },
		{ "  value\"",                                0                                                                 },
		{ "value\"  ",                                0                                                                 },
		{ "  value\"  ",                              0                                                                 },
		{ "\"value",                                  0                                                                 },
		{ "  \"value",                                0                                                                 },
		{ "\"value  ",                                0                                                                 },
		{ "  \"value  ",                              0                                                                 },
		{ "\"value\\\"",                              0                                                                 },
		{ "  \"value\\\"",                            0                                                                 },
		{ "\"value\\\"  ",                            0                                                                 },
		{ "  \"value\\\"  ",                          0                                                                 },
		{ "\"value\\\\\\\"",                          0                                                                 },
		{ "  \"value\\\\\\\"",                        0                                                                 },
		{ "\"value\\\\\\\"  ",                        0                                                                 },
		{ "  \"value\\\\\\\"  ",                      0                                                                 },
	};
	tJsonType ComponentType;
	tJsonPath Component;
	size_t Length;
	size_t n;
	bool ok;

	for (ok = true, n = 0; ok && (n < sizeof(TestPaths) / sizeof(TestPaths[0])); n++)
	{
		Length = JsonPathGetComponent(JsonPathAscii(TestPaths[n].Path), &ComponentType, &Component);

		ok = (Length == TestPaths[n].Length);

		if (TestPaths[n].Length != 0)
		{
			ok = ok && (ComponentType == TestPaths[n].Type);

			if (TestPaths[n].Component == NULL)
			{
				ok = ok && (Component.Value == NULL);

				ok = ok && (Component.Length == 0);
			}
			else
			{
				ok = ok && (Component.Value != NULL);

				ok = ok && (Component.Length == strlen(TestPaths[n].Component));

				ok = ok && (strncmp((const char *)Component.Value, TestPaths[n].Component, Component.Length) == 0);
			}
		}
	}

	return ok;
}


static const tTestCase TestCaseJsonPath[] =
{
	{ "JsonPathUtf8",                TestJsonPathUtf8                },
	{ "JsonPathAscii",               TestJsonPathAscii               },
	{ "JsonPathLeft",                TestJsonPathLeft                },
	{ "JsonPathRight",               TestJsonPathRight               },
	{ "JsonPathMiddle",              TestJsonPathMiddle              },
	{ "JsonPathGetNextUtf8Code",     TestJsonPathGetNextUtf8Code     },
	{ "JsonPathGetPreviousUtf8Code", TestJsonPathGetPreviousUtf8Code },
	{ "JsonPathSetString",           TestJsonPathSetString           },
	{ "JsonPathCompareString",       TestJsonPathCompareString       },
	{ "JsonPathGetComponent",        TestJsonPathGetComponent        }
};


const tTest TestJsonPath =
{
	"JsonPath",
	TestCaseJsonPath,
	sizeof(TestCaseJsonPath) / sizeof(TestCaseJsonPath[0])
};
