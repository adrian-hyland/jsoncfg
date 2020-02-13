#ifndef JSON_PATH_H
#define JSON_PATH_H

#include "json_element.h"


int JsonPathSetString(const uint8_t *Path, size_t PathLength, tJsonString *String);


int JsonPathCompareString(const uint8_t *Path, size_t PathLength, tJsonString *String);


size_t JsonPathGetComponent(const uint8_t *Path, size_t PathLength, tJsonType *ComponentType, const uint8_t **Component, size_t *ComponentLength);


#endif
