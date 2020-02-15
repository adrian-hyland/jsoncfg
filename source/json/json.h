#ifndef JSON_H
#define JSON_H

#include <stdio.h>
#include "json_parse.h"
#include "json_format.h"


/**
 * @brief Reads JSON content from a string
 * @param Root   The root JSON element
 * @param String The null terminated string containing the JSON content
 * @return A non-zero (true) value is returned if the JSON content was successfully read from the string
 * @return A zero (false) value is returned if the JSON content could not be read from the string
 */
int JsonReadString(tJsonElement *Root, const char *String);


/**
 * @brief Reads JSON content from a file
 * @param Root   The root JSON element
 * @param Stream The file to read the JSON content from
 * @return A non-zero (true) value is returned if the JSON content was successfully read from the file
 * @return A zero (false) value is returned if the JSON content could not be read from the file
 */
int JsonReadFile(tJsonElement *Root, FILE *Stream);


/**
 * @brief Writes JSON content to a file
 * @param Root   The root JSON element
 * @param Stream The file to wite the JSON content to
 * @return A non-zero (true) value is returned if the JSON content was successfully written to the file
 * @return A zero (false) value is returned if the JSON content could not be written to the file
 */
int JsonWriteFile(tJsonElement *Root, FILE *Stream);


#endif
