#ifndef JSON_H
#define JSON_H

#include <stdio.h>
#include "json_parse.h"
#include "json_format.h"


/**
 * @brief Reads JSON content from a string
 * @param Root          The root JSON element
 * @param StripComments Indicates whether comments should be stripped from the JSON content
 * @param String        The null terminated ASCII string containing the JSON content
 * @return A true value is returned if the JSON content was successfully read from the string.
 * @return A false value is returned if the JSON content could not be read from the string.
 */
bool JsonReadStringAscii(tJsonElement *Root, bool StripComments, const char *String);


/**
 * @brief Reads JSON content from a string
 * @param Root          The root JSON element
 * @param StripComments Indicates whether comments should be stripped from the JSON content
 * @param String        The null terminated UTF-8 encoded string containing the JSON content
 * @return A true value is returned if the JSON content was successfully read from the string.
 * @return A false value is returned if the JSON content could not be read from the string.
 */
bool JsonReadStringUtf8(tJsonElement *Root, bool StripComments, const uint8_t *String);


/**
 * @brief Reads JSON content from a file
 * @param Root          The root JSON element
 * @param StripComments Indicates whether comments should be stripped from the JSON content
 * @param Stream        The file to read the JSON content from
 * @param BufferSize    The size of the buffer that is used when reading content from the file
 * @return A true value is returned if the JSON content was successfully read from the file.
 * @return A false value is returned if the JSON content could not be read from the file.
 * @note The buffer is allocated on the stack and should not be too large.
 */
bool JsonReadFile(tJsonElement *Root, bool StripComments, FILE *Stream, size_t BufferSize);


/**
 * @brief Writes JSON content to a file
 * @param Root        The root JSON element
 * @param UtfType     The type of encoding to use
 * @param RequireBOM  Indicates if a byte order mark should be encoded at the start of the content
 * @param IndentSize  The number of spaces to use for each indentation
 * @param CommentType Indicates how to format any comments
 * @param Stream      The file to wite the JSON content to
 * @param BufferSize  The size of the buffer that is used when writing content to the file
 * @return A true value is returned if the JSON content was successfully written to the file.
 * @return A false value is returned if the JSON content could not be written to the file.
 * @note If the parameter \a `IndentSize` is zero then the content will use a 'spaced' format. Any comments will also be stripped (the value of the parameter \a `CommentType` will be ignored)
 * @note The buffer is allocated on the stack and should not be too large.
 */
bool JsonWriteFile(tJsonElement *Root, tJsonUtfType UtfType, bool RequireBOM, size_t IndentSize, tJsonCommentType CommentType, FILE *Stream, size_t BufferSize);


#endif
