#ifndef JSON_CHARACTER_H
#define JSON_CHARACTER_H

#include <stdint.h>
#include <stdbool.h>


/**
 * @brief The value of the unicode replacement character
 */
#define JSON_CHARACTER_REPLACEMENT 0xFFFD


/**
 * @brief Type used to hold a unicode character
 */
typedef uint32_t tJsonCharacter;


/**
 * @brief Checks if a character can be used for a literal value
 * @param Character The character to check
 * @return A true value is returned if the character can be used for a literal value
 * @return A false value is returned of the character cannot be used for a literal value
 * @note Literal values are assumed to contain just the following set of characters:
 *        - alphanumeric characters  '`0`'...'`9`', '`a`'...'`z`' and '`A`'...'`Z`'
 *        - plus/minus characters    '`-`' and '`+`'
 *        - decimal places character '`.`'
 */
bool JsonCharacterIsLiteral(tJsonCharacter Character);


/**
 * @brief Checks if a character is a whitespace character
 * @param Character The character to check
 * @return A true value is returned if the character is a whitespace character
 * @return A false value is returned of the character is not a whitespace character
 */
bool JsonCharacterIsWhitespace(tJsonCharacter Character);


/**
 * @brief Checks if a character needs to be escaped
 * @param Character The character to check
 * @return A true value is returned if the character needs to be escaped
 * @return A false value is returned of the character does not need to be escaped
 */
bool JsonCharacterIsEscapable(tJsonCharacter Character);


/**
 * @brief Gets the escape code for a character
 * @param Character The character to escape
 * @return The character escape code
 */
tJsonCharacter JsonCharacterToEscape(tJsonCharacter Character);


/**
 * @brief Gets the character that corresponds to an escape code
 * @param Character The character escape code
 * @return The character that corresponds to the escape code
 */
tJsonCharacter JsonCharacterFromEscape(tJsonCharacter Character);


#endif
