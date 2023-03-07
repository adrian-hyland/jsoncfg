#ifndef JSON_PARSE_H
#define JSON_PARSE_H

#include "json_element.h"
#include "json_utf16.h"


/**
 * @brief Enumeration used to define different parsing states
 */
typedef enum
{
	json_ParseComplete,         /**< Parsing is complete */
	json_ParseError,            /**< A parsing error has occurred */
	json_ParseEscape,           /**< Parsing an escaped character */
	json_ParseUtf16Escape,      /**< Parsing an escaped unicode character */
	json_ParseUtf16,            /**< Parsing the start of an escaped unicode character */
	json_ParseUtf16Digit1,      /**< Parsing the first digit of an escaped unicode character */
	json_ParseUtf16Digit2,      /**< Parsing the second digit of an escaped unicode character */
	json_ParseUtf16Digit3,      /**< Parsing the third digit of an escaped unicode character */
	json_ParseUtf16Digit4,      /**< Parsing the fourth digit of an escaped unicode character */
	json_ParseKeyStart,         /**< Parsing the start of a key string */
	json_ParseKey,              /**< Parsing a character in a key string */
	json_ParseKeyEnd,           /**< Parsing the end of a key string */
	json_ParseValueStart,       /**< Parsing the start of a value */
	json_ParseValueString,      /**< Parsing a character in a string value */
	json_ParseValueLiteral,     /**< Parsing a character in a literal value */
	json_ParseValueEnd,         /**< Parsing the end of a value */
	json_ParseCommentStart,     /**< Parsing the start of a comment */
	json_ParseCommentLine,      /**< Parsing a character in a line comment */
	json_ParseCommentBlock,     /**< Parsing a character in a block comment */
	json_ParseCommentBlockLine, /**< Parsing a new line in a block comment */
	json_ParseCommentBlockEnd,  /**< Parsing the end of a block comment */
} tJsonParseState;


/**
 * @brief Type used for parsing JSON content
 */
typedef struct
{
	tJsonParseState State;         /**< The current parse state */
	tJsonElement   *Element;       /**< The current element being parsed */
	tJsonUtf8Code   Utf8Code;      /**< The current UTF-8 character code being parsed */
	tJsonUtf16Code  Utf16Code;     /**< The current UTF-16 character being escaped */
	tJsonParseState CommentState;  /**< The state that the parser is in when a comment is encountered (so that it can be restored afterwards) */
	tJsonParseState EscapeState;   /**< The state that the parser is in when a character needs to be escaped (so that it can be restored afterwards) */
	bool            AllocateChild; /**< Indicates whether an allocated element should be a child element (or the next sibling element) */
	bool            StripComments; /**< Indicates whether to strip or keep any comments that are in the content */
} tJsonParse;


/**
 * @name JsonParse return codes
 * @{
 */
#define JSON_PARSE_ERROR      -1 /**< A parsing error occured */
#define JSON_PARSE_COMPLETE   0  /**< Parsing is complete */
#define JSON_PARSE_INCOMPLETE 1  /**< Parsing is incomplete */
/** @} */


/**
 * @brief Sets up a JSON content parser
 * @param Parse         The JSON content parser to set up
 * @param StripComments Indicates whether comments should be stripped from the JSON content
 * @param RootElement   Used to return the elements parsed from the JSON content
 * @return None
 */
void JsonParseSetUp(tJsonParse *Parse, bool StripComments, tJsonElement *RootElement);


/**
 * @brief Cleans up a JSON content parser
 * @param Parse The JSON content parser to clean up
 * @return None
 */
void JsonParseCleanUp(tJsonParse *Parse);


/**
 * @brief Parses the next UTF-8 code unit in the JSON content
 * @param Parse    The JSON content parser
 * @param CodeUnit The next UTF-8 code unit in the JSON content
 * @return `JSON_PARSE_ERROR`      is returned if there was a parsing error
 * @return `JSON_PARSE_COMPLETE`   is returned if the parsing is complete
 * @return `JSON_PARSE_INCOMPLETE` is returned if parsing is not yet complete
 * @note The parsing should be completed by passing a null character (zero value code unit)
 */
int JsonParse(tJsonParse *Parse, tJsonUtf8Unit CodeUnit);


#endif
