#include "json_utf8.h"


size_t JsonUtf8Encode(uint8_t *Content, size_t Length, size_t Offset, tJsonCharacter Character)
{
	if (Character < 0x80)
	{
		if (Offset < Length)
		{
			Content[Offset] = Character;
			return 1;
		}
	}
	else if (Character < 0x800)
	{
		if ((Length > 1) && (Offset < Length - 1))
		{
			Content[Offset] = 0xC0 + ((Character >> 6) & 0x1F);
			Content[Offset + 1] = 0x80 + (Character & 0x3F);
			return 2;
		}
	}
	else if (Character < 0x10000)
	{
		if ((Character < 0xD800) || (Character > 0xDFFF))
		{
			if ((Length > 2) && (Offset < Length - 2))
			{
				Content[Offset] = 0xE0 + ((Character >> 12) & 0x0F);
				Content[Offset + 1] = 0x80 + ((Character >> 6) & 0x3F);
				Content[Offset + 2] = 0x80 + (Character & 0x3F);
				return 3;
			}
		}
	}
	else if (Character < 0x110000)
	{
		if ((Length > 3) && (Offset < Length - 3))
		{
			Content[Offset] = 0xF0 + ((Character >> 18) & 0x07);
			Content[Offset + 1] = 0x80 + ((Character >> 12) & 0x3F);
			Content[Offset + 2] = 0x80 + ((Character >> 6) & 0x3F);
			Content[Offset + 3] = 0x80 + (Character & 0x3F);
			return 4;
		}
	}

	return 0;
}


size_t JsonUtf8DecodeNext(const uint8_t *Content, size_t Length, size_t Offset, tJsonCharacter *Character)
{
	if (Offset >= Length)
	{
		*Character = '\0';
		return 0;
	}

	if (Content[Offset] < 0x80)
	{
		*Character = Content[Offset];
		return 1;
	}
	
	if ((Length < 2) || (Offset > Length - 2) || (Content[Offset] < 0xC2) || ((Content[Offset + 1] & 0xC0) != 0x80))
	{
		*Character = JSON_CHARACTER_REPLACEMENT;
		return 0;
	}

	if (Content[Offset] < 0xE0)
	{
		*Character = ((Content[Offset] & 0x1F) << 6) + (Content[Offset + 1] & 0x3F);
		return 2;
	}
	
	if ((Length < 3) || (Offset > Length - 3) || ((Content[Offset + 2] & 0xC0) != 0x80))
	{
		*Character = JSON_CHARACTER_REPLACEMENT;
		return 0;
	}

	if (Content[Offset] < 0xF0)
	{
		if (((Content[Offset] == 0xE0) && (Content[Offset + 1] < 0xA0)) ||
		    ((Content[Offset] == 0xED) && (Content[Offset + 1] >= 0xA0)))
		{
			*Character = JSON_CHARACTER_REPLACEMENT;
			return 0;
		}

		*Character = ((Content[Offset] & 0x0F) << 12) + ((Content[Offset + 1] & 0x3F) << 6) + (Content[Offset + 2] & 0x3F);
		return 3;
	}
	
	if ((Length < 4) || (Offset > Length - 4) || ((Content[Offset + 3] & 0xC0) != 0x80))
	{
		*Character = JSON_CHARACTER_REPLACEMENT;
		return 0;
	}

	if (Content[Offset] < 0xF5)
	{
		if (((Content[Offset] == 0xF0) && (Content[Offset + 1] < 0x90)) ||
		    ((Content[Offset] == 0xF4) && (Content[Offset + 1] >= 0x90)))
		{
			*Character = JSON_CHARACTER_REPLACEMENT;
			return 0;
		}
		
		*Character = ((Content[Offset] & 0x07) << 18) + ((Content[Offset + 1] & 0x3F) << 12) + ((Content[Offset + 2] & 0x3F) << 6) + (Content[Offset + 3] & 0x3F);
		return 4;
	}

	*Character = JSON_CHARACTER_REPLACEMENT;
	return 0;
}


size_t JsonUtf8DecodePrevious(const uint8_t *Content, size_t Length, size_t Offset, tJsonCharacter *Character)
{
	if ((Offset < 1) || (Offset > Length))
	{
		*Character = 0;
		return 0;
	}

	if (Content[Offset - 1] < 0x80)
	{
		*Character = Content[Offset - 1];
		return 1;
	}
	
	if ((Offset < 2) || ((Content[Offset - 1] & 0xC0) != 0x80))
	{
		*Character = JSON_CHARACTER_REPLACEMENT;
		return 0;
	}

	if ((Content[Offset - 2] >= 0xC2) && (Content[Offset - 2] < 0xE0))
	{
		*Character = ((Content[Offset - 2] & 0x1F) << 6) + (Content[Offset - 1] & 0x3F);
		return 2;
	}
	
	if ((Offset < 3) || ((Content[Offset - 2] & 0xC0) != 0x80))
	{
		*Character = JSON_CHARACTER_REPLACEMENT;
		return 0;
	}

	if ((Content[Offset - 3] >= 0xE0) && (Content[Offset - 3] < 0xF0))
	{
		if (((Content[Offset - 3] == 0xE0) && (Content[Offset - 2] < 0xA0)) ||
		    ((Content[Offset - 3] == 0xED) && (Content[Offset - 2] >= 0xA0)))
		{
			*Character = JSON_CHARACTER_REPLACEMENT;
			return 0;
		}

		*Character = ((Content[Offset - 3] & 0x0F) << 12) + ((Content[Offset - 2] & 0x3F) << 6) + (Content[Offset - 1] & 0x3F);
		return 3;
	}
	
	if ((Offset < 4) || ((Content[Offset - 3] & 0xC0) != 0x80))
	{
		*Character = JSON_CHARACTER_REPLACEMENT;
		return 0;
	}

	if ((Content[Offset - 4] >= 0xF0) && (Content[Offset - 4] < 0xF5))
	{
		if (((Content[Offset - 4] == 0xF0) && (Content[Offset - 3] < 0x90)) ||
		    ((Content[Offset - 4] == 0xF4) && (Content[Offset - 3] >= 0x90)))
		{
			*Character = JSON_CHARACTER_REPLACEMENT;
			return 0;
		}
		
		*Character = ((Content[Offset - 4] & 0x07) << 18) + ((Content[Offset - 3] & 0x3F) << 12) + ((Content[Offset - 2] & 0x3F) << 6) + (Content[Offset - 1] & 0x3F);
		return 4;
	}

	*Character = JSON_CHARACTER_REPLACEMENT;
	return 0;
}
