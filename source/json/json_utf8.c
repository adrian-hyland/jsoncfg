#include "json_utf8.h"


tJsonUtf8Code JsonUtf8Code(tJsonCharacter Character)
{
	if (Character < 0x80)
	{
		return Character;
	}
	else if (Character < 0x800)
	{
		return 0xC080 + ((Character & 0x7C0) << 2) + (Character & 0x3F);
	}
	else if (Character < 0x10000)
	{
		if ((Character < 0xD800) || (Character > 0xDFFF))
		{
			return 0xE08080 + ((Character & 0xF000) << 4) + ((Character & 0xFC0) << 2) + (Character & 0x3F);
		}
	}
	else if (Character < 0x110000)
	{
		return 0xF0808080 + ((Character & 0x1C0000) << 6) + ((Character & 0x3F000) << 4) + ((Character & 0xFC0) << 2) + (Character & 0x3F);
	}

	return JSON_UTF8_CODE_REPLACEMENT;
}


tJsonCharacter JsonUtf8CodeGetCharacter(tJsonUtf8Code Code)
{
	if ((Code & 0xFFFFFF80) == 0)
	{
		return Code;
	}
	else if ((Code & 0xFFFFE0C0) == 0xC080)
	{
		if (Code >= 0xC280)
		{
			return ((Code & 0x1F00) >> 2) + (Code & 0x3F);
		}
	}
	else if ((Code & 0xFFF0C0C0) == 0xE08080)
	{
		if ((Code >= 0xE0A080) && ((Code < 0xEDA080) || (Code >= 0xEE8080)))
		{
			return ((Code & 0xF0000) >> 4) + ((Code & 0x3F00) >> 2) + (Code & 0x3F);
		}
	}
	else if ((Code & 0xF8C0C0C0) == 0xF0808080)
	{
		if ((Code >= 0xF0908080) && (Code < 0xF4908080))
		{
			return ((Code & 0x7000000) >> 6) + ((Code & 0x3F0000) >> 4) + ((Code & 0x3F00) >> 2) + (Code & 0x3F);
		}
	}

	return JSON_CHARACTER_REPLACEMENT;
}


bool JsonUtf8CodeIsValid(tJsonUtf8Code Code)
{
	if ((Code & 0xFFFFFF80) == 0)
	{
		return true;
	}
	else if ((Code & 0xFFFFE0C0) == 0xC080)
	{
		return Code >= 0xC280;
	}
	else if ((Code & 0xFFF0C0C0) == 0xE08080)
	{
		return (Code >= 0xE0A080) && ((Code < 0xEDA080) || (Code >= 0xEE8080));
	}
	else if ((Code & 0xF8C0C0C0) == 0xF0808080)
	{
		return (Code >= 0xF0908080) && (Code < 0xF4908080);
	}
	else
	{
		return false;
	}
}


size_t JsonUtf8CodeGetUnitLength(tJsonUtf8Code Code)
{
	if (JsonUtf8CodeIsValid(Code))
	{
		if ((Code & 0xFFFFFF80) == 0)
		{
			return 1;
		}
		else if ((Code & 0xFFFFE0C0) == 0xC080)
		{
			return 2;
		}
		else if ((Code & 0xFFF0C0C0) == 0xE08080)
		{
			return 3;
		}
		else if ((Code & 0xF8C0C0C0) == 0xF0808080)
		{
			return 4;
		}
	}
	return 0;
}


tJsonUtf8Unit JsonUtf8CodeGetUnit(tJsonUtf8Code Code, size_t Index)
{
	size_t Length = JsonUtf8CodeGetUnitLength(Code);
	if (Index < Length)
	{
		return Code >> ((Length - Index - 1) * 8);
	}
	return 0;
}


bool JsonUtf8CodeAddUnit(tJsonUtf8Code *Code, tJsonUtf8Unit Unit)
{
	if (*Code == 0)
	{
		if (((Unit >= 0x80) && (Unit < 0xC2)) || (Unit >= 0xF5))
		{
			return false;
		}
	}
	else if ((Unit & 0xC0) != 0x80)
	{
		return false;
	}
	else if ((*Code & 0xFFFFFFE0) == 0xC0)
	{
		if (*Code < 0xC2)
		{
			return false;
		}
	}
	else if ((*Code & 0xFFFFFFF0) == 0xE0)
	{
		if (((*Code == 0xE0) && (Unit < 0xA0)) || ((*Code == 0xED) && (Unit >= 0xA0)))
		{
			return false;
		}
	}
	else if ((*Code & 0xFFFFFFF8) == 0xF0)
	{
		if (((*Code == 0xF0) && (Unit < 0x90)) || ((*Code == 0xF4) && (Unit >= 0x90)) || (*Code >= 0xF5))
		{
			return false;
		}
	}
	else if ((*Code & 0xFFFFF0C0) == 0xE080)
	{
		if ((*Code < 0xE0A0) || ((*Code >= 0xEDA0) && (*Code < 0xEE80)))
		{
			return false;
		}
	}
	else if ((*Code & 0xFFFFF8C0) == 0xF080)
	{
		if ((*Code < 0xF090) || (*Code >= 0xF490))
		{
			return false;
		}
	}
	else if ((*Code & 0xFFF8C0C0) == 0xF08080)
	{
		if ((*Code < 0xF09080) || (*Code >= 0xF49080))
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	*Code = (*Code << 8) | Unit;
	return true;
}


size_t JsonUtf8GetNextCode(const tJsonUtf8Unit *Content, size_t Length, size_t Offset, tJsonUtf8Code *Code)
{
	bool IsValid = false;
	size_t n = 0;

	*Code = 0;
	if (Offset < Length)
	{
		for (n = 0; !IsValid && (n < JSON_UTF8_MAX_SIZE) && (n < Length - Offset); n++)
		{
			*Code = (*Code << 8) + Content[Offset + n];
			IsValid = JsonUtf8CodeIsValid(*Code);
		}
	}

	return IsValid ? n : 0;
}


size_t JsonUtf8GetPreviousCode(const tJsonUtf8Unit *Content, size_t Length, size_t Offset, tJsonUtf8Code *Code)
{
	bool IsValid = false;
	size_t n = 0;

	*Code = 0;
	if (Offset <= Length)
	{
		for (n = 0; !IsValid && (n < JSON_UTF8_MAX_SIZE) && (n < Offset); n++)
		{
			*Code = (Content[Offset - n - 1] << (n * 8)) + *Code;
			IsValid = JsonUtf8CodeIsValid(*Code);
		}
	}

	return IsValid ? n : 0;
}
