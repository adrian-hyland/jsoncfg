#ifndef JSON_FORMAT_H
#define JSON_FORMAT_H

#include "json_element.h"
#include "json_utf16.h"


/**
 * @brief Enumeration used to define different formatting states
 */
typedef enum
{
	json_FormatComplete,        /**< Formatting is complete */
	json_FormatError,           /**< A parsing error has occurred */
	json_FormatEscape,          /**< Formatting an escaped character in a key string */
	json_FormatUtf16Escape,     /**< Formatting an escaped unicode character in a key string */
	json_FormatUtf16,           /**< Formatting the start of an escaped unicode character in a key string */
	json_FormatUtf16Digit1,     /**< Formatting the first digit of an escaped unicode character in a key string */
	json_FormatUtf16Digit2,     /**< Formatting the second digit of an escaped unicode character in a key string */
	json_FormatUtf16Digit3,     /**< Formatting the third digit of an escaped unicode character in a key string */
	json_FormatUtf16Digit4,     /**< Formatting the fourth digit of an escaped unicode character in a key string */
	json_FormatKey,             /**< Formatting a character in a key string */
	json_FormatKeyEnd,          /**< Formatting the end of a key string */
	json_FormatValueStart,      /**< Formatting the start of a value */
	json_FormatValueString,     /**< Formatting a character in a string value */
	json_FormatValueLiteral,    /**< Formatting a character in a literal value */
	json_FormatValueNext,       /**< Formatting the next element */
	json_FormatValueEnd,        /**< Formatting the end of a value */
	json_FormatCommentStart,    /**< Formatting the start of a comment */
	json_FormatComment,         /**< Formatting a character in a comment */
	json_FormatCommentBlock,    /**< Formatting a character in a comment block */
	json_FormatCommentBlockEnd, /**< Formatting the end of a comment block */
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
 * @brief Enumeration used to define different JSON comment types
 */
typedef enum
{
	json_CommentNone, /**< No comments */
	json_CommentLine, /**< Line comments */
	json_CommentBlock /**< Block comments */
} tJsonCommentType;


/**
 * @brief Type used for formatting JSON content
 */
typedef struct
{
	tJsonFormatType  Type;           /**< The type of formatting to use */
	tJsonFormatState State;          /**< The current format state */
	tJsonElement    *Element;        /**< The current element being formatted */
	tJsonUtf8Code    Utf8Code;       /**< The current UTF-8 character code being formatted */
	tJsonUtf16Code   Utf16Code;      /**< The current UTF-16 character being escaped */
	tJsonCommentType CommentType;    /**< Indicates how to format comments */
	tJsonFormatState EscapeState;    /**< The state that the formatter is in when a character needs to be escaped (so that it can be restored afterwards) */
	size_t           Utf8CodeIndex;  /**< Index into the current UTF-8 character code */
	size_t           Utf16CodeIndex; /**< Index into the current UTF-16 character being escaped */
	size_t           NameIndex;      /**< Index into the element name */
	size_t           Indent;         /**< The current level of indentation */
	size_t           IndentSize;     /**< The number of spaces to use for an indentation */
	size_t           SpaceCount;     /**< The current number of spaces required in the format */
	bool             NewLine;        /**< Indicates if a new line is required in the format */
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
 * @param Format      The JSON content formatter to set up
 * @param IndentSize  The number of spaces to use for each indentation
 * @param CommentType Indicates how to format any comments
 * @param RootElement The elements to use for formatting the JSON content
 * @return None
 */
void JsonFormatSetUpIndent(tJsonFormat *Format, size_t IndentSize, tJsonCommentType CommentType, tJsonElement *RootElement);


/**
 * @brief Cleans up a JSON content formatter
 * @param Format The JSON content formatter to clean up
 * @return None
 */
void JsonFormatCleanUp(tJsonFormat *Format);


/**
 * @brief Formats the next character in the JSON content
 * @param Format   The JSON content formatter
 * @param CodeUnit Used to return the next UTF-8 code unit in the JSON content
 * @return \a `JSON_FORMAT_ERROR`      is returned if there was a formatting error
 * @return \a `JSON_FORMAT_COMPLETE`   is returned if the formatting is complete
 * @return \a `JSON_FORMAT_INCOMPLETE` is returned if formatting is not yet complete
 * @note if the function returns \a `JSON_FORMAT_COMPLETE` then a null character will be returned in \a `Character`
 */
int JsonFormat(tJsonFormat *Format, tJsonUtf8Unit *CodeUnit);


#endif
