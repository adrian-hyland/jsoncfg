#ifndef JSON_UTF16_H
#define JSON_UTF16_H

#include "json_character.h"


/**
 * @brief The maximum number of code units that an encoded UTF-16 character can have
 */
#define JSON_UTF16_MAX_SIZE (sizeof(tJsonUtf16Code) / sizeof(tJsonUtf16Unit))


/**
 * @brief The UTF-16 code value of the unicode replacement character
 */
#define JSON_UTF16_CODE_REPLACEMENT 0xFFFD


/**
 * @name JsonUtf16CodeAddUnit and JsonUtf16CodeAddNibble return codes
 * @{
 */
#define JSON_UTF16_INVALID    -1 /**< UTF-16 code is invalid */
#define JSON_UTF16_VALID      0  /**< UTF-16 code is complete */
#define JSON_UTF16_INCOMPLETE 1  /**< UTF-16 code is incomplete */
/** @} */


/**
 * @brief Type used to hold a UTF-16 character code
 * @note The value that this holds is the UTF-16 code of a unicode character (padded with leading zeros).
 * @note Use \a `JsonUtf16CodeGetCharacter()` to get the actual character code point value.
 */
typedef uint32_t tJsonUtf16Code;


/**
 * @brief Type used to hold a UTF-16 code unit
 */
typedef uint16_t tJsonUtf16Unit;


/**
 * @brief Converts a unicode character to a UTF-16 character code 
 * @param Character The unicode character 
 * @return The UTF-16 character code
 * @note The value \a `JSON_UTF16_CODE_REPLACEMENT` is returned if the unicode character is not valid
 */
tJsonUtf16Code JsonUtf16Code(tJsonCharacter Character);


/**
 * @brief Converts a UTF-16 character code to a unicode character
 * @param Code The UTF-16 character code
 * @return The unicode character
 * @note The value \a `JSON_CHARACTER_REPLACEMENT` is returned if the UTF-16 character code is not valid
 */
tJsonCharacter JsonUtf16CodeGetCharacter(tJsonUtf16Code Code);


/**
 * @brief Checks if a UTF-16 character code has a valid value
 * @param Code The character code to check
 * @return A true value is returned if the character code is valid
 * @return A false value is returned if the character code is not valid
 */
bool JsonUtf16CodeIsValid(tJsonUtf16Code Code);


/**
 * @brief Returns the number of code units in a UTF-16 character code
 * @param Code The UTF-16 character code
 * @return The number of code units
 * @return A zero value is returned if the code is not valid
 */
size_t JsonUtf16CodeGetUnitLength(tJsonUtf16Code Code);


/**
 * @brief Gets a code unit from a UTF-16 character code
 * @param Code  The UTF-16 character code
 * @param Index The code unit to get
 * @return The code unit
 * @return A zero value is returned if the code unit could not be returned
 */
tJsonUtf16Unit JsonUtf16CodeGetUnit(tJsonUtf16Code Code, size_t Index);


/**
 * @brief Adds a code unit to a UTF-16 character code
 * @param Code The UTF-16 character code
 * @param Unit The code unit to add to the character code
 * @return `JSON_UTF16_INVALID`    is returned if the code unit could not be added to the character code.
 * @return `JSON_UTF16_VALID`      is returned if the code unit was added to the character code (and the character code is valid - no more code units need to be added).
 * @return `JSON_UTF16_INCOMPLETE` is returned if the code unit was added to the character code (but the character code is incomplete - one or more code units still need to be added).
 */
int JsonUtf16CodeAddUnit(tJsonUtf16Code *Code, tJsonUtf16Unit Unit);


/**
 * @brief Returns the number of nibbles in a UTF-16 character code
 * @param Code The UTF-16 character code
 * @return The number of nibbles
 * @return A zero value is returned if the code is not valid
 */
size_t JsonUtf16CodeGetNibbleLength(tJsonUtf16Code Code);


/**
 * @brief Gets a nibble from a UTF-16 character code
 * @param Code  The UTF-16 character code
 * @param Index The nibble to get
 * @return The nibble
 * @return A zero value is returned if the nibble could not be returned
 */
uint8_t JsonUtf16CodeGetNibble(tJsonUtf16Code Code, size_t Index);


/**
 * @brief Adds a nibble to a UTF-16 character code
 * @param Code   The UTF-16 character code
 * @param Nibble The nibble (value between 0x0 and 0xF) to add to the character code
 * @return `JSON_UTF16_INVALID`    is returned if the nibble could not be added to the character code.
 * @return `JSON_UTF16_VALID`      is returned if the nibble was added to the character code (and the character code is valid).
 * @return `JSON_UTF16_INCOMPLETE` is returned if the nibble was added to the character code (but the character code is incomplete - one or more nibbles still need to be added to make it valid).
 */
int JsonUtf16CodeAddNibble(tJsonUtf16Code *Code, uint8_t Nibble);


/**
 * @brief Checks if a UTF-16 code unit is a high surrogate value value
 * @param Unit The UTF-16 code unit to check
 * @return A true value is returned if the code unit is is a high surrogate value
 * @return A false value is returned if the code unit is not a high surrogate value
 * @note A high surrogate value does not represent a valid unicode character.
 * @note If the code unit is a high surrogate value then it should be combined with a low surrogate code unit.
 */
bool JsonUtf16UnitIsHighSurrogate(tJsonUtf16Unit Unit);


/**
 * @brief Checks if a UTF-16 code unit is a low surrogate value value
 * @param Unit The UTF-16 code unit to check
 * @return A true value is returned if the character unit is is a low surrogate value
 * @return A false value is returned if the character unit is not a low surrogate value
 * @note A low surrogate value does not represent a valid unicode character.
 */
bool JsonUtf16UnitIsLowSurrogate(tJsonUtf16Unit Unit);


/**
 * @brief Sets a nibble in a UTF-16 code unit
 * @param Code  The UTF-16 code unit
 * @param Index The nibble to set
 * @param Nibble The nibble value (between 0x0 and 0xF)
 * @return A true value is returned if the nibble was set in the code unit
 * @return A false value is returned if the nibble could not be set in the code unit
 */
bool JsonUtf16UnitSetNibble(tJsonUtf16Unit *Code, size_t Index, uint8_t Nibble);


#endif
