#ifndef JSON_FORMAT_H
#define JSON_FORMAT_H

#include "json_element.h"
#include "json_utf16.h"
#include "json_utf.h"


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
	json_FormatUtf16Digit,      /**< Formatting the digit of an escaped unicode character in a key string */
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
	tJsonFormatType  Type;        /**< The type of formatting to use */
	tJsonFormatState State;       /**< The current format state */
	tJsonElement    *Element;     /**< The current element being formatted */
	tJsonCommentType CommentType; /**< Indicates how to format comments */
	tJsonFormatState EscapeState; /**< The state that the formatter is in when a character needs to be escaped (so that it can be restored afterwards) */
	tJsonUtf16       Utf16;       /**< The UTF-16 encoding for a character being escaped */
	size_t           Utf16Length; /**< The length of the UTF-16 encoding */
	size_t           Utf16Index;  /**< The current index into the UTF-16 encoding */
	size_t           NameIndex;   /**< The current index into the element name */
	size_t           Indent;      /**< The current level of indentation */
	size_t           IndentSize;  /**< The number of spaces to use for an indentation */
	size_t           SpaceCount;  /**< The current number of spaces required in the format */
	bool             NewLine;     /**< Indicates if a new line is required in the format */
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
 * @brief Sets up a JSON content formatter (using an indented format)
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
 * @param Format    The JSON content formatter
 * @param Character Used to return the next character in the JSON content
 * @return \a `JSON_FORMAT_ERROR`      is returned if there was a formatting error
 * @return \a `JSON_FORMAT_COMPLETE`   is returned if the formatting is complete
 * @return \a `JSON_FORMAT_INCOMPLETE` is returned if formatting is not yet complete
 * @note If the function returns \a `JSON_FORMAT_COMPLETE` then a null character will be returned in \a `Character`
 * @note The character is a unicode code point value and should be encoded using a suitable UTF encoding
 */
int JsonFormatCharacter(tJsonFormat *Format, tJsonCharacter *Character);


/**
 * @brief The minimum content size (in bytes) that is required for formatting
 */
#define JSON_FORMAT_MIN_SIZE 4


/**
 * @brief Formats JSON content using a UTF encoding
 * @param Format   The JSON content formatter
 * @param UtfType  The type of UTF to use to encode the content
 * @param Content  Used to return the encoded JSON content
 * @param Size     The size of the encoded JSON content
 * @param Offset   Used to pass and return the offset where the JSON content is formatted
 * @return \a `JSON_FORMAT_ERROR`      is returned if there was a formatting error
 * @return \a `JSON_FORMAT_COMPLETE`   is returned if the formatting is complete
 * @return \a `JSON_FORMAT_INCOMPLETE` is returned if formatting is not yet complete
 * @note The \a `Content` size should be at least \a `JSON_FORMAT_MIN_SIZE` in bytes
 */
int JsonFormat(tJsonFormat *Format, tJsonUtfType UtfType, uint8_t *Content, size_t Size, size_t *Offset);


#endif
