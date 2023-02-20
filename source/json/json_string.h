#ifndef JSON_STRING_H
#define JSON_STRING_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>


/**
 * @brief Type used to hold a string value
 */
typedef struct
{
	uint8_t *Content;   /**< The content of the string (null terminated) */
	size_t   Length;    /**< The length of the string */
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
size_t JsonStringGetLength(tJsonString *String);


/**
 * @brief Adds a character to the end of a string
 * @param String    The string
 * @param Character The character to add to the string
 * @return A true value is returned if the character was added to the string ok
 * @return A false value is returned if the character could not be added to the string (out of memory)
 */
bool JsonStringAddCharacter(tJsonString *String, uint8_t Character);


/**
 * @brief Gets a character from a string
 * @param String The string
 * @param Index  The index of the character to retrieve
 * @return The character
 * @return A null character is returned if the index is out of bounds
 */
uint8_t JsonStringGetCharacter(tJsonString *String, size_t Index);


#endif
