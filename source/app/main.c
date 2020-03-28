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
    size_t IndentSize = 3;
    int StripComments = 0;
    int Error;

    JsonElementSetUp(&Root);
    JsonElementSetUp(&Value);

    if (argc < 3)
    {
        Error = JSONCFG_ERROR_BAD_ARGS;
    }
    else if (!JsonReadString(&Value, 1, argv[2]))
    {
        Error = JSONCFG_ERROR_READ_VALUE;
    }
    else if (!JsonReadFile(&Root, StripComments, stdin))
    {
        Error = JSONCFG_ERROR_READ_STDIN;
    }
    else
    {
        Element = JsonElementFind(&Root, (const uint8_t *)argv[1], 1);
        if (Element == NULL)
        {
            Error = JSONCFG_ERROR_NO_PATH;
        }
        else
        {
            ChildValue = JsonElementGetChild(&Value, 1);
            if ((JsonElementGetType(Element) == json_TypeObject) && (JsonElementGetType(ChildValue) == json_TypeObject))
            {
                Error = (JsonElementMoveChild(Element, ChildValue) != NULL) ? JSONCFG_ERROR_NONE : JSONCFG_ERROR_SET_VALUE;
            }
            else
            {
                Error = (JsonElementMoveChild(Element, &Value) != NULL) ? JSONCFG_ERROR_NONE : JSONCFG_ERROR_SET_VALUE;
            }

            if ((Error == JSONCFG_ERROR_NONE) && !JsonWriteFile(&Root, IndentSize, StripComments, stdout))
            {
                Error = JSONCFG_ERROR_WRITE_STDOUT;
            }
        }
    }

    JsonElementCleanUp(&Value);
    JsonElementCleanUp(&Root);

    return Error;
}
