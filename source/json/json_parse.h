#ifndef JSON_PARSE_H
#define JSON_PARSE_H

#include "json_element.h"


/**
 * @brief Enumeration used to define different parsing states
 */
typedef enum
{
    json_ParseComplete,          /**< Parsing is complete */
    json_ParseError,             /**< A parsing error has occurred */
    json_ParseKeyStart,          /**< Parsing the start of a key string */
    json_ParseKey,               /**< Parsing a character in a key string */
    json_ParseKeyEscape,         /**< Parsing an escaped character in a key string */
    json_ParseKeyEnd,            /**< Parsing the end of a key string */
    json_ParseValueStart,        /**< Parsing the start of a value */
    json_ParseValueString,       /**< Parsing a character in a string value */
    json_ParseValueStringEscape, /**< Parsing an escaped character in a string value */
    json_ParseValueLiteral,      /**< Parsing a character in a literal value */
    json_ParseValueEnd,          /**< Parsing the end of a value */
    json_ParseCommentStart,      /**< Parsing the start of a comment */
    json_ParseCommentLine,       /**< Parsing a character in a line comment */
    json_ParseCommentBlock,      /**< Parsing a character in a block comment */
    json_ParseCommentBlockLine,  /**< Parsing a new line in a block comment */
    json_ParseCommentBlockEnd,   /**< Parsing the end of a block comment */
} tJsonParseState;


/**
 * @brief Type used for parsing JSON content
 */
typedef struct
{
    tJsonParseState State;         /**< The current parse state */
    tJsonElement   *Element;       /**< The current element being parsed */
    tJsonParseState CommentState;  /**< The state that the parser was in when a comment is encountered (so that it can be restored afterwards) */
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
 * @brief Parses the next character in the JSON content
 * @param Parse     The JSON content parser
 * @param Character The next character in the JSON content
 * @return `JSON_PARSE_ERROR`      is returned if there was a parsing error
 * @return `JSON_PARSE_COMPLETE`   is returned if the parsing is complete
 * @return `JSON_PARSE_INCOMPLETE` is returned if parsing is not yet complete
 * @note The parsing should be completed by passing a null character
 */
int JsonParse(tJsonParse *Parse, uint8_t Character);


#endif
