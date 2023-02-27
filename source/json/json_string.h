#ifndef JSON_STRING_H
#define JSON_STRING_H

#include <stddef.h>
#include "json_utf8.h"


/**
 * @brief Type used to hold a string value
 */
typedef struct
{
	tJsonUtf8Unit *Content;   /**< The content of the string (null terminated) */
	size_t         Length;    /**< The length of the string */
} tJsonString;


/**
 * @brief Sets up a string
 * @param String The string to set up
 * @return None
 * @note Use \a `JsonStringCleanUp()` to clean up the string when it is no longer required
 */
void JsonStringSetUp(tJsonString *String);


/**
 * @brief Cleans up a string
 * @param String The string to clean up
 * @return None
 */
void JsonStringCleanUp(tJsonString *String);


/**
 * @brief Clears the content of a string
 * @param String The string
 * @return None
 */
void JsonStringClear(tJsonString *String);


/**
 * @brief Gets the length of a string
 * @param String The string
 * @return The length of the string
 */
size_t JsonStringGetLength(const tJsonString *String);


/**
 * @brief Adds a UTF-8 character code to the end of a string
 * @param String The string
 * @param Code   The UTF-8 character code to add to the string
 * @return A true value is returned if the character code was added to the string ok.
 * @return A false value is returned if the character code could not be added to the string (out of memory or invalid code).
 */
bool JsonStringAddUtf8Code(tJsonString *String, tJsonUtf8Code Code);


/**
 * @brief Adds a character to the end of a string
 * @param String    The string
 * @param Character The unicode character to add to the string
 * @return A true value is returned if the character was added to the string ok.
 * @return A false value is returned if the character could not be added to the string (out of memory or invalid character).
 */
bool JsonStringAddCharacter(tJsonString *String, tJsonCharacter Character);


/**
 * @brief Gets the next UTF-8 character code from a string
 * @param String The string
 * @param Offset The offset to the start of the next character code to get
 * @param Code   Used to return the next UTF-8 character code
 * @return The length of the UTF-8 character code that was returned.
 * @return A zero value is returned if the index is out of bounds (and a null character code is returned in \a `Code`).
 * @note Add the returned length to the \a `Index` value to advance to the next character code in the string.
 */
size_t JsonStringGetNextUtf8Code(const tJsonString *String, size_t Index, tJsonUtf8Code *Code);


/**
 * @brief Gets the next character from a string
 * @param String    The string
 * @param Offset    The offset to the start of the next character to get
 * @param Character Used to return the next character value
 * @return The length of the character (UTF-8 encoded) that was returned.
 * @return A zero value is returned if the index is out of bounds (and a null character is returned in \a `Character`).
 * @note Add the returned length to the \a `Index` value to advance to the next character in the string.
 */
size_t JsonStringGetNextCharacter(const tJsonString *String, size_t Offset, tJsonCharacter *Character);


#endif
