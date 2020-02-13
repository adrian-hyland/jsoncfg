#ifndef JSON_H
#define JSON_H

#include <stdio.h>
#include "json_parse.h"
#include "json_format.h"


int JsonReadString(tJsonElement *Root, const char *String);


int JsonReadFile(tJsonElement *Root, FILE *Stream);


int JsonWriteFile(tJsonElement *Root, FILE *Stream);


#endif
