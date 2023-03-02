#include "json_utf16.h"


tJsonUtf16Code JsonUtf16Code(tJsonCharacter Character)
{
	if (Character < 0x10000)
	{
		if ((Character < 0xD800) || (Character > 0xDFFF))
		{
			return Character;
		}
	}
	else if (Character < 0x110000)
	{
		Character = Character - 0x10000;
		return 0xD800DC00 + ((Character << 6) & 0x3FF0000) + (Character & 0x3FF);
	}

	return JSON_UTF16_CODE_REPLACEMENT;
}


tJsonCharacter JsonUtf16CodeGetCharacter(tJsonUtf16Code Code)
{
	if (Code < 0x10000)
	{
		if ((Code < 0xD800) || (Code > 0xDFFF))
		{
			return Code;
		}
	}
	else if ((((Code & 0xFC000000) >= 0xD8000000) && ((Code & 0xFC000000) <= 0xDB000000)) &&
		      (((Code & 0x0000FC00) >= 0x0000DC00) && ((Code & 0x0000FC00) <= 0x0000DF00)))
	{
		return 0x10000 + ((Code & 0x3FF0000) >> 6) + (Code & 0x3FF);
	}

	return JSON_CHARACTER_REPLACEMENT;
}


bool JsonUtf16CodeIsValid(tJsonUtf16Code Code)
{
	if (Code < 0x10000)
	{
		return (Code < 0xD800) || (Code > 0xDFFF);
	}
	else
	{
		return ((((Code & 0xFC000000) >= 0xD8000000) && ((Code & 0xFC000000) <= 0xDB000000)) &&
		        (((Code & 0x0000FC00) >= 0x0000DC00) && ((Code & 0x0000FC00) <= 0x0000DF00)));
	}
}


bool JsonUtf16UnitIsHighSurrogate(tJsonUtf16Unit Unit)
{
	return (Unit >= 0xD800) && (Unit < 0xDC00);
}


bool JsonUtf16UnitIsLowSurrogate(tJsonUtf16Unit Unit)
{
	return (Unit >= 0xDC00) && (Unit < 0xE000);
}


bool JsonUtf16UnitSetNibble(tJsonUtf16Unit *Code, size_t Index, uint8_t Nibble)
{
	if ((Index < 4) && (Nibble < 0x10))
	{
		Index = 3 - Index;
		*Code = (*Code & ~(0xF << (Index * 4))) | (Nibble << (Index * 4));
		return true;
	}

	return false;
}


bool JsonUtf16CodeAddUnit(tJsonUtf16Code *Code, tJsonUtf16Unit Unit)
{
	if (*Code == 0)
	{
		if (JsonUtf16UnitIsLowSurrogate(Unit))
		{
			return false;
		}
	}
	else if (*Code < 0x10000)
	{
		if (!JsonUtf16UnitIsHighSurrogate(*Code) || !JsonUtf16UnitIsLowSurrogate(Unit))
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	*Code = (*Code << 16) + Unit;
	return true;
}


size_t JsonUtf16CodeGetNibbleLength(tJsonUtf16Code Code)
{
	if (JsonUtf16CodeIsValid(Code))
	{
		return (Code < 0x10000) ? 4 : 8;
	}
	return 0;
}


uint8_t JsonUtf16CodeGetNibble(tJsonUtf16Code Code, size_t Index)
{
	size_t Length = JsonUtf16CodeGetNibbleLength(Code);
	if (Index < Length)
	{
		return (Code >> ((Length - Index - 1) * 4)) & 0x0F;
	}
	return 0;
}


bool JsonUtf16CodeAddNibble(tJsonUtf16Code *Code, uint8_t Nibble)
{
	if (Nibble > 0x0F)
	{
		return false;
	}
	else if ((*Code & 0xFFFFF000) == 0)
	{
		if ((*Code >= 0xDC0) && (*Code < 0xE00))
		{
			return false;
		}
	}
	else if ((*Code & 0xFFFFF800) == 0xD800)
	{
		if ((*Code >= 0xDC00) || (Nibble != 0x0D))
		{
			return false;
		}
	}
	else if ((*Code & 0xFFFF800F) == 0xD800D)
	{
		if ((*Code >= 0xDC00D) || (Nibble < 0x0C))
		{
			return false;
		}
	}
	else if ((*Code & 0xFFF800F8) == 0xD800D8)
	{
		if (*Code >= 0xDC00D8)
		{
			return false;
		}
	}
	else if ((*Code & 0xFF800F80) == 0xD800D80)
	{
		if (*Code >= 0xDC00D80)
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	*Code = (*Code << 4) + Nibble;
	return true;
}
