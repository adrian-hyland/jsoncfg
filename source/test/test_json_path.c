#include <string.h>
#include "json_path.h"
#include "test_json.h"


static bool TestJsonPathUtf8(void)
{
	tJsonPath Path;
	const uint8_t *EmptyString = (const uint8_t *)"";
	const uint8_t *NonEmptyString = (const uint8_t *)"a/b/c";
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
	const char *EmptyString = "";
	const char *NonEmptyString = "a/b/c";
	bool ok;

	Path = JsonPathAscii(NULL);

	ok = (Path.Value == NULL) && (Path.Length == 0);

	Path = JsonPathAscii(EmptyString);

	ok = ok && (Path.Value == (const uint8_t *)EmptyString) && (Path.Length == 0);

	Path = JsonPathAscii(NonEmptyString);

	ok = ok && (Path.Value == (const uint8_t *)NonEmptyString) && (Path.Length == strlen(NonEmptyString));

	return ok;
}


static bool TestJsonPathLeft(void)
{
	const uint8_t String[] = "0123456789";
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
	const uint8_t String[] = "0123456789";
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
	const uint8_t String[] = "0123456789";
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


static bool TestJsonPathGetNextCharacter(void)
{
	const char Valid[] = "abcdefghijklmnopqrstuvwxyz";
	const char Escaped[] = "\\b\\f\\n\\r\\t\\\\\\\"";
	const char Unescaped[] = "\b\f\n\r\t\\\"";
	const char Invalid[] = "\\";
	tJsonPath Path;
	size_t Offset;
	size_t Length;
	uint8_t Character;
	bool IsEscaped;
	size_t n;
	bool ok = true;

	Path = JsonPathAscii(Valid);
	for (Offset = 0, n = 0; ok && (n < sizeof(Valid) - 1); Offset = Offset + Length, n++)
	{
		Length = JsonPathGetNextCharacter(Path, Offset, &IsEscaped, &Character);
		ok = (Length != 0) && !IsEscaped && (Character == Valid[n]);
	}
	ok = ok && (JsonPathGetNextCharacter(Path, Offset, &IsEscaped, &Character) == 0);

	Path = JsonPathAscii(Escaped);
	for (Offset = 0, n = 0; ok && (n < sizeof(Unescaped) - 1); Offset = Offset + Length, n++)
	{
		Length = JsonPathGetNextCharacter(Path, Offset, &IsEscaped, &Character);
		ok = (Length != 0) && IsEscaped && (Character == Unescaped[n]);
	}
	ok = ok && (JsonPathGetNextCharacter(Path, Offset, &IsEscaped, &Character) == 0);

	Path = JsonPathAscii(Invalid);
	ok = ok && (JsonPathGetNextCharacter(Path, 0, &IsEscaped, &Character) == 0);

	return ok;
}


static bool TestJsonPathGetPreviousCharacter(void)
{
	const char Valid[] = "abcdefghijklmnopqrstuvwxyz";
	const char Escaped[] = "\\b\\f\\n\\r\\t\\\\\\\"";
	const char Unescaped[] = "\b\f\n\r\t\\\"";
	const char Invalid[] = "\\";
	tJsonPath Path;
	size_t Offset;
	size_t Length;
	uint8_t Character;
	bool IsEscaped;
	size_t n;
	bool ok = true;

	Path = JsonPathAscii(Valid);
	for (Offset = Path.Length, n = sizeof(Valid) - 1; ok && (n > 0); Offset = Offset - Length, n--)
	{
		Length = JsonPathGetPreviousCharacter(Path, Offset, &IsEscaped, &Character);
		ok = (Length != 0) && !IsEscaped && (Character == Valid[n - 1]);
	}
	ok = ok && (JsonPathGetPreviousCharacter(Path, Offset, &IsEscaped, &Character) == 0);

	Path = JsonPathAscii(Escaped);
	for (Offset = Path.Length, n = sizeof(Unescaped) - 1; ok && (n > 0); Offset = Offset - Length, n--)
	{
		Length = JsonPathGetPreviousCharacter(Path, Offset, &IsEscaped, &Character);
		ok = (Length != 0) && IsEscaped && (Character == Unescaped[n - 1]);
	}
	ok = ok && (JsonPathGetPreviousCharacter(Path, Offset, &IsEscaped, &Character) == 0);

	Path = JsonPathAscii(Invalid);
	ok = ok && (JsonPathGetPreviousCharacter(Path, Path.Length, &IsEscaped, &Character) == 0);

	return ok;
}


static bool TestJsonPathSetString(void)
{
	static const char *ValidPaths[] =
	{
		"path value",
		"escaped \\:\\/\\[\\]\\ path",
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
	size_t StringOffset;
	size_t PathOffset;
	uint8_t StringCharacter;
	uint8_t PathCharacter;
	size_t StringCharacterLength;
	size_t PathCharacterLength;
	size_t n;
	bool IsEscaped;
	bool ok;

	JsonStringSetUp(&String);

	for (ok = true, n = 0; n < sizeof(ValidPaths) / sizeof(ValidPaths[0]); n++)
	{
		Path = JsonPathAscii(ValidPaths[n]);

		ok = JsonPathSetString(Path, &String);

		for (StringOffset = 0, PathOffset = 0; ok && (PathOffset <= Path.Length); StringOffset = StringOffset + StringCharacterLength, PathOffset = PathOffset + PathCharacterLength)
		{
			PathCharacterLength = JsonPathGetNextCharacter(Path, PathOffset, &IsEscaped, &PathCharacter);
			StringCharacterLength = JsonStringGetCharacter(&String, StringOffset, &StringCharacter);

			ok = (PathCharacterLength != 0) && (StringCharacterLength != 0);
			ok = ok && (StringCharacter == PathCharacter);
		}
	}

	for (n = 0; n < sizeof(InvalidPaths) / sizeof(InvalidPaths[0]); n++)
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
		"escaped \\:\\/\\[\\]\\ path",
	};
	tJsonString String;
	size_t PathLength;
	size_t n;
	bool ok;

	JsonStringSetUp(&String);

	for (ok = true, n = 0; n < sizeof(Paths) / sizeof(Paths[0]); n++)
	{
		PathLength = strlen((const char *)Paths[n]) - 1;

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
		{ "/",                           1,  json_TypeObject,       NULL                     },
		{ "/object",                     1,  json_TypeObject,       NULL                     },
		{ "  /",                         3,  json_TypeObject,       NULL                     },
		{ "/  ",                         3,  json_TypeObject,       NULL                     },
		{ "  /  ",                       5,  json_TypeObject,       NULL                     },
		{ "  /object",                   3,  json_TypeObject,       NULL                     },
		{ "  /object  ",                 3,  json_TypeObject,       NULL                     },
		{ "/  object",                   3,  json_TypeObject,       NULL                     },
		{ "/  object  ",                 3,  json_TypeObject,       NULL                     },
		{ "  /  object",                 5,  json_TypeObject,       NULL                     },
		{ "  /  object  ",               5,  json_TypeObject,       NULL                     },
		{ "key",                         3,  json_TypeKey,          "key"                    },
		{ "key/object",                  3,  json_TypeKey,          "key"                    },
		{ "key[array]",                  3,  json_TypeKey,          "key"                    },
		{ "key:value",                   3,  json_TypeKey,          "key"                    },
		{ "key\\/\\:\\[\\]\\\\",        13,  json_TypeKey,          "key\\/\\:\\[\\]\\\\"    },
		{ "  key",                       5,  json_TypeKey,          "key"                    },
		{ "key  ",                       5,  json_TypeKey,          "key"                    },
		{ "  key  ",                     7,  json_TypeKey,          "key"                    },
		{ "  key/object",                5,  json_TypeKey,          "key"                    },
		{ "  key/object  ",              5,  json_TypeKey,          "key"                    },
		{ "key  /object",                5,  json_TypeKey,          "key"                    },
		{ "key  /object  ",              5,  json_TypeKey,          "key"                    },
		{ "  key  /object",              7,  json_TypeKey,          "key"                    },
		{ "  key  /object  ",            7,  json_TypeKey,          "key"                    },
		{ "key  [array]",                5,  json_TypeKey,          "key"                    },
		{ "key  [array]  ",              5,  json_TypeKey,          "key"                    },
		{ "  key[array]",                5,  json_TypeKey,          "key"                    },
		{ "  key[array]  ",              5,  json_TypeKey,          "key"                    },
		{ "  key  [array]",              7,  json_TypeKey,          "key"                    },
		{ "  key  [array]  ",            7,  json_TypeKey,          "key"                    },
		{ "  key:value",                 5,  json_TypeKey,          "key"                    },
		{ "  key:value  ",               5,  json_TypeKey,          "key"                    },
		{ "key  :value",                 5,  json_TypeKey,          "key"                    },
		{ "key  :value  ",               5,  json_TypeKey,          "key"                    },
		{ "  key  :value",               7,  json_TypeKey,          "key"                    },
		{ "  key  :value  ",             7,  json_TypeKey,          "key"                    },
		{ "\"key\"",                     5,  json_TypeKey,          "key"                    },
		{ "\"key\"/object",              5,  json_TypeKey,          "key"                    },
		{ "\"key\"/object  ",            5,  json_TypeKey,          "key"                    },
		{ "\"key\"[array]",              5,  json_TypeKey,          "key"                    },
		{ "\"key\"[array]  ",            5,  json_TypeKey,          "key"                    },
		{ "\"key\":value",               5,  json_TypeKey,          "key"                    },
		{ "\"key\":value  ",             5,  json_TypeKey,          "key"                    },
		{ "\"key\\/\\:\\[\\]\\\\\"",     15, json_TypeKey,          "key\\/\\:\\[\\]\\\\"    },
		{ "  \"key\"",                   7,  json_TypeKey,          "key"                    },
		{ "\"key\"  ",                   7,  json_TypeKey,          "key"                    },
		{ "  \"key\"  ",                 9,  json_TypeKey,          "key"                    },
		{ "  \"key\"/object",            7,  json_TypeKey,          "key"                    },
		{ "  \"key\"/object  ",          7,  json_TypeKey,          "key"                    },
		{ "\"key\"  /object",            7,  json_TypeKey,          "key"                    },
		{ "\"key\"  /object  ",          7,  json_TypeKey,          "key"                    },
		{ "  \"key\"  /object",          9,  json_TypeKey,          "key"                    },
		{ "  \"key\"  /object  ",        9,  json_TypeKey,          "key"                    },
		{ "  \"key\"[array]",            7,  json_TypeKey,          "key"                    },
		{ "  \"key\"[array]  ",          7,  json_TypeKey,          "key"                    },
		{ "\"key\"  [array]",            7,  json_TypeKey,          "key"                    },
		{ "\"key\"  [array]  ",          7,  json_TypeKey,          "key"                    },
		{ "  \"key\"  [array]",          9,  json_TypeKey,          "key"                    },
		{ "  \"key\"  [array]  ",        9,  json_TypeKey,          "key"                    },
		{ "  \"key\":value",             7,  json_TypeKey,          "key"                    },
		{ "  \"key\":value  ",           7,  json_TypeKey,          "key"                    },
		{ "\"key\"  :value",             7,  json_TypeKey,          "key"                    },
		{ "\"key\"  :value  ",           7,  json_TypeKey,          "key"                    },
		{ "  \"key\"  :value",           9,  json_TypeKey,          "key"                    },
		{ "  \"key\"  :value  ",         9,  json_TypeKey,          "key"                    },
		{ ":value",                      6,  json_TypeValueLiteral, "value"                  },
		{ ":value\\/\\:\\[\\]\\\\",      16, json_TypeValueLiteral, "value\\/\\:\\[\\]\\\\"  },
		{ ":  value",                    8,  json_TypeValueLiteral, "value"                  },
		{ ":value  ",                    8,  json_TypeValueLiteral, "value"                  },
		{ ":  value  ",                  10, json_TypeValueLiteral, "value"                  },
		{ "  :value",                    8,  json_TypeValueLiteral, "value"                  },
		{ "  :value  ",                  10, json_TypeValueLiteral, "value"                  },
		{ "  :  value",                  10, json_TypeValueLiteral, "value"                  },
		{ "  :  value  ",                12, json_TypeValueLiteral, "value"                  },
		{ ":\"value\"",                  8,  json_TypeValueString,  "value"                  },
		{ ":\"value\\/\\:\\[\\]\\\\\"",  18, json_TypeValueString,  "value\\/\\:\\[\\]\\\\"  },
		{ ":  \"value\"",                10, json_TypeValueString,  "value"                  },
		{ ":\"value\"  ",                10, json_TypeValueString,  "value"                  },
		{ ":  \"value\"  ",              12, json_TypeValueString,  "value"                  },
		{ "  :\"value\"",                10, json_TypeValueString,  "value"                  },
		{ "  :\"value\"  ",              12, json_TypeValueString,  "value"                  },
		{ "  :  \"value\"",              12, json_TypeValueString,  "value"                  },
		{ "  :  \"value\"  ",            14, json_TypeValueString,  "value"                  },
		{ "[value]",                     7,  json_TypeArray,        "value"                  },
		{ "[value\\/\\:\\[\\]\\\\]",     17, json_TypeArray,        "value\\/\\:\\[\\]\\\\"  },
		{ "[  value]",                   9,  json_TypeArray,        "value"                  },
		{ "[value  ]",                   9,  json_TypeArray,        "value"                  },
		{ "[  value  ]",                 11, json_TypeArray,        "value"                  },
		{ "[/array[key]]",               13, json_TypeArray,        "/array[key]"            },
		{ "[  /array[key]]",             15, json_TypeArray,        "/array[key]"            },
		{ "[/array[key]  ]",             15, json_TypeArray,        "/array[key]"            },
		{ "[  /array[key]  ]",           17, json_TypeArray,        "/array[key]"            },
		{ "  [value]",                   9,  json_TypeArray,        "value"                  },
		{ "[value]  ",                   9,  json_TypeArray,        "value"                  },
		{ "  [value]  ",                 11, json_TypeArray,        "value"                  },
		{ "  [  value]",                 11, json_TypeArray,        "value"                  },
		{ "[  value]  ",                 11, json_TypeArray,        "value"                  },
		{ "  [  value]  ",               13, json_TypeArray,        "value"                  },
		{ "  [value  ]",                 11, json_TypeArray,        "value"                  },
		{ "[value  ]  ",                 11, json_TypeArray,        "value"                  },
		{ "  [value  ]  ",               13, json_TypeArray,        "value"                  },
		{ "  [  value  ]",               13, json_TypeArray,        "value"                  },
		{ "[  value  ]  ",               13, json_TypeArray,        "value"                  },
		{ "  [  value  ]  ",             15, json_TypeArray,        "value"                  },
		{ "  [/array[key]]",             15, json_TypeArray,        "/array[key]"            },
		{ "[/array[key]]  ",             15, json_TypeArray,        "/array[key]"            },
		{ "  [/array[key]]  ",           17, json_TypeArray,        "/array[key]"            },
		{ "  [  /array[key]]",           17, json_TypeArray,        "/array[key]"            },
		{ "[  /array[key]]  ",           17, json_TypeArray,        "/array[key]"            },
		{ "  [  /array[key]]  ",         19, json_TypeArray,        "/array[key]"            },
		{ "  [/array[key]  ]",           17, json_TypeArray,        "/array[key]"            },
		{ "[/array[key]  ]  ",           17, json_TypeArray,        "/array[key]"            },
		{ "  [/array[key]  ]  ",         19, json_TypeArray,        "/array[key]"            },
		{ "  [  /array[key]  ]",         19, json_TypeArray,        "/array[key]"            },
		{ "[  /array[key]  ]  ",         19, json_TypeArray,        "/array[key]"            },
		{ "  [  /array[key]  ]  ",       21, json_TypeArray,        "/array[key]"            },
		{ "[",                           0                                                   },
		{ "  [",                         0                                                   },
		{ "[  ",                         0                                                   },
		{ "  [  ",                       0                                                   },
		{ "[array\\]",                   0                                                   },
		{ "  [array\\]",                 0                                                   },
		{ "[array\\]  ",                 0                                                   },
		{ "  [array\\]  ",               0                                                   },
		{ "[array\\\\\\]",               0                                                   },
		{ "  [array\\\\\\]",             0                                                   },
		{ "[array\\\\\\]  ",             0                                                   },
		{ "  [array\\\\\\]  ",           0                                                   },
		{ "]",                           0                                                   },
		{ "  ]",                         0                                                   },
		{ "]  ",                         0                                                   },
		{ "  ]  ",                       0                                                   },
		{ "]/key",                       0                                                   },
		{ "  ]/key",                     0                                                   },
		{ "]/key  ",                     0                                                   },
		{ "  ]/key  ",                   0                                                   },
		{ "\"",                          0                                                   },
		{ "  \"",                        0                                                   },
		{ "\"  ",                        0                                                   },
		{ "  \"  ",                      0                                                   },
		{ "value\"",                     0                                                   },
		{ "  value\"",                   0                                                   },
		{ "value\"  ",                   0                                                   },
		{ "  value\"  ",                 0                                                   },
		{ "\"value",                     0                                                   },
		{ "  \"value",                   0                                                   },
		{ "\"value  ",                   0                                                   },
		{ "  \"value  ",                 0                                                   },
		{ "\"value\\\"",                 0                                                   },
		{ "  \"value\\\"",               0                                                   },
		{ "\"value\\\"  ",               0                                                   },
		{ "  \"value\\\"  ",             0                                                   },
		{ "\"value\\\\\\\"",             0                                                   },
		{ "  \"value\\\\\\\"",           0                                                   },
		{ "\"value\\\\\\\"  ",           0                                                   },
		{ "  \"value\\\\\\\"  ",         0                                                   },
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
