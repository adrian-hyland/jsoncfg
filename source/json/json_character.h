#ifndef JSON_CHARACTER_H
#define JSON_CHARACTER_H

#include <stdint.h>


int JsonCharacterIsLiteral(uint8_t Character);


int JsonCharacterIsWhitespace(uint8_t Character);


int JsonCharacterIsEscapable(uint8_t Character);


uint8_t JsonCharacterToEscape(uint8_t Character);


uint8_t JsonCharacterFromEscape(uint8_t Character);


#endif
