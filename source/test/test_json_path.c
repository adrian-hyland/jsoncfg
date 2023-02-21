#include <string.h>
#include "json_path.h"
#include "test_json.h"


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
	size_t StringIndex;
	size_t PathLength;
	size_t PathIndex;
	uint8_t Character;
	size_t CharacterLength;
	size_t n;
	bool ok;

	JsonStringSetUp(&String);

	for (ok = true, n = 0; n < sizeof(ValidPaths) / sizeof(ValidPaths[0]); n++)
	{
		PathLength = strlen(ValidPaths[n]);
		ok = JsonPathSetString((const uint8_t *)ValidPaths[n], PathLength, &String);

		for (StringIndex = 0, PathIndex = 0; ok && (PathIndex <= PathLength); PathIndex++)
		{
			if (ValidPaths[n][PathIndex] != '\\')
			{
				CharacterLength = JsonStringGetCharacter(&String, StringIndex, &Character);
				ok = (CharacterLength != 0);
				ok = ok && (Character == ValidPaths[n][PathIndex]);
				StringIndex = StringIndex + CharacterLength;
			}
		}
	}

	for (n = 0; n < sizeof(InvalidPaths) / sizeof(InvalidPaths[0]); n++)
	{
		PathLength = strlen(InvalidPaths[n]);
		ok = !JsonPathSetString((const uint8_t *)InvalidPaths[n], PathLength, &String);
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
		PathLength = strlen(Paths[n]) - 1;

		ok = JsonPathSetString((const uint8_t *)Paths[n], PathLength, &String);

		ok = ok && !JsonPathCompareString((const uint8_t *)Paths[n], PathLength - 1, &String);

		ok = ok && JsonPathCompareString((const uint8_t *)Paths[n], PathLength, &String);

		ok = ok && !JsonPathCompareString((const uint8_t *)Paths[n], PathLength + 1, &String);
	}

	JsonStringCleanUp(&String);

	return ok;
}


static bool TestJsonPathGetComponent(void)
{
	static const struct
	{
		const uint8_t *Path;
		size_t         Length;
		tJsonType      Type;
		const uint8_t *Component;
	} TestPaths[] =
	{
		{ (const uint8_t *)"/",                           1,  json_TypeObject,       NULL                                      },
		{ (const uint8_t *)"/object",                     1,  json_TypeObject,       NULL                                      },
		{ (const uint8_t *)"  /",                         3,  json_TypeObject,       NULL                                      },
		{ (const uint8_t *)"/  ",                         3,  json_TypeObject,       NULL                                      },
		{ (const uint8_t *)"  /  ",                       5,  json_TypeObject,       NULL                                      },
		{ (const uint8_t *)"  /object",                   3,  json_TypeObject,       NULL                                      },
		{ (const uint8_t *)"/  object",                   1,  json_TypeObject,       NULL                                      },
		{ (const uint8_t *)"  /  object",                 3,  json_TypeObject,       NULL                                      },
		{ (const uint8_t *)"key",                         3,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"key/object",                  3,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"key[array]",                  3,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"key:value",                   3,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"key\\/\\:\\[\\]\\\\",        13,  json_TypeKey,          (const uint8_t *)"key\\/\\:\\[\\]\\\\"    },
		{ (const uint8_t *)"  key",                       5,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"key  ",                       5,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"  key  ",                     7,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"  key/object",                5,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"key  /object",                5,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"  key  /object",              7,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"key  [array]",                5,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"  key[array]",                5,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"  key  [array]",              7,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"  key:value",                 5,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"key  :value",                 5,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"  key  :value",               7,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"\"key\"",                     5,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"\"key\"/object",              5,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"\"key\"[array]",              5,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"\"key\":value",               5,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"\"key\\/\\:\\[\\]\\\\\"",     15, json_TypeKey,          (const uint8_t *)"key\\/\\:\\[\\]\\\\"    },
		{ (const uint8_t *)"  \"key\"",                   7,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"\"key\"  ",                   7,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"  \"key\"  ",                 9,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"  \"key\"/object",            7,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"\"key\"  /object",            7,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"  \"key\"  /object",          9,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"  \"key\"[array]",            7,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"\"key\"  [array]",            7,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"  \"key\"  [array]",          9,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"  \"key\":value",             7,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"\"key\"  :value",             7,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)"  \"key\"  :value",           9,  json_TypeKey,          (const uint8_t *)"key"                    },
		{ (const uint8_t *)":value",                      6,  json_TypeValueLiteral, (const uint8_t *)"value"                  },
		{ (const uint8_t *)":value\\/\\:\\[\\]\\\\",      16, json_TypeValueLiteral, (const uint8_t *)"value\\/\\:\\[\\]\\\\"  },
		{ (const uint8_t *)":  value",                    8,  json_TypeValueLiteral, (const uint8_t *)"value"                  },
		{ (const uint8_t *)":value  ",                    8,  json_TypeValueLiteral, (const uint8_t *)"value"                  },
		{ (const uint8_t *)":  value  ",                  10, json_TypeValueLiteral, (const uint8_t *)"value"                  },
		{ (const uint8_t *)"  :value",                    8,  json_TypeValueLiteral, (const uint8_t *)"value"                  },
		{ (const uint8_t *)"  :value  ",                  10, json_TypeValueLiteral, (const uint8_t *)"value"                  },
		{ (const uint8_t *)"  :  value",                  10, json_TypeValueLiteral, (const uint8_t *)"value"                  },
		{ (const uint8_t *)"  :  value  ",                12, json_TypeValueLiteral, (const uint8_t *)"value"                  },
		{ (const uint8_t *)":\"value\"",                  8,  json_TypeValueString,  (const uint8_t *)"value"                  },
		{ (const uint8_t *)":\"value\\/\\:\\[\\]\\\\\"",  18, json_TypeValueString,  (const uint8_t *)"value\\/\\:\\[\\]\\\\"  },
		{ (const uint8_t *)":  \"value\"",                10, json_TypeValueString,  (const uint8_t *)"value"                  },
		{ (const uint8_t *)":\"value\"  ",                10, json_TypeValueString,  (const uint8_t *)"value"                  },
		{ (const uint8_t *)":  \"value\"  ",              12, json_TypeValueString,  (const uint8_t *)"value"                  },
		{ (const uint8_t *)"  :\"value\"",                10, json_TypeValueString,  (const uint8_t *)"value"                  },
		{ (const uint8_t *)"  :\"value\"  ",              12, json_TypeValueString,  (const uint8_t *)"value"                  },
		{ (const uint8_t *)"  :  \"value\"",              12, json_TypeValueString,  (const uint8_t *)"value"                  },
		{ (const uint8_t *)"  :  \"value\"  ",            14, json_TypeValueString,  (const uint8_t *)"value"                  },
		{ (const uint8_t *)"[value]",                     7,  json_TypeArray,        (const uint8_t *)"value"                  },
		{ (const uint8_t *)"[value\\/\\:\\[\\]\\\\]",     17, json_TypeArray,        (const uint8_t *)"value\\/\\:\\[\\]\\\\"  },
		{ (const uint8_t *)"[  value]",                   9,  json_TypeArray,        (const uint8_t *)"value"                  },
		{ (const uint8_t *)"[value  ]",                   9,  json_TypeArray,        (const uint8_t *)"value"                  },
		{ (const uint8_t *)"[  value  ]",                 11, json_TypeArray,        (const uint8_t *)"value"                  },
		{ (const uint8_t *)"[/array[key]]",               13, json_TypeArray,        (const uint8_t *)"/array[key]"            },
		{ (const uint8_t *)"[  /array[key]]",             15, json_TypeArray,        (const uint8_t *)"/array[key]"            },
		{ (const uint8_t *)"[/array[key]  ]",             15, json_TypeArray,        (const uint8_t *)"/array[key]"            },
		{ (const uint8_t *)"[  /array[key]  ]",           17, json_TypeArray,        (const uint8_t *)"/array[key]"            },
		{ (const uint8_t *)"  [value]",                   9,  json_TypeArray,        (const uint8_t *)"value"                  },
		{ (const uint8_t *)"[value]  ",                   9,  json_TypeArray,        (const uint8_t *)"value"                  },
		{ (const uint8_t *)"  [value]  ",                 11, json_TypeArray,        (const uint8_t *)"value"                  },
		{ (const uint8_t *)"  [  value]",                 11, json_TypeArray,        (const uint8_t *)"value"                  },
		{ (const uint8_t *)"[  value]  ",                 11, json_TypeArray,        (const uint8_t *)"value"                  },
		{ (const uint8_t *)"  [  value]  ",               13, json_TypeArray,        (const uint8_t *)"value"                  },
		{ (const uint8_t *)"  [value  ]",                 11, json_TypeArray,        (const uint8_t *)"value"                  },
		{ (const uint8_t *)"[value  ]  ",                 11, json_TypeArray,        (const uint8_t *)"value"                  },
		{ (const uint8_t *)"  [value  ]  ",               13, json_TypeArray,        (const uint8_t *)"value"                  },
		{ (const uint8_t *)"  [  value  ]",               13, json_TypeArray,        (const uint8_t *)"value"                  },
		{ (const uint8_t *)"[  value  ]  ",               13, json_TypeArray,        (const uint8_t *)"value"                  },
		{ (const uint8_t *)"  [  value  ]  ",             15, json_TypeArray,        (const uint8_t *)"value"                  },
		{ (const uint8_t *)"  [/array[key]]",             15, json_TypeArray,        (const uint8_t *)"/array[key]"            },
		{ (const uint8_t *)"[/array[key]]  ",             15, json_TypeArray,        (const uint8_t *)"/array[key]"            },
		{ (const uint8_t *)"  [/array[key]]  ",           17, json_TypeArray,        (const uint8_t *)"/array[key]"            },
		{ (const uint8_t *)"  [  /array[key]]",           17, json_TypeArray,        (const uint8_t *)"/array[key]"            },
		{ (const uint8_t *)"[  /array[key]]  ",           17, json_TypeArray,        (const uint8_t *)"/array[key]"            },
		{ (const uint8_t *)"  [  /array[key]]  ",         19, json_TypeArray,        (const uint8_t *)"/array[key]"            },
		{ (const uint8_t *)"  [/array[key]  ]",           17, json_TypeArray,        (const uint8_t *)"/array[key]"            },
		{ (const uint8_t *)"[/array[key]  ]  ",           17, json_TypeArray,        (const uint8_t *)"/array[key]"            },
		{ (const uint8_t *)"  [/array[key]  ]  ",         19, json_TypeArray,        (const uint8_t *)"/array[key]"            },
		{ (const uint8_t *)"  [  /array[key]  ]",         19, json_TypeArray,        (const uint8_t *)"/array[key]"            },
		{ (const uint8_t *)"[  /array[key]  ]  ",         19, json_TypeArray,        (const uint8_t *)"/array[key]"            },
		{ (const uint8_t *)"  [  /array[key]  ]  ",       21, json_TypeArray,        (const uint8_t *)"/array[key]"            },
		{ (const uint8_t *)"[",                           0                                                                    },
		{ (const uint8_t *)"  [",                         0                                                                    },
		{ (const uint8_t *)"[  ",                         0                                                                    },
		{ (const uint8_t *)"  [  ",                       0                                                                    },
		{ (const uint8_t *)"[array\\]",                   0                                                                    },
		{ (const uint8_t *)"  [array\\]",                 0                                                                    },
		{ (const uint8_t *)"[array\\]  ",                 0                                                                    },
		{ (const uint8_t *)"  [array\\]  ",               0                                                                    },
		{ (const uint8_t *)"[array\\\\\\]",               0                                                                    },
		{ (const uint8_t *)"  [array\\\\\\]",             0                                                                    },
		{ (const uint8_t *)"[array\\\\\\]  ",             0                                                                    },
		{ (const uint8_t *)"  [array\\\\\\]  ",           0                                                                    },
		{ (const uint8_t *)"]",                           0                                                                    },
		{ (const uint8_t *)"  ]",                         0                                                                    },
		{ (const uint8_t *)"]  ",                         0                                                                    },
		{ (const uint8_t *)"  ]  ",                       0                                                                    },
		{ (const uint8_t *)"]/key",                       0                                                                    },
		{ (const uint8_t *)"  ]/key",                     0                                                                    },
		{ (const uint8_t *)"]/key  ",                     0                                                                    },
		{ (const uint8_t *)"  ]/key  ",                   0                                                                    },
		{ (const uint8_t *)"\"",                          0                                                                    },
		{ (const uint8_t *)"  \"",                        0                                                                    },
		{ (const uint8_t *)"\"  ",                        0                                                                    },
		{ (const uint8_t *)"  \"  ",                      0                                                                    },
		{ (const uint8_t *)"value\"",                     0                                                                    },
		{ (const uint8_t *)"  value\"",                   0                                                                    },
		{ (const uint8_t *)"value\"  ",                   0                                                                    },
		{ (const uint8_t *)"  value\"  ",                 0                                                                    },
		{ (const uint8_t *)"\"value",                     0                                                                    },
		{ (const uint8_t *)"  \"value",                   0                                                                    },
		{ (const uint8_t *)"\"value  ",                   0                                                                    },
		{ (const uint8_t *)"  \"value  ",                 0                                                                    },
		{ (const uint8_t *)"\"value\\\"",                 0                                                                    },
		{ (const uint8_t *)"  \"value\\\"",               0                                                                    },
		{ (const uint8_t *)"\"value\\\"  ",               0                                                                    },
		{ (const uint8_t *)"  \"value\\\"  ",             0                                                                    },
		{ (const uint8_t *)"\"value\\\\\\\"",             0                                                                    },
		{ (const uint8_t *)"  \"value\\\\\\\"",           0                                                                    },
		{ (const uint8_t *)"\"value\\\\\\\"  ",           0                                                                    },
		{ (const uint8_t *)"  \"value\\\\\\\"  ",         0                                                                    },
	};
	tJsonType ComponentType;
	const uint8_t *Component;
	size_t ComponentLength;
	size_t Length;
	size_t n;
	bool ok;

	for (ok = true, n = 0; ok && (n < sizeof(TestPaths) / sizeof(TestPaths[0])); n++)
	{
		Length = JsonPathGetComponent(TestPaths[n].Path, strlen((const char *)TestPaths[n].Path), &ComponentType, &Component, &ComponentLength);

		ok = (Length == TestPaths[n].Length);

		if (TestPaths[n].Length != 0)
		{
			ok = ok && (ComponentType == TestPaths[n].Type);

			if (TestPaths[n].Component == NULL)
			{
				ok = ok && (Component == NULL);

				ok = ok && (ComponentLength == 0);
			}
			else
			{
				ok = ok && (Component != NULL);

				ok = ok && (ComponentLength == strlen((const char *)TestPaths[n].Component));

				ok = ok && (strncmp((const char *)Component, (const char *)TestPaths[n].Component, ComponentLength) == 0);
			}
		}
	}

	return ok;
}


static const tTestCase TestCaseJsonPath[] =
{
	{ "JsonPathSetString",     TestJsonPathSetString     },
	{ "JsonPathCompareString", TestJsonPathCompareString },
	{ "JsonPathGetComponent",  TestJsonPathGetComponent  }
};


const tTest TestJsonPath =
{
	"JsonPath",
	TestCaseJsonPath,
	sizeof(TestCaseJsonPath) / sizeof(TestCaseJsonPath[0])
};
