#include <string.h>
#include "json_utf16.h"
#include "json_path.h"
#include "test_json.h"


static tTestResult TestJsonPathUtf8(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonPath Path;
	const uint8_t EmptyString[] = "";
	const uint8_t NonEmptyString[] = "a/b/c";

	Path = JsonPathUtf8(NULL);

	TEST_IS_EQ(Path.Value, NULL, TestResult);
	TEST_IS_EQ(Path.Length, 0, TestResult);

	Path = JsonPathUtf8(EmptyString);

	TEST_IS_EQ(Path.Value, EmptyString, TestResult);
	TEST_IS_EQ(Path.Length, 0, TestResult);

	Path = JsonPathUtf8(NonEmptyString);

	TEST_IS_EQ(Path.Value, NonEmptyString, TestResult);
	TEST_IS_EQ(Path.Length, strlen((const char *)NonEmptyString), TestResult);

	return TestResult;
}


static tTestResult TestJsonPathAscii(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonPath Path;
	const char EmptyString[] = "";
	const char NonEmptyString[] = "a/b/c";

	Path = JsonPathAscii(NULL);

	TEST_IS_EQ(Path.Value, NULL, TestResult);
	TEST_IS_EQ(Path.Length, 0, TestResult);

	Path = JsonPathAscii(EmptyString);

	TEST_IS_EQ(Path.Value, (const uint8_t *)EmptyString, TestResult);
	TEST_IS_EQ(Path.Length, 0, TestResult);

	Path = JsonPathAscii(NonEmptyString);

	TEST_IS_EQ(Path.Value, (const uint8_t *)NonEmptyString, TestResult);
	TEST_IS_EQ(Path.Length, strlen(NonEmptyString), TestResult);

	return TestResult;
}


static tTestResult TestJsonPathLeft(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	const uint8_t String[] = "0123456789";
	tJsonPath Path;
	size_t n;

	Path = JsonPathUtf8(String);

	TEST_IS_EQ(Path.Value, String, TestResult);
	TEST_IS_EQ(Path.Length, sizeof(String) - 1, TestResult);

	Path = JsonPathLeft(Path, sizeof(String));

	TEST_IS_EQ(Path.Value, String, TestResult);
	TEST_IS_EQ(Path.Length, sizeof(String) - 1, TestResult);

	for (n = 0; n < sizeof(String) - 1; n++)
	{
		Path = JsonPathLeft(Path, Path.Length - 1);

		TEST_IS_EQ(Path.Value, String, TestResult);
		TEST_IS_EQ(Path.Length, sizeof(String) - n - 2, TestResult);
	}

	Path = JsonPathLeft(Path, 1);

	TEST_IS_EQ(Path.Value, String, TestResult);
	TEST_IS_EQ(Path.Length, 0, TestResult);

	return TestResult;
}


static tTestResult TestJsonPathRight(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	const uint8_t String[] = "0123456789";
	tJsonPath Path;
	size_t n;

	Path = JsonPathUtf8(String);

	TEST_IS_EQ(Path.Value, String, TestResult);
	TEST_IS_EQ(Path.Length, sizeof(String) - 1, TestResult);

	Path = JsonPathRight(Path, sizeof(String));

	TEST_IS_EQ(Path.Value, &String[sizeof(String) - 1], TestResult);
	TEST_IS_EQ(Path.Length, 0, TestResult);

	Path = JsonPathUtf8(String);

	for (n = 0; n < sizeof(String) - 1; n++)
	{
		Path = JsonPathRight(Path, 1);

		TEST_IS_EQ(Path.Value, &String[n + 1], TestResult);
		TEST_IS_EQ(Path.Length, sizeof(String) - n - 2, TestResult);
	}

	Path = JsonPathRight(Path, 1);

	TEST_IS_EQ(Path.Value, &String[sizeof(String) - 1], TestResult);
	TEST_IS_EQ(Path.Length, 0, TestResult);

	return TestResult;
}


static tTestResult TestJsonPathMiddle(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	const uint8_t String[] = "0123456789";
	tJsonPath Path;
	size_t n;

	Path = JsonPathUtf8(String);

	TEST_IS_EQ(Path.Value, String, TestResult);
	TEST_IS_EQ(Path.Length, sizeof(String) - 1, TestResult);

	Path = JsonPathMiddle(Path, 0, sizeof(String));

	TEST_IS_EQ(Path.Value, String, TestResult);
	TEST_IS_EQ(Path.Length, sizeof(String) - 1, TestResult);

	for (n = 0; n < sizeof(String) / 2; n++)
	{
		Path = JsonPathMiddle(Path, 1, Path.Length - 1);

		TEST_IS_EQ(Path.Value, &String[n + 1], TestResult);
		TEST_IS_EQ(Path.Length, sizeof(String) - n * 2 - 3, TestResult);
	}

	Path = JsonPathMiddle(Path, 1, 2);

	TEST_IS_EQ(Path.Value, &String[5], TestResult);
	TEST_IS_EQ(Path.Length, 0, TestResult);

	Path = JsonPathUtf8(String);

	Path = JsonPathMiddle(Path, Path.Length, 0);

	TEST_IS_EQ(Path.Value, &String[sizeof(String) - 1], TestResult);
	TEST_IS_EQ(Path.Length, 0, TestResult);

	return TestResult;
}


static tTestResult TestJsonPathGetNextCharacter(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonPath Path;
	tJsonCharacter Character;
	tJsonCharacter PathCharacter;
	const uint8_t Escaped[] = "\\b\\f\\n\\r\\t\\\\\\\"";
	const uint8_t Unescaped[] = "\b\f\n\r\t\\\"";
	const uint8_t Invalid[] = "\\";
	uint8_t Valid[JSON_UTF8_MAX_SIZE + 1];
	uint8_t EscapedUnicode[6 + 6 + 1 + 1];
	uint8_t Utf16[JSON_UTF16_MAX_SIZE];
	size_t Offset;
	size_t Length;
	bool IsEscaped;
	size_t n;

	for (Character = 0x20; Character < 0xD800; Character++)
	{
		if (Character != '\\')
		{
			Length = JsonUtf8Encode(Valid, sizeof(Valid), 0, Character);
			Valid[Length] = '\0';
			Length = JsonPathGetNextCharacter(JsonPathUtf8(Valid), 0, &IsEscaped, &PathCharacter);
			TEST_IS_NOT_EQ(Length, 0, TestResult);
			TEST_IS_FALSE(IsEscaped, TestResult);
			TEST_IS_EQ(PathCharacter, Character, TestResult);
		}
	}

	for (Character = 0xE000; Character < 0x110000; Character++)
	{
		Length = JsonUtf8Encode(Valid, sizeof(Valid), 0, Character);
		Valid[Length] = '\0';
		Length = JsonPathGetNextCharacter(JsonPathUtf8(Valid), 0, &IsEscaped, &PathCharacter);
		TEST_IS_NOT_EQ(Length, 0, TestResult);
		TEST_IS_FALSE(IsEscaped, TestResult);
		TEST_IS_EQ(PathCharacter, Character, TestResult);
	}

	Path = JsonPathUtf8(Escaped);
	for (Offset = 0, n = 0; n < sizeof(Unescaped) - 1; Offset = Offset + Length, n++)
	{
		Length = JsonPathGetNextCharacter(Path, Offset, &IsEscaped, &PathCharacter);
		TEST_IS_NOT_EQ(Length, 0, TestResult);
		TEST_IS_TRUE(IsEscaped, TestResult);
		TEST_IS_EQ(PathCharacter, Unescaped[n], TestResult);
	}
	TEST_IS_EQ(JsonPathGetNextCharacter(Path, Offset, &IsEscaped, &PathCharacter), 0, TestResult);

	Path = JsonPathUtf8(Invalid);
	TEST_IS_EQ(JsonPathGetNextCharacter(Path, 0, &IsEscaped, &PathCharacter), 0, TestResult);

	for (Character = 0; Character < 0xD800; Character++)
	{
		EscapedUnicode[0] = '\\';
		EscapedUnicode[1] = '\0';
		TEST_IS_EQ(JsonPathGetNextCharacter(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &PathCharacter), 0, TestResult);

		EscapedUnicode[1] = 'u';
		EscapedUnicode[2] = '\0';
		TEST_IS_EQ(JsonPathGetNextCharacter(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &PathCharacter), 0, TestResult);

		EscapedUnicode[2] = JsonCharacterFromHexDigit((Character >> 12) & 0x0F);
		EscapedUnicode[3] = '\0';
		TEST_IS_EQ(JsonPathGetNextCharacter(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &PathCharacter), 0, TestResult);

		EscapedUnicode[3] = JsonCharacterFromHexDigit((Character >> 8) & 0x0F);
		EscapedUnicode[4] = '\0';
		TEST_IS_EQ(JsonPathGetNextCharacter(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &PathCharacter), 0, TestResult);

		EscapedUnicode[4] = JsonCharacterFromHexDigit((Character >> 4) & 0x0F);
		EscapedUnicode[5] = '\0';
		TEST_IS_EQ(JsonPathGetNextCharacter(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &PathCharacter), 0, TestResult);

		EscapedUnicode[5] = JsonCharacterFromHexDigit(Character & 0x0F);
		EscapedUnicode[6] = '\0';
		Length = JsonPathGetNextCharacter(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &PathCharacter);
		TEST_IS_EQ(Length, strlen((char *)EscapedUnicode), TestResult);
		TEST_IS_TRUE(IsEscaped, TestResult);
		TEST_IS_EQ(PathCharacter, Character, TestResult);

		EscapedUnicode[6] = '0';
		EscapedUnicode[7] = '\0';
		Length = JsonPathGetNextCharacter(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &PathCharacter);
		TEST_IS_EQ(Length, strlen((char *)EscapedUnicode) - 1, TestResult);
		TEST_IS_TRUE(IsEscaped, TestResult);
		TEST_IS_EQ(PathCharacter, Character, TestResult);
	}

	for (Character = 0xD800; Character < 0xE000; Character++)
	{
		EscapedUnicode[0] = '\\';
		EscapedUnicode[1] = '\0';
		TEST_IS_EQ(JsonPathGetNextCharacter(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &PathCharacter), 0, TestResult);

		EscapedUnicode[1] = 'u';
		EscapedUnicode[2] = '\0';
		TEST_IS_EQ(JsonPathGetNextCharacter(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &PathCharacter), 0, TestResult);

		EscapedUnicode[2] = JsonCharacterFromHexDigit((Character >> 12) & 0x0F);
		EscapedUnicode[3] = '\0';
		TEST_IS_EQ(JsonPathGetNextCharacter(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &PathCharacter), 0, TestResult);

		EscapedUnicode[3] = JsonCharacterFromHexDigit((Character >> 8) & 0x0F);
		EscapedUnicode[4] = '\0';
		TEST_IS_EQ(JsonPathGetNextCharacter(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &PathCharacter), 0, TestResult);

		EscapedUnicode[4] = JsonCharacterFromHexDigit((Character >> 4) & 0x0F);
		EscapedUnicode[5] = '\0';
		TEST_IS_EQ(JsonPathGetNextCharacter(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &PathCharacter), 0, TestResult);

		EscapedUnicode[5] = JsonCharacterFromHexDigit(Character & 0x0F);
		EscapedUnicode[6] = '\0';
		TEST_IS_EQ(JsonPathGetNextCharacter(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &PathCharacter), 0, TestResult);

		EscapedUnicode[6] = '0';
		EscapedUnicode[7] = '\0';
		TEST_IS_EQ(JsonPathGetNextCharacter(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &PathCharacter), 0, TestResult);
	}

	for (Character = 0xE000; Character < 0x10000; Character++)
	{
		EscapedUnicode[0] = '\\';
		EscapedUnicode[1] = '\0';
		TEST_IS_EQ(JsonPathGetNextCharacter(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &PathCharacter), 0, TestResult);

		EscapedUnicode[1] = 'u';
		EscapedUnicode[2] = '\0';
		TEST_IS_EQ(JsonPathGetNextCharacter(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &PathCharacter), 0, TestResult);

		EscapedUnicode[2] = JsonCharacterFromHexDigit((Character >> 12) & 0x0F);
		EscapedUnicode[3] = '\0';
		TEST_IS_EQ(JsonPathGetNextCharacter(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &PathCharacter), 0, TestResult);

		EscapedUnicode[3] = JsonCharacterFromHexDigit((Character >> 8) & 0x0F);
		EscapedUnicode[4] = '\0';
		TEST_IS_EQ(JsonPathGetNextCharacter(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &PathCharacter), 0, TestResult);

		EscapedUnicode[4] = JsonCharacterFromHexDigit((Character >> 4) & 0x0F);
		EscapedUnicode[5] = '\0';
		TEST_IS_EQ(JsonPathGetNextCharacter(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &PathCharacter), 0, TestResult);

		EscapedUnicode[5] = JsonCharacterFromHexDigit(Character & 0x0F);
		EscapedUnicode[6] = '\0';
		Length = JsonPathGetNextCharacter(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &PathCharacter);
		TEST_IS_EQ(Length, strlen((char *)EscapedUnicode), TestResult);
		TEST_IS_TRUE(IsEscaped, TestResult);
		TEST_IS_EQ(PathCharacter, Character, TestResult);

		EscapedUnicode[6] = '0';
		EscapedUnicode[7] = '\0';
		Length = JsonPathGetNextCharacter(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &PathCharacter);
		TEST_IS_EQ(Length, strlen((char *)EscapedUnicode) - 1, TestResult);
		TEST_IS_TRUE(IsEscaped, TestResult);
		TEST_IS_EQ(PathCharacter, Character, TestResult);
	}

	for (Character = 0x10000; Character < 0x110000; Character++)
	{
		JsonUtf16beEncode(Utf16, sizeof(Utf16), 0, Character);
		EscapedUnicode[0] = '\\';
		EscapedUnicode[1] = 'u';
		EscapedUnicode[2] = JsonCharacterFromHexDigit((Utf16[0] >> 4) & 0x0F);
		EscapedUnicode[3] = JsonCharacterFromHexDigit(Utf16[0] & 0x0F);
		EscapedUnicode[4] = JsonCharacterFromHexDigit((Utf16[1] >> 4) & 0x0F);
		EscapedUnicode[5] = JsonCharacterFromHexDigit(Utf16[1] & 0x0F);
		EscapedUnicode[6] = '\\';
		EscapedUnicode[7] = '\0';
		TEST_IS_EQ(JsonPathGetNextCharacter(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &PathCharacter), 0, TestResult);

		EscapedUnicode[7] = 'u';
		EscapedUnicode[8] = '\0';
		TEST_IS_EQ(JsonPathGetNextCharacter(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &PathCharacter), 0, TestResult);

		EscapedUnicode[8] = JsonCharacterFromHexDigit((Utf16[2] >> 4) & 0x0F);
		EscapedUnicode[9] = '\0';
		TEST_IS_EQ(JsonPathGetNextCharacter(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &PathCharacter), 0, TestResult);

		EscapedUnicode[9] = JsonCharacterFromHexDigit(Utf16[2] & 0x0F);
		EscapedUnicode[10] = '\0';
		TEST_IS_EQ(JsonPathGetNextCharacter(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &PathCharacter), 0, TestResult);

		EscapedUnicode[10] = JsonCharacterFromHexDigit((Utf16[3] >> 4) & 0x0F);
		EscapedUnicode[11] = '\0';
		TEST_IS_EQ(JsonPathGetNextCharacter(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &PathCharacter), 0, TestResult);

		EscapedUnicode[11] = JsonCharacterFromHexDigit(Utf16[3] & 0x0F);
		EscapedUnicode[12] = '\0';
		Length = JsonPathGetNextCharacter(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &PathCharacter);
		TEST_IS_EQ(Length, strlen((char *)EscapedUnicode), TestResult);
		TEST_IS_TRUE(IsEscaped, TestResult);
		TEST_IS_EQ(PathCharacter, Character, TestResult);

		EscapedUnicode[12] = '0';
		EscapedUnicode[13] = '\0';
		Length = JsonPathGetNextCharacter(JsonPathUtf8(EscapedUnicode), 0, &IsEscaped, &PathCharacter);
		TEST_IS_EQ(Length, strlen((char *)EscapedUnicode) - 1, TestResult);
		TEST_IS_TRUE(IsEscaped, TestResult);
		TEST_IS_EQ(PathCharacter, Character, TestResult);
	}

	return TestResult;
}


static tTestResult TestJsonPathGetPreviousCharacter(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonPath Path;
	tJsonCharacter Character;
	tJsonCharacter PathCharacter;
	const uint8_t Escaped[] = "\\b\\f\\n\\r\\t\\\\\\\"";
	const uint8_t Unescaped[] = "\b\f\n\r\t\\\"";
	const uint8_t Invalid[] = "\\";
	uint8_t Valid[JSON_UTF8_MAX_SIZE + 1];
	uint8_t EscapedUnicode[6 + 6 + 1 + 1];
	uint8_t Utf16[JSON_UTF16_MAX_SIZE];
	size_t Offset;
	size_t Length;
	bool IsEscaped;
	size_t n;

	for (Character = 0x20; Character < 0xD800; Character++)
	{
		if (Character != '\\')
		{
			Length = JsonUtf8Encode(Valid, sizeof(Valid), 0, Character);
			Valid[Length] = '\0';
			Length = JsonPathGetPreviousCharacter(JsonPathUtf8(Valid), Length, &IsEscaped, &PathCharacter);
			TEST_IS_NOT_EQ(Length, 0, TestResult);
			TEST_IS_FALSE(IsEscaped, TestResult);
			TEST_IS_EQ(PathCharacter, Character, TestResult);
		}
	}

	for (Character = 0xE000; Character < 0x110000; Character++)
	{
		Length = JsonUtf8Encode(Valid, sizeof(Valid), 0, Character);
		Valid[Length] = '\0';
		Length = JsonPathGetPreviousCharacter(JsonPathUtf8(Valid), Length, &IsEscaped, &PathCharacter);
		TEST_IS_NOT_EQ(Length, 0, TestResult);
		TEST_IS_FALSE(IsEscaped, TestResult)
		TEST_IS_EQ(PathCharacter, Character, TestResult);
	}

	Path = JsonPathUtf8(Escaped);
	for (Offset = Path.Length, n = sizeof(Unescaped) - 1; n > 0; Offset = Offset - Length, n--)
	{
		Length = JsonPathGetPreviousCharacter(Path, Offset, &IsEscaped, &PathCharacter);
		TEST_IS_NOT_EQ(Length, 0, TestResult);
		TEST_IS_TRUE(IsEscaped, TestResult);
		TEST_IS_EQ(PathCharacter, Unescaped[n - 1], TestResult);
	}
	TEST_IS_EQ(JsonPathGetPreviousCharacter(Path, Offset, &IsEscaped, &PathCharacter), 0, TestResult);

	Path = JsonPathUtf8(Invalid);
	TEST_IS_EQ(JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &PathCharacter), 0, TestResult);

	for (Character = 0; Character < 0xD800; Character++)
	{
		EscapedUnicode[0] = '\\';
		EscapedUnicode[1] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		TEST_IS_EQ(JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &PathCharacter), 0, TestResult);

		EscapedUnicode[1] = 'u';
		EscapedUnicode[2] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		TEST_IS_EQ(JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &PathCharacter), 0, TestResult);

		EscapedUnicode[2] = JsonCharacterFromHexDigit((Character >> 12) & 0x0F);
		EscapedUnicode[3] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &PathCharacter);
		TEST_IS_EQ(Length, 1, TestResult);
		TEST_IS_FALSE(IsEscaped, TestResult);
		TEST_IS_EQ(PathCharacter, EscapedUnicode[2], TestResult);

		EscapedUnicode[3] = JsonCharacterFromHexDigit((Character >> 8) & 0x0F);
		EscapedUnicode[4] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &PathCharacter);
		TEST_IS_EQ(Length, 1, TestResult);
		TEST_IS_FALSE(IsEscaped, TestResult);
		TEST_IS_EQ(PathCharacter, EscapedUnicode[3], TestResult);

		EscapedUnicode[4] = JsonCharacterFromHexDigit((Character >> 4) & 0x0F);
		EscapedUnicode[5] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &PathCharacter);
		TEST_IS_EQ(Length, 1, TestResult);
		TEST_IS_FALSE(IsEscaped, TestResult);
		TEST_IS_EQ(PathCharacter, EscapedUnicode[4], TestResult);

		EscapedUnicode[5] = JsonCharacterFromHexDigit(Character & 0x0F);
		EscapedUnicode[6] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &PathCharacter);
		TEST_IS_EQ(Length, Path.Length, TestResult);
		TEST_IS_TRUE(IsEscaped, TestResult);
		TEST_IS_EQ(PathCharacter, Character, TestResult);

		EscapedUnicode[6] = '0';
		EscapedUnicode[7] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &PathCharacter);
		TEST_IS_EQ(Length, 1, TestResult);
		TEST_IS_FALSE(IsEscaped, TestResult);
		TEST_IS_EQ(PathCharacter, EscapedUnicode[6], TestResult);
	}

	for (Character = 0xD800; Character < 0xE000; Character++)
	{
		EscapedUnicode[0] = '\\';
		EscapedUnicode[1] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		TEST_IS_EQ(JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &PathCharacter), 0, TestResult);

		EscapedUnicode[1] = 'u';
		EscapedUnicode[2] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		TEST_IS_EQ(JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &PathCharacter), 0, TestResult);

		EscapedUnicode[2] = JsonCharacterFromHexDigit((Character >> 12) & 0x0F);
		EscapedUnicode[3] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &PathCharacter);
		TEST_IS_EQ(Length, 1, TestResult);
		TEST_IS_FALSE(IsEscaped, TestResult);
		TEST_IS_EQ(PathCharacter, EscapedUnicode[2], TestResult);

		EscapedUnicode[3] = JsonCharacterFromHexDigit((Character >> 8) & 0x0F);
		EscapedUnicode[4] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &PathCharacter);
		TEST_IS_EQ(Length, 1, TestResult);
		TEST_IS_FALSE(IsEscaped, TestResult);
		TEST_IS_EQ(PathCharacter, EscapedUnicode[3], TestResult);

		EscapedUnicode[4] = JsonCharacterFromHexDigit((Character >> 4) & 0x0F);
		EscapedUnicode[5] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &PathCharacter);
		TEST_IS_EQ(Length, 1, TestResult);
		TEST_IS_FALSE(IsEscaped, TestResult);
		TEST_IS_EQ(PathCharacter, EscapedUnicode[4], TestResult);

		EscapedUnicode[5] = JsonCharacterFromHexDigit(Character & 0x0F);
		EscapedUnicode[6] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &PathCharacter);
		TEST_IS_EQ(Length, 1, TestResult);
		TEST_IS_FALSE(IsEscaped, TestResult);
		TEST_IS_EQ(PathCharacter, EscapedUnicode[5], TestResult);

		EscapedUnicode[6] = '0';
		EscapedUnicode[7] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &PathCharacter);
		TEST_IS_EQ(Length, 1, TestResult);
		TEST_IS_FALSE(IsEscaped, TestResult);
		TEST_IS_EQ(PathCharacter, EscapedUnicode[6], TestResult);
	}

	for (Character = 0xE000; Character < 0x10000; Character++)
	{
		EscapedUnicode[0] = '\\';
		EscapedUnicode[1] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		TEST_IS_EQ(JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &PathCharacter), 0, TestResult);

		EscapedUnicode[1] = 'u';
		EscapedUnicode[2] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		TEST_IS_EQ(JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &PathCharacter), 0, TestResult);

		EscapedUnicode[2] = JsonCharacterFromHexDigit((Character >> 12) & 0x0F);
		EscapedUnicode[3] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &PathCharacter);
		TEST_IS_EQ(Length, 1, TestResult);
		TEST_IS_FALSE(IsEscaped, TestResult);
		TEST_IS_EQ(PathCharacter, EscapedUnicode[2], TestResult);

		EscapedUnicode[3] = JsonCharacterFromHexDigit((Character >> 8) & 0x0F);
		EscapedUnicode[4] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &PathCharacter);
		TEST_IS_EQ(Length, 1, TestResult);
		TEST_IS_FALSE(IsEscaped, TestResult);
		TEST_IS_EQ(PathCharacter, EscapedUnicode[3], TestResult);

		EscapedUnicode[4] = JsonCharacterFromHexDigit((Character >> 4) & 0x0F);
		EscapedUnicode[5] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &PathCharacter);
		TEST_IS_EQ(Length, 1, TestResult);
		TEST_IS_FALSE(IsEscaped, TestResult);
		TEST_IS_EQ(PathCharacter, EscapedUnicode[4], TestResult);

		EscapedUnicode[5] = JsonCharacterFromHexDigit(Character & 0x0F);
		EscapedUnicode[6] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &PathCharacter);
		TEST_IS_EQ(Length, Path.Length, TestResult);
		TEST_IS_TRUE(IsEscaped, TestResult);
		TEST_IS_EQ(PathCharacter, Character, TestResult);

		EscapedUnicode[6] = '0';
		EscapedUnicode[7] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &PathCharacter);
		TEST_IS_EQ(Length, 1, TestResult);
		TEST_IS_FALSE(IsEscaped, TestResult);
		TEST_IS_EQ(PathCharacter, EscapedUnicode[6], TestResult);
	}

	for (Character = 0x10000; Character < 0x110000; Character++)
	{
		JsonUtf16beEncode(Utf16, sizeof(Utf16), 0, Character);
		EscapedUnicode[0] = '\\';
		EscapedUnicode[1] = 'u';
		EscapedUnicode[2] = JsonCharacterFromHexDigit((Utf16[0] >> 4) & 0x0F);
		EscapedUnicode[3] = JsonCharacterFromHexDigit(Utf16[0] & 0x0F);
		EscapedUnicode[4] = JsonCharacterFromHexDigit((Utf16[1] >> 4) & 0x0F);
		EscapedUnicode[5] = JsonCharacterFromHexDigit(Utf16[1] & 0x0F);
		EscapedUnicode[6] = '\\';
		EscapedUnicode[7] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		TEST_IS_EQ(JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &PathCharacter), 0, TestResult);

		EscapedUnicode[7] = 'u';
		EscapedUnicode[8] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		TEST_IS_EQ(JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &PathCharacter), 0, TestResult);

		EscapedUnicode[8] = JsonCharacterFromHexDigit((Utf16[2] >> 4) & 0x0F);
		EscapedUnicode[9] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &PathCharacter);
		TEST_IS_EQ(Length, 1, TestResult);
		TEST_IS_FALSE(IsEscaped, TestResult);
		TEST_IS_EQ(PathCharacter, EscapedUnicode[8], TestResult);

		EscapedUnicode[9] = JsonCharacterFromHexDigit(Utf16[2] & 0x0F);
		EscapedUnicode[10] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &PathCharacter);
		TEST_IS_EQ(Length, 1, TestResult);
		TEST_IS_FALSE(IsEscaped, TestResult);
		TEST_IS_EQ(PathCharacter, EscapedUnicode[9], TestResult);

		EscapedUnicode[10] = JsonCharacterFromHexDigit((Utf16[3] >> 4) & 0x0F);
		EscapedUnicode[11] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &PathCharacter);
		TEST_IS_EQ(Length, 1, TestResult);
		TEST_IS_FALSE(IsEscaped, TestResult);
		TEST_IS_EQ(PathCharacter, EscapedUnicode[10], TestResult);

		EscapedUnicode[11] = JsonCharacterFromHexDigit(Utf16[3] & 0x0F);
		EscapedUnicode[12] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &PathCharacter);
		TEST_IS_EQ(Length, Path.Length, TestResult);
		TEST_IS_TRUE(IsEscaped, TestResult);
		TEST_IS_EQ(PathCharacter, Character, TestResult);

		EscapedUnicode[12] = '0';
		EscapedUnicode[13] = '\0';
		Path = JsonPathUtf8(EscapedUnicode);
		Length = JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &PathCharacter);
		TEST_IS_EQ(Length, 1, TestResult);
		TEST_IS_FALSE(IsEscaped, TestResult);
		TEST_IS_EQ(PathCharacter, EscapedUnicode[12], TestResult);
	}

	return TestResult;
}


static tTestResult TestJsonPathSetString(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
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
	tJsonCharacter StringCharacter;
	tJsonCharacter PathCharacter;
	size_t StringOffset;
	size_t PathOffset;
	size_t StringLength;
	size_t PathLength;
	size_t n;
	bool IsEscaped;

	JsonStringSetUp(&String);

	for (n = 0; n < sizeof(ValidPaths) / sizeof(ValidPaths[0]); n++)
	{
		Path = JsonPathAscii(ValidPaths[n]);

		TEST_IS_TRUE(JsonPathSetString(Path, &String), TestResult);

		for (StringOffset = 0, PathOffset = 0; PathOffset < Path.Length; StringOffset = StringOffset + StringLength, PathOffset = PathOffset + PathLength)
		{
			PathLength = JsonPathGetNextCharacter(Path, PathOffset, &IsEscaped, &PathCharacter);
			StringLength = JsonStringGetNextCharacter(&String, StringOffset, &StringCharacter);

			TEST_IS_NOT_EQ(PathLength, 0, TestResult);
			TEST_IS_NOT_EQ(StringLength, 0, TestResult);
			TEST_IS_EQ(StringCharacter, PathCharacter, TestResult);
		}
	}

	for (n = 0; n < sizeof(InvalidPaths) / sizeof(InvalidPaths[0]); n++)
	{
		TEST_IS_FALSE(JsonPathSetString(JsonPathAscii(InvalidPaths[n]), &String), TestResult);
	}

	JsonStringCleanUp(&String);

	return TestResult;
}


static tTestResult TestJsonPathCompareString(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	static const char *Paths[] =
	{
		"path value",
		"escaped \\:\\/\\[\\]\\\\\\uD83D\\uDE00 path",
	};
	tJsonString String;
	size_t PathLength;
	size_t n;

	JsonStringSetUp(&String);

	for (n = 0; n < sizeof(Paths) / sizeof(Paths[0]); n++)
	{
		PathLength = strlen(Paths[n]) - 1;

		TEST_IS_TRUE(JsonPathSetString(JsonPathLeft(JsonPathAscii(Paths[n]), PathLength), &String), TestResult);

		TEST_IS_FALSE(JsonPathCompareString(JsonPathLeft(JsonPathAscii(Paths[n]), PathLength - 1), &String), TestResult);

		TEST_IS_TRUE(JsonPathCompareString(JsonPathLeft(JsonPathAscii(Paths[n]), PathLength), &String), TestResult);

		TEST_IS_FALSE(JsonPathCompareString(JsonPathLeft(JsonPathAscii(Paths[n]), PathLength + 1), &String), TestResult);
	}

	JsonStringCleanUp(&String);

	return TestResult;
}


static tTestResult TestJsonPathGetComponent(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
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

	for (n = 0; n < sizeof(TestPaths) / sizeof(TestPaths[0]); n++)
	{
		Length = JsonPathGetComponent(JsonPathAscii(TestPaths[n].Path), &ComponentType, &Component);

		TEST_IS_EQ(Length, TestPaths[n].Length, TestResult);

		if (TestPaths[n].Length != 0)
		{
			TEST_IS_EQ(ComponentType, TestPaths[n].Type, TestResult);

			if (TestPaths[n].Component == NULL)
			{
				TEST_IS_EQ(Component.Value, NULL, TestResult);

				TEST_IS_EQ(Component.Length, 0, TestResult);
			}
			else
			{
				TEST_IS_NOT_EQ(Component.Value, NULL, TestResult);

				TEST_IS_EQ(Component.Length, strlen(TestPaths[n].Component), TestResult);

				TEST_IS_EQ(strncmp((const char *)Component.Value, TestPaths[n].Component, Component.Length), 0, TestResult);
			}
		}
	}

	return TestResult;
}


static const tTestCase TestCaseJsonPath[] =
{
	{ "JsonPathUtf8",                 TestJsonPathUtf8                 },
	{ "JsonPathAscii",                TestJsonPathAscii                },
	{ "JsonPathLeft",                 TestJsonPathLeft                 },
	{ "JsonPathRight",                TestJsonPathRight                },
	{ "JsonPathMiddle",               TestJsonPathMiddle               },
	{ "JsonPathGetNextCharacter",     TestJsonPathGetNextCharacter     },
	{ "JsonPathGetPreviousCharacter", TestJsonPathGetPreviousCharacter },
	{ "JsonPathSetString",            TestJsonPathSetString            },
	{ "JsonPathCompareString",        TestJsonPathCompareString        },
	{ "JsonPathGetComponent",         TestJsonPathGetComponent         }
};


const tTest TestJsonPath =
{
	"JsonPath",
	TestCaseJsonPath,
	sizeof(TestCaseJsonPath) / sizeof(TestCaseJsonPath[0])
};
