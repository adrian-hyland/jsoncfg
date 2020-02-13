#include "json.h"


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
        else if (JsonElementMoveChild(Element, &Value) == NULL)
        {
            Error = JSONCFG_ERROR_SET_VALUE;
        }
        else if (!JsonWriteFile(&Root, stdout))
        {
            Error = JSONCFG_ERROR_WRITE_STDOUT;
        }
        else
        {
            Error = JSONCFG_ERROR_NONE;
        }
    }

    JsonElementCleanUp(&Value);
    JsonElementCleanUp(&Root);

    return Error;
}
