#include "json_utf.h"
#include "json_utf8.h"
#include "json_utf16.h"
#include "test_json.h"


static tTestResult TestJsonUtfGetType(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonCharacter Character;
	tJsonUtfType UtfTypes[] = { json_Utf8, json_Utf16be, json_Utf16le };
	tJsonUtfType UtfType;
	tJsonUtf Utf;
	size_t UtfLength;
	size_t Length;
	size_t Type;

	Length = JsonUtfGetType(NULL, 0, &UtfType);
	TEST_IS_ZERO(Length, TestResult);
	TEST_IS_EQ(UtfType, json_Utf8, TestResult);

	for (Type = 0; Type < sizeof(UtfTypes) / sizeof(UtfTypes[0]); Type++)
	{
		UtfLength = JsonUtfEncode(UtfTypes[Type], Utf, sizeof(Utf), 0, JSON_CHARACTER_BOM);
		Length = JsonUtfGetType(Utf, UtfLength, &UtfType);
		TEST_IS_EQ(Length, UtfLength, TestResult);
		TEST_IS_EQ(UtfType, UtfTypes[Type], TestResult);

		for (Character = 0x20; Character < 0x7F; Character++)
		{
			UtfLength = JsonUtfEncode(UtfTypes[Type], Utf, sizeof(Utf), 0, Character);
			Length = JsonUtfGetType(Utf, UtfLength, &UtfType);
			TEST_IS_ZERO(Length, TestResult);
			TEST_IS_EQ(UtfType, UtfTypes[Type], TestResult);
		}
	}

	return TestResult;
}


static tTestResult TestJsonUtfEncode(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonCharacter Character;
	tJsonUtf Utf;
	size_t EncodeLength;
	size_t DecodeLength;

	EncodeLength = JsonUtfEncode(json_UtfUnknown, Utf, sizeof(Utf), 0, 0x10FFFF);
	TEST_IS_ZERO(EncodeLength, TestResult);

	EncodeLength = JsonUtfEncode(json_Utf8, Utf, sizeof(Utf), 0, 0x10FFFF);
	TEST_IS_EQ(EncodeLength, JSON_UTF8_MAX_SIZE, TestResult);
	DecodeLength = JsonUtf8DecodeNext(Utf, EncodeLength, 0, &Character);
	TEST_IS_EQ(DecodeLength, EncodeLength, TestResult);
	TEST_IS_EQ(Character, 0x10FFFF, TestResult);

	EncodeLength = JsonUtfEncode(json_Utf16be, Utf, sizeof(Utf), 0, 0x10FFFF);
	TEST_IS_EQ(EncodeLength, JSON_UTF16_MAX_SIZE, TestResult);
	DecodeLength = JsonUtf16beDecodeNext(Utf, EncodeLength, 0, &Character);
	TEST_IS_EQ(DecodeLength, EncodeLength, TestResult);
	TEST_IS_EQ(Character, 0x10FFFF, TestResult);

	EncodeLength = JsonUtfEncode(json_Utf16le, Utf, sizeof(Utf), 0, 0x10FFFF);
	TEST_IS_EQ(EncodeLength, JSON_UTF16_MAX_SIZE, TestResult);
	DecodeLength = JsonUtf16leDecodeNext(Utf, EncodeLength, 0, &Character);
	TEST_IS_EQ(DecodeLength, EncodeLength, TestResult);
	TEST_IS_EQ(Character, 0x10FFFF, TestResult);

	return TestResult;
}


static tTestResult TestJsonUtfDecode(void)
{
	tTestResult TestResult = TEST_RESULT_INITIAL;
	tJsonCharacter Character;
	tJsonUtf Utf;
	size_t EncodeLength;
	size_t DecodeLength;

	DecodeLength = JsonUtfDecode(json_UtfUnknown, NULL, 0, 0, &Character);
	TEST_IS_ZERO(DecodeLength, TestResult);

	EncodeLength = JsonUtf8Encode(Utf, sizeof(Utf), 0, 0x10FFFF);
	TEST_IS_EQ(EncodeLength, JSON_UTF8_MAX_SIZE, TestResult);
	DecodeLength = JsonUtfDecode(json_Utf8, Utf, EncodeLength, 0, &Character);
	TEST_IS_EQ(DecodeLength, EncodeLength, TestResult);
	TEST_IS_EQ(Character, 0x10FFFF, TestResult);

	EncodeLength = JsonUtf16beEncode(Utf, sizeof(Utf), 0, 0x10FFFF);
	TEST_IS_EQ(EncodeLength, JSON_UTF16_MAX_SIZE, TestResult);
	DecodeLength = JsonUtfDecode(json_Utf16be, Utf, EncodeLength, 0, &Character);
	TEST_IS_EQ(DecodeLength, EncodeLength, TestResult);
	TEST_IS_EQ(Character, 0x10FFFF, TestResult);

	EncodeLength = JsonUtf16leEncode(Utf, sizeof(Utf), 0, 0x10FFFF);
	TEST_IS_EQ(EncodeLength, JSON_UTF16_MAX_SIZE, TestResult);
	DecodeLength = JsonUtfDecode(json_Utf16le, Utf, EncodeLength, 0, &Character);
	TEST_IS_EQ(DecodeLength, EncodeLength, TestResult);
	TEST_IS_EQ(Character, 0x10FFFF, TestResult);

	return TestResult;
}


static const tTestCase TestCaseJsonUtf[] =
{
	{ "JsonUtfGetType", TestJsonUtfGetType },
	{ "JsonUtfEncode",  TestJsonUtfEncode  },
	{ "JsonUtfDecode",  TestJsonUtfDecode  }
};


const tTest TestJsonUtf =
{
	"JsonUtf",
	TestCaseJsonUtf,
	sizeof(TestCaseJsonUtf) / sizeof(TestCaseJsonUtf[0])
};
