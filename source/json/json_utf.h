#ifndef JSON_UTF_H
#define JSON_UTF_H

#include <stddef.h>
#include "json_character.h"


/**
 * @brief Enumeration used to define different UTF types
 */
typedef enum
{
	json_UtfUnknown, /**< Unknown UTF type */
	json_Utf8,       /**< UTF-8 */
	json_Utf16be,    /**< UTF-16 (big endian) */
	json_Utf16le     /**< UTF-16 (little endian) */
} tJsonUtfType;


/**
 * @brief The maximum number of bytes that an encoded UTF character can have
 */
#define JSON_UTF_MAX_SIZE 4


/**
 * @brief Type that can hold a UTF encoded character
 */
typedef uint8_t tJsonUtf[JSON_UTF_MAX_SIZE];


/**
 * @brief Determines the type of UTF that is used at the start of a JSON content
 * @param Content The start of the JSON content
 * @param Length  The length of the JSON content
 * @param UtfType Used to return the type of UTF that is being used - \a `json_UtfUnknown` is returned of the type of UTF could not be determined
 * @return The offset to the start of the actual JSON content (skipping over any 'byte order mark' character that may be present)
 */
size_t JsonUtfGetType(const uint8_t *Content, size_t Length, tJsonUtfType *UtfType);


/**
 * @brief Encodes a character in a particular UTF encoding
 * @param UtfType   The type of UTF to use to encode the character
 * @param Content   The UTF encoded content
 * @param Size      The maximum size of UTF encoded content
 * @param Offset    The offset in the UTF encoded content where the character should be encoded
 * @param Character The character to encode
 * @return The length of the encoded character.
 * @return A zero value is returned if the character could not be encoded
 * @note This function can only be used to add a character to the end the UTF encoded content.
 * @note i.e. The current length of \a `Content` should be passed in the parameter \a `Offset`.
 */
size_t JsonUtfEncode(tJsonUtfType UtfType, uint8_t *Content, size_t Size, size_t Offset, tJsonCharacter Character);


/**
 * @brief Decodes the next character in a UTF encoded content
 * @param UtfType   The type of UTF to use to decode the character
 * @param Content   The UTF encoded content
 * @param Length    The length of the UTF encoded content
 * @param Offset    The offset to the start of the next character to decode
 * @param Character Used to return the next character
 * @return The decoded length of the next character.
 * @return A zero value is returned if the next character could not be decoded.
 * @note Add the returned length to the \a `Offset` value to advance it to the next character in the content.
 */
size_t JsonUtfDecode(tJsonUtfType UtfType, const uint8_t *Content, size_t Length, size_t Offset, tJsonCharacter *Character);


#endif
