#ifndef JSON_PATH_H
#define JSON_PATH_H

#include "json_element.h"


/**
 * @brief Type used for a JSON path
 */
typedef struct
{
	const uint8_t *Value;  /* The path value */
	size_t         Length; /* The length of the path value (in bytes) */
} tJsonPath;


/**
 * @brief Returns a JSON path using a UTF-8 encoded path name string
 * @param PathString The path name string (null terminated)
 * @return The JSON path
 * @note The life time of the returned JSON path must be longer than than \a 'PathString'.
 */
tJsonPath JsonPathUtf8(const uint8_t *PathString);


/**
 * @brief Returns a JSON path using an ASCII encoded path name string
 * @param PathString The path name string (null terminated)
 * @return The JSON path
 * @note The life time of the returned JSON path must be longer than than \a 'PathString'.
 */
tJsonPath JsonPathAscii(const char *PathString);


/**
 * @brief Gets the left part of a JSON path
 * @param Path     The JSON path
 * @param ToOffset The offset up to where the left part is taken from
 * @return The left part between [0, ToOffset) of the JSON path
 * @note The life time of the returned JSON path must have a longer life time than the orignal \a 'Path'.
 */
tJsonPath JsonPathLeft(tJsonPath Path, size_t ToOffset);


/**
 * @brief Gets the right part of a JSON path
 * @param Path       The JSON path
 * @param FromOffset The offset from where the right part is taken from
 * @return The right part between [FromOffset, Path.Length) of the JSON path
 * @note The life time of the returned JSON path must have a longer life time than the orignal \a 'Path'.
 */
tJsonPath JsonPathRight(tJsonPath Path, size_t FromOffset);


/**
 * @brief Gets the middle part of a JSON path
 * @param Path       The JSON path
 * @param FromOffset The offset from where the middle part is taken from
 * @param ToOffset   The offset up to where the left part is taken from
 * @return The middle part between [FromOffset, ToOffset) of the JSON path
 * @note The life time of the returned JSON path must have a longer life time than the orignal \a 'Path'.
 * @note The value of \a 'ToOffset' should be greater than \a 'FromOffset' - the returned path will have a zero length otherwise.
 */
tJsonPath JsonPathMiddle(tJsonPath Path, size_t FromOffset, size_t ToOffset);


/**
 * @brief Gets the next character in a JSON path
 * @param Path      The JSON path
 * @param Offset    The offset of the character to retrieve
 * @param IsEscaped Used to return a boolean value that indicates whether the next character had been escaped or not
 * @param Character Used to return the next character value
 * @return The length (in bytes) that the encoded character takes up in the path (do not assume a fixed length).
 * @return A zero value is returned if a character could not be returned.
 * @note Add the returned length to the \a `Offset` value to advance it to the next character in the path.
 */
size_t JsonPathGetNextCharacter(tJsonPath Path, size_t Offset, bool *IsEscaped, uint8_t *Character);


/**
 * @brief Gets the previous character in a JSON path
 * @param Path      The JSON path
 * @param Offset    The offset of the character to retrieve
 * @param IsEscaped Used to return a boolean value that indicates whether the previous character had been escaped or not
 * @param Character Used to return the previous character value
 * @return The length (in bytes) that the encoded character takes up in the path (do not assume a fixed length).
 * @return A zero value is returned if a character could not be returned.
 * @note Pass \a 'Path.Length' in the \a 'Offset' parameter to get the last character in the path.
 * @note Subtract the returned length from the \a `Offset` value to advance it to the previous character in the path.
 */
size_t JsonPathGetPreviousCharacter(tJsonPath Path, size_t Offset, bool *IsEscaped, uint8_t *Character);


/**
 * @brief Set a string to the value of a path name
 * @param Path   The path name
 * @param String The string
 * @return A true value is returned if the string was successfully set to the path name
 * @return A false value is returned if the string could not be set to the path name
 */
bool JsonPathSetString(tJsonPath Path, tJsonString *String);


/**
 * @brief Compares a path name with a string
 * @param Path   The path name
 * @param String The string
 * @return A true value is returned if the path name and string are equal
 * @return A false value is returned if the path name and string are not equal
 */
bool JsonPathCompareString(tJsonPath Path, tJsonString *String);


/**
 * @brief Gets a component of a path
 * @param Path          The path
 * @param ComponentType Used to return the path component type
 * @param Component     Used to return the path component name
 * @return The length of the path component
 * @return A zero value is returned if the path component could not be returned
 */
size_t JsonPathGetComponent(tJsonPath Path, tJsonType *ComponentType, tJsonPath *Component);


#endif
