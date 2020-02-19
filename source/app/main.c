#include "json.h"


#ifdef DEBUG
// This string declaration serves only to illustrate using jsoncfg to set up different build configuration for VS Code.
// Once you have built a release version of jsoncfg (using 'make all'), use 'make vscode DEBUG=1' to add a 'Debug' build configuration
// to the VS Code project.
// If you select the 'Debug' configuration (at the right of the bar at the bottom of the window) then this part of the code will appear
// enabled and 'APP_NAME' will correctly resolve to 'jsoncfg' when you hover your mouse pointer over it.
// You can also use 'make vscode' to add a 'Release' build configurations to the VS Code project. Selecting the 'Release' configuration
// will cause this code to appear disabled.
const char AppName[] = APP_NAME;
#endif


#define JSONCFG_ERROR_NONE          0
#define JSONCFG_ERROR_BAD_ARGS     -1
#define JSONCFG_ERROR_READ_VALUE   -2
#define JSONCFG_ERROR_READ_STDIN   -3
#define JSONCFG_ERROR_NO_PATH      -4
#define JSONCFG_ERROR_SET_VALUE    -5
#define JSONCFG_ERROR_WRITE_STDOUT -6


static void FileCopy(FILE *To, FILE *From)
{
    unsigned char Content[256];
    size_t Length;

    fseek(From, 0, SEEK_SET);
    for (;;)
    {
        Length = fread(Content, 1, sizeof(Content), From);
        if (Length == 0)
        {
            break;
        }
        fwrite(Content, 1, Length, To);
    }
}


int main(int argc, const char *argv[])
{
    tJsonElement Root;
    tJsonElement Value;
    tJsonElement *Element;
    tJsonElement *ChildValue;
    int Error;

    JsonElementSetUp(&Root);
    JsonElementSetUp(&Value);

    if (argc < 3)
    {
        Error = JSONCFG_ERROR_BAD_ARGS;
    }
    else if (!JsonReadString(&Value, argv[2]))
    {
        Error = JSONCFG_ERROR_READ_VALUE;
    }
    else if (!JsonReadFile(&Root, stdin))
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
            ChildValue = JsonElementGetChild(&Value);
            if ((JsonElementGetType(Element) == json_TypeObject) && (JsonElementGetType(ChildValue) == json_TypeObject))
            {
                Error = (JsonElementMoveChild(Element, ChildValue) != NULL) ? JSONCFG_ERROR_NONE : JSONCFG_ERROR_SET_VALUE;
            }
            else
            {
                Error = (JsonElementMoveChild(Element, &Value) != NULL) ? JSONCFG_ERROR_NONE : JSONCFG_ERROR_SET_VALUE;
            }

            if ((Error == JSONCFG_ERROR_NONE) && !JsonWriteFile(&Root, stdout))
            {
                Error = JSONCFG_ERROR_WRITE_STDOUT;
            }
        }
    }

    if (Error != JSONCFG_ERROR_NONE)
    {
        FileCopy(stdout, stdin);
    }

    JsonElementCleanUp(&Value);
    JsonElementCleanUp(&Root);

    return Error;
}
