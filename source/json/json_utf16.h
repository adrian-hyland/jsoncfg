#ifndef JSON_UTF16_H
#define JSON_UTF16_H

#include "json_character.h"


/**
 * @brief The maximum number of bytes that an encoded UTF-16 character can have
 */
#define JSON_UTF16_MAX_SIZE 4


/**
 * @brief The size of a UTF-16 code unit
 */
#define JSON_UTF16_UNIT_SIZE 2


/**
 * @brief Type that can hold a UTF-16 encoded character
 */
typedef uint8_t tJsonUtf16[JSON_UTF16_MAX_SIZE];


/**
 * @brief Type that can hold a UTF-16 code unit
 */
typedef uint8_t tJsonUtf16Unit[JSON_UTF16_UNIT_SIZE];


/**
 * @brief Checks if a UTF-16 code unit is a high surrogate value value
 * @param Unit The UTF-16 code unit to check
 * @return A true value is returned if the code unit is is a high surrogate value
 * @return A false value is returned if the code unit is not a high surrogate value
 * @note A high surrogate value does not represent a valid unicode character.
 * @note If the code unit is a high surrogate value then it should be combined with a low surrogate code unit.
 */
bool JsonUtf16beIsHighSurrogate(const tJsonUtf16Unit Unit);


/**
 * @brief Checks if a UTF-16 code unit is a low surrogate value value
 * @param Unit The UTF-16 code unit to check
 * @return A true value is returned if the character unit is is a low surrogate value
 * @return A false value is returned if the character unit is not a low surrogate value
 * @note A low surrogate value does not represent a valid unicode character.
 */
bool JsonUtf16beIsLowSurrogate(const tJsonUtf16Unit Unit);


/**
 * @brief Encodes a character in UTF-16 (big endian)
 * @param Content   The UTF-16 (big endian) encoded content
 * @param Size      The maximum size of UTF-16 (big endian) encoded content
 * @param Offset    The offset in the UTF-16 (big endian) encoded content where the character should be encoded
 * @param Character The character to encode
 * @return The length of the encoded character.
 * @return A zero value is returned if the character could not be encoded
 * @note This function can only be used to add a character to the end the UTF-16 (big endian) encoded content.
 * @note i.e. The current length of \a `Content` should be passed in the parameter \a `Offset`.
 */
size_t JsonUtf16beEncode(uint8_t *Content, size_t Size, size_t Offset, tJsonCharacter Character);


/**
 * @brief Decodes the next character in a UTF-16 (big endian) encoded content
 * @param Content   The UTF-16 (big endian) encoded content
 * @param Length    The length of the UTF-16 (big endian) encoded content
 * @param Offset    The offset to the start of the next character to decode
 * @param Character Used to return the next character
 * @return The decoded length of the next character.
 * @return A zero value is returned if the next character could not be decoded.
 * @note Add the returned length to the \a `Offset` value to advance it to the next character in the content.
 */
size_t JsonUtf16beDecodeNext(const uint8_t *Content, size_t Length, size_t Offset, tJsonCharacter *Character);


/**
 * @brief Encodes a character in UTF-16 (little endian)
 * @param Content   The UTF-16 (little endian) encoded content
 * @param Size      The maximum size of UTF-16 (little endian) encoded content
 * @param Offset    The offset in the UTF-16 (little endian) encoded content where the character should be encoded
 * @param Character The character to encode
 * @return The length of the encoded character.
 * @return A zero value is returned if the character could not be encoded
 * @note This function can only be used to add a character to the end the UTF-16 (little endian) encoded content.
 * @note i.e. The current length of \a `Content` should be passed in the parameter \a `Offset`.
 */
size_t JsonUtf16leEncode(uint8_t *Content, size_t Size, size_t Offset, tJsonCharacter Character);


/**
 * @brief Decodes the next character in a UTF-16 (little endian) encoded content
 * @param Content   The UTF-16 (little endian) encoded content
 * @param Length    The length of the UTF-16 (little endian) encoded content
 * @param Offset    The offset to the start of the next character to decode
 * @param Character Used to return the next character
 * @return The decoded length of the next character.
 * @return A zero value is returned if the next character could not be decoded.
 * @note Add the returned length to the \a `Offset` value to advance it to the next character in the content.
 */
size_t JsonUtf16leDecodeNext(const uint8_t *Content, size_t Length, size_t Offset, tJsonCharacter *Character);


#endif
