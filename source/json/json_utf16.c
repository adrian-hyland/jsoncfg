#include "json_utf16.h"


bool JsonUtf16beIsHighSurrogate(const tJsonUtf16Unit Unit)
{
	return (Unit[0] >= 0xD8) && (Unit[0] < 0xDC);
}


bool JsonUtf16beIsLowSurrogate(const tJsonUtf16Unit Unit)
{
	return (Unit[0] >= 0xDC) && (Unit[0] < 0xE0);
}


size_t JsonUtf16beDecodeNext(const uint8_t *Content, size_t Length, size_t Offset, tJsonCharacter *Character)
{
	if (Offset >= Length)
	{
		*Character = '\0';
		return 0;
	}

	if ((Length < 2) || (Offset > Length - 2))
	{
		*Character = JSON_CHARACTER_REPLACEMENT;
		return 0;
	}

	if ((Content[Offset] < 0xD8) || (Content[Offset] >= 0xE0))
	{
		*Character = (Content[Offset] << 8) + Content[Offset + 1];
		return 2;
	}

	if ((Length < 4) || (Offset > Length - 4) || (Content[Offset] >= 0xDC))
	{
		*Character = JSON_CHARACTER_REPLACEMENT;
		return 0;
	}

	if ((Content[Offset + 2] >= 0xDC) && (Content[Offset + 2] < 0xE0))
	{
		*Character = 0x10000 + ((Content[Offset] & 0x03) << 18) + (Content[Offset + 1] << 10) + ((Content[Offset + 2] & 0x03) << 8) + Content[Offset + 3];
		return 4;
	}

	*Character = JSON_CHARACTER_REPLACEMENT;
	return 0;
}


size_t JsonUtf16beEncode(uint8_t *Content, size_t Size, size_t Offset, tJsonCharacter Character)
{
	if (Character < 0x10000)
	{
		if ((Character < 0xD800) || (Character >= 0xE000))
		{
			if ((Size > 1) && (Offset < Size - 1))
			{
				Content[Offset] = (Character >> 8) & 0xFF;
				Content[Offset + 1] = Character & 0xFF;
				return 2;
			}
		}
	}
	else if (Character < 0x110000)
	{
		if ((Size > 3) && (Offset < Size - 3))
		{
			Character = Character - 0x10000;
			Content[Offset] = 0xD8 + ((Character >> 18) & 0x03);
			Content[Offset + 1] = Character >> 10;
			Content[Offset + 2] = 0xDC + ((Character >> 8) & 0x03);
			Content[Offset + 3] = Character;
			return 4;
		}
	}

	return 0;
}


size_t JsonUtf16leDecodeNext(const uint8_t *Content, size_t Length, size_t Offset, tJsonCharacter *Character)
{
	if (Offset >= Length)
	{
		*Character = '\0';
		return 0;
	}

	if ((Length < 2) || (Offset > Length - 2))
	{
		*Character = JSON_CHARACTER_REPLACEMENT;
		return 0;
	}

	if ((Content[Offset + 1] < 0xD8) || (Content[Offset + 1] >= 0xE0))
	{
		*Character = (Content[Offset + 1] << 8) + Content[Offset];
		return 2;
	}

	if ((Length < 4) || (Offset > Length - 4) || (Content[Offset + 1] >= 0xDC))
	{
		*Character = JSON_CHARACTER_REPLACEMENT;
		return 0;
	}

	if ((Content[Offset + 3] >= 0xDC) && (Content[Offset + 3] < 0xE0))
	{
		*Character = 0x10000 + ((Content[Offset + 1] & 0x03) << 18) + (Content[Offset] << 10) + ((Content[Offset + 3] & 0x03) << 8) + Content[Offset + 2];
		return 4;
	}

	*Character = JSON_CHARACTER_REPLACEMENT;
	return 0;
}


size_t JsonUtf16leEncode(uint8_t *Content, size_t Size, size_t Offset, tJsonCharacter Character)
{
	if (Character < 0x10000)
	{
		if ((Character < 0xD800) || (Character >= 0xE000))
		{
			if ((Size > 1) && (Offset < Size - 1))
			{
				Content[Offset] = Character & 0xFF;
				Content[Offset + 1] = (Character >> 8) & 0xFF;
				return 2;
			}
		}
	}
	else if (Character < 0x110000)
	{
		if ((Size > 3) && (Offset < Size - 3))
		{
			Character = Character - 0x10000;
			Content[Offset] = Character >> 10;
			Content[Offset + 1] = 0xD8 + ((Character >> 18) & 0x03);
			Content[Offset + 2] = Character;
			Content[Offset + 3] = 0xDC + ((Character >> 8) & 0x03);
			return 4;
		}
	}

	return 0;
}
