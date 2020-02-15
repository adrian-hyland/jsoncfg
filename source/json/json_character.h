#ifndef JSON_CHARACTER_H
#define JSON_CHARACTER_H

#include <stdint.h>


/**
 * @brief Checks if a character can be used for a literal value
 * @param Character The character to check
 * @return A non-zero (true) value is returned if the character can be used for a literal value
 * @return A zero (false) value is returned of the character cannot be used for a literal value
 * @note Literal values are assumed to contain just the following set of characters:
 *        - alphanumeric characters  '`0`'...'`9`', '`a`'...'`z`' and '`A`'...'`Z`'
 *        - plus/minus characters    '`-`' and '`+`'
 *        - decimal places character '`.`'
 */
int JsonCharacterIsLiteral(uint8_t Character);


/**
 * @brief Checks if a character is a whitespace character
 * @param Character The character to check
 * @return A non-zero (true) value is returned if the character is a whitespace character
 * @return A zero (false) value is returned of the character is not a whitespace character
 */
int JsonCharacterIsWhitespace(uint8_t Character);


/**
 * @brief Checks if a character needs to be escaped
 * @param Character The character to check
 * @return A non-zero (true) value is returned if the character needs to be escaped
 * @return A zero (false) value is returned of the character does not need to be escaped
 */
int JsonCharacterIsEscapable(uint8_t Character);


/**
 * @brief Gets the escape code for a character
 * @param Character The character to escape
 * @return The character escape code
 */
uint8_t JsonCharacterToEscape(uint8_t Character);


/**
 * @brief Gets the character that corresponds to an escape code
 * @param Character The character escape code
 * @return The character that corresponds to the escape code
 */
uint8_t JsonCharacterFromEscape(uint8_t Character);


#endif
