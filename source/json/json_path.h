#ifndef JSON_PATH_H
#define JSON_PATH_H

#include "json_element.h"


/**
 * @brief Set a string to the value of a path name
 * @param Path       The path name
 * @param PathLength The length of the path name
 * @param String     The string
 * @return A non-zero (true) value is returned if the string was successfully set to the path name
 * @return A zero (false) value is returned if the string could not be set to the path name
 */
int JsonPathSetString(const uint8_t *Path, size_t PathLength, tJsonString *String);


/**
 * @brief Compares a path name with a string
 * @param Path       The path name
 * @param PathLength The length of the path name
 * @param String     The string
 * @return A non-zero (true) value is returned if the path name and string are equal
 * @return A zero (false) value is returned if the path name and string are not equal
 */
int JsonPathCompareString(const uint8_t *Path, size_t PathLength, tJsonString *String);


/**
 * @brief Gets a component of a path
 * @param Path            The path
 * @param PathLength      The length of the path
 * @param ComponentType   Used to return the path component type
 * @param Component       Used to return the path component name
 * @param ComponentLength Used to return the length of the path component name
 * @return The length of the path component
 * @return A zero value is returned if the path component could not be returned
 */
size_t JsonPathGetComponent(const uint8_t *Path, size_t PathLength, tJsonType *ComponentType, const uint8_t **Component, size_t *ComponentLength);


#endif
