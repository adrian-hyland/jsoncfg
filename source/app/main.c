#include "json.h"


#ifdef DEBUG
// This string declaration serves only to illustrate using jsoncfg to set up different build configurations for VS Code.
// The 'DEBUG' and 'APP_NAME' define constants are passed to the compiler by the makefile when the application is built. Their values
// are defined based on the configuration that is being used for the build. For this project, there are just two configurations:
//  - a 'Debug' configuration
//     - the constant 'DEBUG' will be defined
//     - the constant 'RELEASE' will not be defined
//     - the constant 'APP_NAME' will be defined as 'jsoncfg (Debug)'
//  - a 'Release' configuration
//     - the constant 'DEBUG' will not be defined
//     - the constant 'RELEASE' will be defined
//     - the constant 'APP_NAME' will be defined as 'jsoncfg (Release)'
// Once you have built the release version of jsoncfg (using 'make all'), you can use 'make vscode' to add the 'Release' configuration
// to the VS Code project and 'make vscode DEBUG=1' to add a 'Debug' configuration.
// The values that are set in the VS Code project configuration are exactly the same ones that are passed to the compiler. So if you
// now select the 'Debug' configuration (at the right of the bar at the bottom of the window) then this part of the code will appear
// enabled and 'APP_NAME' will correctly resolve to 'jsoncfg (Debug)' (when you hover your mouse pointer over it). Likewise, if you
// select the 'Release' configuration then this part of the code will appear disabled.
const char AppName[] = APP_NAME;
#endif

#define JSONCFG_BUFFER_SIZE        1024

#define JSONCFG_ERROR_NONE          0
#define JSONCFG_ERROR_BAD_ARGS     -1
#define JSONCFG_ERROR_READ_VALUE   -2
#define JSONCFG_ERROR_READ_STDIN   -3
#define JSONCFG_ERROR_NO_PATH      -4
#define JSONCFG_ERROR_SET_VALUE    -5
#define JSONCFG_ERROR_WRITE_STDOUT -6


int main(int argc, const char *argv[])
{
	tJsonElement Root;
	tJsonElement Value;
	tJsonElement *Element;
	tJsonElement *ChildValue;
	tJsonCommentType CommentType = json_CommentLine;
	tJsonUtfType UtfType = json_Utf8;
	size_t IndentSize = 3;
	bool RequireBOM = false;
	int Argument;
	int Error;

	JsonElementSetUp(&Root);
	JsonElementSetUp(&Value);

	for (Error = JSONCFG_ERROR_NONE, Argument = 1; (Error == JSONCFG_ERROR_NONE) && (Argument < argc) && (argv[Argument][0] == '-'); Argument++)
	{
		if ((argv[Argument][1] == 'c') && ((argv[Argument][2] == 'n') || (argv[Argument][2] == 'l') || (argv[Argument][2] == 'b')) && (argv[Argument][3] == '\0'))
		{
			if (argv[Argument][2] == 'l')
			{
				CommentType = json_CommentLine;
			}
			else if (argv[Argument][2] == 'b')
			{
				CommentType = json_CommentBlock;
			}
			else
			{
				CommentType = json_CommentNone;
			}
		}
		else if ((argv[Argument][1] == 'i') && (argv[Argument][2] >= '0') && (argv[Argument][2] <= '9') && (argv[Argument][3] == '\0'))
		{
			IndentSize = argv[Argument][2] - '0';
		}
		else if ((argv[Argument][1] == 'u') && (argv[Argument][2] == 't') && (argv[Argument][3] == 'f'))
		{
			if ((argv[Argument][4] == '8') && (argv[Argument][5] == '\0'))
			{
				UtfType = json_Utf8;
			}
			else if ((argv[Argument][4] == '1') && (argv[Argument][5] == '6'))
			{
				if ((argv[Argument][6] == 'l') && (argv[Argument][7] == 'e') && (argv[Argument][8] == '\0'))
				{
					UtfType = json_Utf16le;
				}
				else if ((argv[Argument][6] == 'b') && (argv[Argument][7] == 'e') && (argv[Argument][8] == '\0'))
				{
					UtfType = json_Utf16be;
				}
				else
				{
					Error = JSONCFG_ERROR_BAD_ARGS;
				}
			}
			else
			{
				Error = JSONCFG_ERROR_BAD_ARGS;
			}
		}
		else if ((argv[Argument][1] == 'b') && (argv[Argument][2] == 'o') && (argv[Argument][3] == 'm') && (argv[Argument][4] == '\0'))
		{
			RequireBOM = true;
		}
		else
		{
			Error = JSONCFG_ERROR_BAD_ARGS;
		}
	}

	if (Error == JSONCFG_ERROR_NONE)
	{
		if ((argc - Argument) % 2)
		{
			Error = JSONCFG_ERROR_BAD_ARGS;
		}
		else if (!JsonReadFile(&Root, CommentType == json_CommentNone, stdin, JSONCFG_BUFFER_SIZE))
		{
			Error = JSONCFG_ERROR_READ_STDIN;
		}

		for (; (Error == JSONCFG_ERROR_NONE) && (Argument < argc); Argument = Argument + 2)
		{
			Element = JsonElementFind(&Root, JsonPathAscii(argv[Argument]), true);
			if (Element == NULL)
			{
				Error = JSONCFG_ERROR_NO_PATH;
			}
			else if (!JsonReadStringAscii(&Value, false, argv[Argument + 1]))
			{
				Error = JSONCFG_ERROR_READ_VALUE;
			}
			else
			{
				ChildValue = JsonElementGetChild(&Value, true);
				if ((JsonElementGetType(Element) == json_TypeObject) && (JsonElementGetType(ChildValue) == json_TypeObject))
				{
					Error = (JsonElementMoveChild(Element, ChildValue) != NULL) ? JSONCFG_ERROR_NONE : JSONCFG_ERROR_SET_VALUE;
				}
				else
				{
					Error = (JsonElementMoveChild(Element, &Value) != NULL) ? JSONCFG_ERROR_NONE : JSONCFG_ERROR_SET_VALUE;
				}
			}
		}

		if ((Error == JSONCFG_ERROR_NONE) && !JsonWriteFile(&Root, UtfType, RequireBOM, IndentSize, CommentType, stdout, JSONCFG_BUFFER_SIZE))
		{
			Error = JSONCFG_ERROR_WRITE_STDOUT;
		}
	}

	JsonElementCleanUp(&Value);
	JsonElementCleanUp(&Root);

	return Error;
}
