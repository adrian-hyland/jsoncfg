#ifndef JSON_FORMAT_H
#define JSON_FORMAT_H

#include "json_element.h"


/**
 * @brief Enumeration used to define different formatting states
 */
typedef enum
{
    json_FormatComplete,          /**< Formatting is complete */
    json_FormatError,             /**< A parsing error has occurred */
    json_FormatKey,               /**< Formatting a character in a key string */
    json_FormatKeyEscape,         /**< Formatting an escaped character in a key string */
    json_FormatKeyEnd,            /**< Formatting the end of a key string */
    json_FormatValueStart,        /**< Formatting the start of a value */
    json_FormatValueString,       /**< Formatting a character in a string value */
    json_FormatValueStringEscape, /**< Formatting an escaped character in a string value */
    json_FormatValueLiteral,      /**< Formatting a character in a literal value */
    json_FormatValueNext,         /**< Formatting the next element */
    json_FormatValueEnd,          /**< Formatting the end of a value */
    json_FormatCommentLineStart,  /**< Formatting the start of a comment line */
    json_FormatCommentLine,       /**< Formatting a character in a comment line */
} tJsonFormatState;


/**
 * @brief Enumeration used to define different JSON formatting types
 */
typedef enum
{
    json_FormatCompress, /**< Compressed format - no spaces or new lines are used */
    json_FormatSpace,    /**< Spaced format - elements are separated with a space; no new lines are used */
    json_FormatIndent    /**< Indent format - object elements are indented on new lines */
} tJsonFormatType;


/**
 * @brief Type used for formatting JSON content
 */
typedef struct
{
    tJsonFormatType  Type;          /**< The type of formatting to use */
    tJsonFormatState State;         /**< The current format state */
    tJsonElement    *Element;       /**< The current element being formatted */
    size_t           NameIndex;     /**< Index into the element name */
    size_t           Indent;        /**< The current level of indentation */
    size_t           IndentSize;    /**< The number of spaces to use for an indentation */
    size_t           SpaceCount;    /**< The current number of spaces required in the format */
    int              NewLine;       /**< Indicates if a new line is required in the format */
    int              StripComments; /**< Indicates if comments should be stripped from the content */
} tJsonFormat;


/**
 * @name JsonFormat return codes
 * @{
 */
#define JSON_FORMAT_ERROR      -1 /**< A formatting error occured */
#define JSON_FORMAT_COMPLETE   0  /**< Formatting is complete */
#define JSON_FORMAT_INCOMPLETE 1  /**< Formatting is incomplete */
/** @} */


/**
 * @brief Sets up a JSON content formatter (using a compressed format)
 * @param Format      The JSON content formatter to set up
 * @param RootElement The elements to use for formatting the JSON content
 * @return None
 */
void JsonFormatSetUpCompress(tJsonFormat *Format, tJsonElement *RootElement);


/**
 * @brief Sets up a JSON content formatter (using a spaced format)
 * @param Format      The JSON content formatter to set up
 * @param RootElement The elements to use for formatting the JSON content
 * @return None
 */
void JsonFormatSetUpSpace(tJsonFormat *Format, tJsonElement *RootElement);


/**
 * @brief Sets up a JSON content formatter (using a indented format)
 * @param Format        The JSON content formatter to set up
 * @param IndentSize    The number of spaces to use for each indentation
 * @param StripComments Indicates whether comments should be stripped from the JSON content
 * @param RootElement   The elements to use for formatting the JSON content
 * @return None
 */
void JsonFormatSetUpIndent(tJsonFormat *Format, size_t IndentSize, int StripComments, tJsonElement *RootElement);


/**
 * @brief Cleans up a JSON content formatter
 * @param Format The JSON content formatter to clean up
 * @return None
 */
void JsonFormatCleanUp(tJsonFormat *Format);


/**
 * @brief Formats the next character in the JSON content
 * @param Format    The JSON content formatter
 * @param Character Used to return the next character in the JSON content
 * @return \a `JSON_PARSE_ERROR`      is returned if there was a formatting error
 * @return \a `JSON_PARSE_COMPLETE`   is returned if the formatting is complete
 * @return \a `JSON_PARSE_INCOMPLETE` is returned if formatting is not yet complete
 * @note if the function returns \a `JSON_PARSE_COMPLETE` then a null character will be returned in \a `Character`
 */
int JsonFormat(tJsonFormat *Format, uint8_t *Character);


#endif
