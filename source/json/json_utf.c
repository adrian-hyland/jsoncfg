#include "json_utf8.h"
#include "json_utf16.h"
#include "json_utf.h"


size_t JsonUtfGetType(const uint8_t *Content, size_t Length, tJsonUtfType *UtfType)
{
	if (Length >= 2)
	{
		if ((Content[0] == 0xFE) && (Content[1] == 0xFF))
		{
			*UtfType = json_Utf16be;
			return 2;
		}
		if ((Content[0] == 0xFF) && (Content[1] == 0xFE))
		{
			*UtfType = json_Utf16le;
			return 2;
		}
		if (Content[0] == 0x00)
		{
			if ((Content[1] != 0x00))
			{
				*UtfType = json_Utf16be;
				return 0;
			}
		}
		else if (Content[1] == 0x00)
		{
			*UtfType = json_Utf16le;
			return 0;
		}
	}
	if ((Length >= 3) && (Content[0] == 0xEF) && (Content[1] == 0xBB) && (Content[2] == 0xBF))
	{
		*UtfType = json_Utf8;
		return 3;
	}

	*UtfType = json_Utf8;
	return 0;
}


size_t JsonUtfDecode(tJsonUtfType UtfType, const uint8_t *Content, size_t Length, size_t Offset, tJsonCharacter *Character)
{
	switch (UtfType)
	{
		case json_Utf8:
			return JsonUtf8DecodeNext(Content, Length, Offset, Character);
		break;

		case json_Utf16be:
			return JsonUtf16beDecodeNext(Content, Length, Offset, Character);
		break;

		case json_Utf16le:
			return JsonUtf16leDecodeNext(Content, Length, Offset, Character);
		break;

		default:
			return 0;
		break;
	}
}


size_t JsonUtfEncode(tJsonUtfType UtfType, uint8_t *Content, size_t Size, size_t Offset, tJsonCharacter Character)
{
	switch (UtfType)
	{
		case json_Utf8:
			return JsonUtf8Encode(Content, Size, Offset, Character);
		break;

		case json_Utf16be:
			return JsonUtf16beEncode(Content, Size, Offset, Character);
		break;

		case json_Utf16le:
			return JsonUtf16leEncode(Content, Size, Offset, Character);
		break;

		default:
			return 0;
		break;
	}
}
