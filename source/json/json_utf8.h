#ifndef JSON_UTF8_H
#define JSON_UTF8_H

#include <stddef.h>
#include "json_character.h"


/**
 * @brief The maximum number of bytes that an encoded UTF-8 character can have
 */
#define JSON_UTF8_MAX_SIZE 4


/**
 * @brief Type that can hold a UTF-8 encoded character
 */
typedef uint8_t tJsonUtf8[JSON_UTF8_MAX_SIZE];


/**
 * @brief Encodes a character in UTF-8
 * @param Content   The UTF-8 encoded content
 * @param Size      The maximum size of UTF-8 encoded content
 * @param Offset    The offset in the UTF-8 encoded content where the character should be encoded
 * @param Character The character to encode
 * @return The length of the encoded character.
 * @return A zero value is returned if the character could not be encoded
 * @note This function can only be used to add a character to the end the UTF-8 encoded content.
 * @note i.e. The current length of \a `Content` should be passed in the parameter \a `Offset`.
 */
size_t JsonUtf8Encode(uint8_t *Content, size_t Size, size_t Offset, tJsonCharacter Character);


/**
 * @brief Decodes the next character in a UTF-8 encoded content
 * @param Content   The UTF-8 encoded content
 * @param Length    The length of the UTF-8 encoded content
 * @param Offset    The offset to the start of the next character to decode
 * @param Character Used to return the next character
 * @return The decoded length of the next character.
 * @return A zero value is returned if the next character could not be decoded.
 * @note Add the returned length to the \a `Offset` value to advance it to the next character in the content.
 */
size_t JsonUtf8DecodeNext(const uint8_t *Content, size_t Length, size_t Offset, tJsonCharacter *Character);


/**
 * @brief Decodes the previous character in a UTF-8 encoded content
 * @param Content   The UTF-8 encoded content
 * @param Length    The length of the UTF-8 encoded content
 * @param Offset    The offset to the end of the previous character to decode
 * @param Character Used to return the previous character
 * @return The decoded length of the previous character.
 * @return A zero value is returned if the previous character could not be decoded.
 * @note Pass \a 'Length' in the \a 'Offset' parameter to get the last character in the content.
 * @note Subtract the returned length from the \a `Offset` value to advance it to the previous character in the content.
 */
size_t JsonUtf8DecodePrevious(const uint8_t *Content, size_t Length, size_t Offset, tJsonCharacter *Character);


#endif
