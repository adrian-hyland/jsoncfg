#ifndef JSON_UTF8_H
#define JSON_UTF8_H

#include "json_character.h"


/**
 * @brief The maximum number of code units that an encoded UTF-8 character can have
 */
#define JSON_UTF8_MAX_SIZE (sizeof(tJsonUtf8Code) / sizeof(tJsonUtf8Unit))


/**
 * @brief The UTF-8 code value of the unicode replacement character
 */
#define JSON_UTF8_CODE_REPLACEMENT 0xEFBFBD


/**
 * @name JsonUtf8CodeAddUnit return codes
 * @{
 */
#define JSON_UTF8_INVALID    -1 /**< UTF-8 code is invalid */
#define JSON_UTF8_VALID      0  /**< UTF-8 code is valid */
#define JSON_UTF8_INCOMPLETE 1  /**< UTF-8 code is incomplete */
/** @} */


/**
 * @brief Type used to hold a UTF-8 character code
 * @note The value that this holds is the UTF-8 code of a unicode character (padded with leading zeros).
 * @note Use \a `JsonUtf8CodeGetCharacter()` to get the actual character code point value.
 */
typedef uint32_t tJsonUtf8Code;


/**
 * @brief Type used to hold a UTF-8 code unit
 */
typedef uint8_t tJsonUtf8Unit;


/**
 * @brief Converts a unicode character to a UTF-8 character code 
 * @param Character The unicode character 
 * @return The UTF-8 character code
 * @note The value \a `JSON_UTF8_CODE_REPLACEMENT` is returned if the unicode character is not valid
 */
tJsonUtf8Code JsonUtf8Code(tJsonCharacter Character);


/**
 * @brief Converts a UTF-8 character code to a unicode character
 * @param Code The UTF-8 character code
 * @return The unicode character
 * @note The value \a `JSON_CHARACTER_REPLACEMENT` is returned if the UTF-8 character code is not valid
 */
tJsonCharacter JsonUtf8CodeGetCharacter(tJsonUtf8Code Code);


/**
 * @brief Checks if a UTF-8 character code has a valid value
 * @param Code The character code to check
 * @return A true value is returned if the character code is valid
 * @return A false value is returned if the character code is not valid
 */
bool JsonUtf8CodeIsValid(tJsonUtf8Code Code);


/**
 * @brief Returns the number of code units in a UTF-8 character code
 * @param Code The UTF-8 character code
 * @return The number of code units
 * @return A zero value is returned if the code is not valid
 */
size_t JsonUtf8CodeGetUnitLength(tJsonUtf8Code Code);


/**
 * @brief Gets a code unit from a UTF-8 character code
 * @param Code  The UTF-8 character code
 * @param Index The code unit to get
 * @return The code unit
 * @return A zero value is returned if the code unit could not be returned
 */
tJsonUtf8Unit JsonUtf8CodeGetUnit(tJsonUtf8Code Code, size_t Index);


/**
 * @brief Adds a code unit to a UTF-8 character code
 * @param Code The UTF-8 character code
 * @param Unit The code unit to add to the character code
 * @return `JSON_UTF8_INVALID`    is returned if the code unit could not be added to the character code.
 * @return `JSON_UTF8_VALID`      is returned if the code unit was added to the character code (and the character code is valid - no more code units need to be added).
 * @return `JSON_UTF8_INCOMPLETE` is returned if the code unit was added to the character code (but the character code is incomplete - one or more code units still need to be added).
 */
int JsonUtf8CodeAddUnit(tJsonUtf8Code *Code, tJsonUtf8Unit Unit);


/**
 * @brief Gets the next character code in a UTF-8 encoded content
 * @param Content The UTF-8 encoded content
 * @param Length  The length of the UTF-8 encoded content
 * @param Offset  The offset to the start of the next character code to get
 * @param Code    Used to return the next character code
 * @return The length of the UTF-8 character code that was returned.
 * @return A zero value is returned if the next character code could not be returned.
 * @note Add the returned length to the \a `Offset` value to advance it to the next character code in the content.
 */
size_t JsonUtf8GetNextCode(const tJsonUtf8Unit *Content, size_t Length, size_t Offset, tJsonUtf8Code *Code);


/**
 * @brief Gets the previous character code in a UTF-8 encoded content
 * @param Content The UTF-8 encoded content
 * @param Length  The length of the UTF-8 encoded content
 * @param Offset  The offset to the end of the previous character code to get
 * @param Code    Used to return the previous character code
 * @return The length of the UTF-8 character code that was returned.
 * @return A zero value is returned if the previous character code could not be returned.
 * @note Pass \a 'Length' in the \a 'Offset' parameter to get the last character in the content.
 * @note Subtract the returned length from the \a `Offset` value to advance it to the previous character in the content.
 */
size_t JsonUtf8GetPreviousCode(const tJsonUtf8Unit *Content, size_t Length, size_t Offset, tJsonUtf8Code *Code);


#endif
