#include "json_parse.h"
#include "test_json.h"


static bool TestJsonValueUtf8Valid(void)
{
	tJsonElement Root;
	tJsonParse Parse;
	bool ok;

	// TODO: should we allow parsing of control characters in values? or should we ensure that they always get escaped?
	JsonElementSetUp(&Root);
	JsonParseSetUp(&Parse, false, &Root);

	ok = (JsonParse(&Parse, '"') == JSON_PARSE_INCOMPLETE);

	for (uint32_t code = 0x20; ok && (code < 0x80); code++)
	{
		if ((code != '\\') && (code != '"'))
		{
			ok = ok && (JsonParse(&Parse, code) == JSON_PARSE_INCOMPLETE);
		}
	}

	for (uint32_t code = 0x80; ok && (code < 0x800); code++)
	{
		ok = ok && (JsonParse(&Parse, 0xC0 + (code >> 6)) == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0x80 + (code & 0x3F)) == JSON_PARSE_INCOMPLETE);
	}

	for (uint32_t code = 0x800; ok && (code < 0xD800); code++)
	{
		ok = ok && (JsonParse(&Parse, 0xE0 + (code >> 12)) == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0x80 + ((code >> 6) & 0x3F)) == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0x80 + (code & 0x3F)) == JSON_PARSE_INCOMPLETE);
	}

	for (uint32_t code = 0xE000; ok && (code < 0x10000); code++)
	{
		ok = ok && (JsonParse(&Parse, 0xE0 + (code >> 12)) == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0x80 + ((code >> 6) & 0x3F)) == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0x80 + (code & 0x3F)) == JSON_PARSE_INCOMPLETE);
	}

	for (uint32_t code = 0x10000; ok && (code < 0x110000); code++)
	{
		ok = ok && (JsonParse(&Parse, 0xF0 + (code >> 18)) == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0x80 + ((code >> 12) & 0x3F)) == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0x80 + ((code >> 6) & 0x3F)) == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0x80 + (code & 0x3F)) == JSON_PARSE_INCOMPLETE);
	}

	ok = ok && (JsonParse(&Parse, '"') == JSON_PARSE_INCOMPLETE);
	ok = ok && (JsonParse(&Parse, '\0') == JSON_PARSE_COMPLETE);

	JsonParseCleanUp(&Parse);
	JsonElementCleanUp(&Root);

	return ok;
}


static bool TestJsonValueUtf8Invalid(void)
{
	tJsonElement Root;
	tJsonParse Parse;
	bool ok = true;

	for (uint32_t code = 0x80; ok && (code < 0x100); code++)
	{
		JsonElementSetUp(&Root);
		JsonParseSetUp(&Parse, false, &Root);

		ok = (JsonParse(&Parse, '"') == JSON_PARSE_INCOMPLETE);
		if (((code & 0xE0) == 0xC0) || ((code & 0xF0) == 0xE0) || ((code & 0xF8) == 0xF0))
		{
			ok = ok && (JsonParse(&Parse, code) == JSON_PARSE_INCOMPLETE);
			ok = ok && (JsonParse(&Parse, '"') == JSON_PARSE_ERROR);
		}
		else
		{
			ok = ok && (JsonParse(&Parse, code) == JSON_PARSE_ERROR);
		}

		JsonParseCleanUp(&Parse);
		JsonElementCleanUp(&Root);
	}

	for (uint32_t code = 0x8000; ok && (code < 0x10000); code++)
	{
		if ((code & 0xE0C0) == 0xC080)
		{
			continue;
		}

		JsonElementSetUp(&Root);
		JsonParseSetUp(&Parse, false, &Root);

		if (((code & 0xF0C0) == 0xE080) || ((code & 0xF8C0) == 0xF080))
		{
			ok = ok && (JsonParse(&Parse, code >> 8) == JSON_PARSE_INCOMPLETE);
			ok = ok && (JsonParse(&Parse, code) == JSON_PARSE_INCOMPLETE);
			ok = ok && (JsonParse(&Parse, '"') == JSON_PARSE_ERROR);
		}
		else if (((code & 0xE000) == 0xC000) || ((code & 0xF000) == 0xE000) || ((code & 0xF800) == 0xF000))
		{
			ok = ok && (JsonParse(&Parse, code >> 8) == JSON_PARSE_INCOMPLETE);
			ok = ok && (JsonParse(&Parse, code) == JSON_PARSE_ERROR);
		}
		else
		{
			ok = ok && (JsonParse(&Parse, code >> 8) == JSON_PARSE_ERROR);
		}

		JsonParseCleanUp(&Parse);
		JsonElementCleanUp(&Root);
	}

	for (uint32_t code = 0x800000; ok && (code < 0x1000000); code++)
	{
		if (((code & 0xE0C000) == 0xC08000) || ((code & 0xF0C0C0) == 0xE08080))
		{
			continue;
		}

		JsonElementSetUp(&Root);
		JsonParseSetUp(&Parse, false, &Root);

		if ((code & 0xF8C0C0) == 0xF08080)
		{
			ok = ok && (JsonParse(&Parse, code >> 16) == JSON_PARSE_INCOMPLETE);
			ok = ok && (JsonParse(&Parse, code >> 8) == JSON_PARSE_INCOMPLETE);
			ok = ok && (JsonParse(&Parse, code) == JSON_PARSE_INCOMPLETE);
			ok = ok && (JsonParse(&Parse, '"') == JSON_PARSE_ERROR);
		}
		else if (((code & 0xF0C000) == 0xE08000) || (code & 0xF8C000) == 0xF08000)
		{
			ok = ok && (JsonParse(&Parse, code >> 16) == JSON_PARSE_INCOMPLETE);
			ok = ok && (JsonParse(&Parse, code >> 8) == JSON_PARSE_INCOMPLETE);
			ok = ok && (JsonParse(&Parse, code) == JSON_PARSE_ERROR);
		}
		else if (((code & 0xE00000) == 0xC00000) || ((code & 0xF00000) == 0xE00000) || ((code & 0xF80000) == 0xF00000))
		{
			ok = ok && (JsonParse(&Parse, code >> 16) == JSON_PARSE_INCOMPLETE);
			ok = ok && (JsonParse(&Parse, code >> 8) == JSON_PARSE_ERROR);
		}
		else
		{
			ok = ok && (JsonParse(&Parse, code >> 8) == JSON_PARSE_ERROR);
		}

		JsonParseCleanUp(&Parse);
		JsonElementCleanUp(&Root);
	}

	for (uint32_t code = 0x80000000; ok && (code != 0); code++)
	{
		if (((code & 0xE0C00000) == 0xC0800000) || ((code & 0xF0C0C000) == 0xE0808000) || ((code & 0xF8C0C0C0) == 0xF0808080))
		{
			continue;
		}

		JsonElementSetUp(&Root);
		JsonParseSetUp(&Parse, false, &Root);

		if ((code & 0xF8C0C0C0) == 0xF0808000)
		{
			ok = ok && (JsonParse(&Parse, code >> 24) == JSON_PARSE_INCOMPLETE);
			ok = ok && (JsonParse(&Parse, code >> 16) == JSON_PARSE_INCOMPLETE);
			ok = ok && (JsonParse(&Parse, code >> 8) == JSON_PARSE_INCOMPLETE);
			ok = ok && (JsonParse(&Parse, code) == JSON_PARSE_ERROR);
		}
		else if (((code & 0xF0C00000) == 0xE0800000) || (code & 0xF8C0C000) == 0xF0800000)
		{
			ok = ok && (JsonParse(&Parse, code >> 16) == JSON_PARSE_INCOMPLETE);
			ok = ok && (JsonParse(&Parse, code >> 8) == JSON_PARSE_INCOMPLETE);
			ok = ok && (JsonParse(&Parse, code) == JSON_PARSE_ERROR);
		}
		else if (((code & 0xE0000000) == 0xC0000000) || ((code & 0xF0000000) == 0xE0000000) || ((code & 0xF8000000) == 0xF0000000))
		{
			ok = ok && (JsonParse(&Parse, code >> 16) == JSON_PARSE_INCOMPLETE);
			ok = ok && (JsonParse(&Parse, code >> 8) == JSON_PARSE_ERROR);
		}
		else
		{
			ok = ok && (JsonParse(&Parse, code >> 8) == JSON_PARSE_ERROR);
		}

		JsonParseCleanUp(&Parse);
		JsonElementCleanUp(&Root);
	}

	return ok;
}


static bool TestJsonValueUtf8OutOfRange(void)
{
	tJsonElement Root;
	tJsonParse Parse;
	bool ok = true;

	for (uint32_t code = 0xD800; ok && (code < 0xE000); code++)
	{
		JsonElementSetUp(&Root);
		JsonParseSetUp(&Parse, false, &Root);

		ok = (JsonParse(&Parse, '"') == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0xE0 + (code >> 12)) == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0x80 + ((code >> 6) & 0x3F)) == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0x80 + (code & 0x3F)) == JSON_PARSE_ERROR);

		JsonParseCleanUp(&Parse);
		JsonElementCleanUp(&Root);
	}

	for (uint32_t code = 0x110000; ok && (code < 0x200000); code++)
	{
		JsonElementSetUp(&Root);
		JsonParseSetUp(&Parse, false, &Root);

		ok = ok && (JsonParse(&Parse, 0xF0 + (code >> 18)) == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0x80 + ((code >> 12) & 0x3F)) == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0x80 + ((code >> 6) & 0x3F)) == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0x80 + (code & 0x3F)) == JSON_PARSE_ERROR);

		JsonParseCleanUp(&Parse);
		JsonElementCleanUp(&Root);
	}

	return ok;
}


static bool TestJsonValueUtf8Overlong(void)
{
	tJsonElement Root;
	tJsonParse Parse;
	bool ok = true;

	for (uint32_t code = 0; ok && (code < 0x80); code++)
	{
		JsonElementSetUp(&Root);
		JsonParseSetUp(&Parse, false, &Root);

		ok = ok && (JsonParse(&Parse, 0xC0 + (code >> 6)) == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0x80 + (code & 0x3F)) == JSON_PARSE_ERROR);

		JsonParseCleanUp(&Parse);
		JsonElementCleanUp(&Root);
	}

	for (uint32_t code = 0; ok && (code < 0x800); code++)
	{
		JsonElementSetUp(&Root);
		JsonParseSetUp(&Parse, false, &Root);

		ok = ok && (JsonParse(&Parse, 0xE0 + (code >> 12)) == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0x80 + ((code >> 6) & 0x3F)) == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0x80 + (code & 0x3F)) == JSON_PARSE_ERROR);

		JsonParseCleanUp(&Parse);
		JsonElementCleanUp(&Root);
	}

	for (uint32_t code = 0; ok && (code < 0x10000); code++)
	{
		JsonElementSetUp(&Root);
		JsonParseSetUp(&Parse, false, &Root);

		ok = ok && (JsonParse(&Parse, 0xF0 + (code >> 18)) == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0x80 + ((code >> 12) & 0x3F)) == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0x80 + ((code >> 6) & 0x3F)) == JSON_PARSE_INCOMPLETE);
		ok = ok && (JsonParse(&Parse, 0x80 + (code & 0x3F)) == JSON_PARSE_ERROR);

		JsonParseCleanUp(&Parse);
		JsonElementCleanUp(&Root);
	}

	return ok;
}


static const tTestCase TestCaseJsonValueUtf8[] =
{
   { "Valid",      TestJsonValueUtf8Valid      },
	{ "Invalid",    TestJsonValueUtf8Invalid    },
	{ "OutOfRange", TestJsonValueUtf8OutOfRange },
	{ "Overlong",   TestJsonValueUtf8Overlong   }
};


const tTest TestJsonValueUtf8 =
{
    "JsonValueUtf8",
    TestCaseJsonValueUtf8,
    sizeof(TestCaseJsonValueUtf8) / sizeof(TestCaseJsonValueUtf8[0])
};
