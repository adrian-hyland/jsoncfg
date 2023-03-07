#ifndef TEST_JSON_H
#define TEST_JSON_H

#include "test.h"


/**
 * @brief Contains test cases for the `JsonCharacter` set of functions
 */
extern const tTest TestJsonCharacter;


/**
 * @brief Contains test cases for the `JsonUtf8` set of functions
 */
extern const tTest TestJsonUtf8;


/**
 * @brief Contains test cases for the `JsonUtf16` set of functions
 */
extern const tTest TestJsonUtf16;


/**
 * @brief Contains test cases for the `JsonString` set of functions
 */
extern const tTest TestJsonString;


/**
 * @brief Contains test cases for the `JsonPath` set of functions
 */
extern const tTest TestJsonPath;


/**
 * @brief Contains test cases for the `JsonElement` set of functions
 */
extern const tTest TestJsonElement;


/**
 * @brief Contains test cases for the `JsonParse` set of functions
 */
extern const tTest TestJsonParse;


/**
 * @brief Contains test cases for the `JsonFormat` set of functions
 */
extern const tTest TestJsonFormat;


/**
 * @brief Contains test cases for UTF-8 encoded key string values
 */
extern const tTest TestJsonKeyUtf8;


/**
 * @brief Contains test cases for UTF-8 encoded string values
 */
extern const tTest TestJsonValueUtf8;


/**
 * @brief Contains test cases for UTF-8 encoded literal values
 */
extern const tTest TestJsonLiteralUtf8;


/**
 * @brief Contains test cases for UTF-8 encoded line comments
 */
extern const tTest TestJsonCommentLineUtf8;


/**
 * @brief Contains test cases for UTF-8 encoded block comments
 */
extern const tTest TestJsonCommentBlockUtf8;


/**
 * @brief Contains test cases for UTF-16 escaped key string values
 */
extern const tTest TestJsonKeyUtf16;


/**
 * @brief Contains test cases for UTF-16 escaped string values
 */
extern const tTest TestJsonValueUtf16;


#endif
