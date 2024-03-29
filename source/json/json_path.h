#ifndef JSON_PATH_H
#define JSON_PATH_H

#include "json_string.h"
#include "json_type.h"


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
 * @return The JSON path.
 * @note The life time of the returned JSON path must be longer than than \a 'PathString'.
 */
tJsonPath JsonPathUtf8(const uint8_t *PathString);


/**
 * @brief Returns a JSON path using an ASCII encoded path name string
 * @param PathString The path name string (null terminated)
 * @return The JSON path.
 * @note The life time of the returned JSON path must be longer than than \a 'PathString'.
 */
tJsonPath JsonPathAscii(const char *PathString);


/**
 * @brief Gets the left part of a JSON path
 * @param Path     The JSON path
 * @param ToOffset The offset (in bytes) up to where the left part is taken from
 * @return The left part between [0, ToOffset) of the JSON path.
 * @note The life time of the returned JSON path must have a longer life time than the orignal \a 'Path'.
 */
tJsonPath JsonPathLeft(tJsonPath Path, size_t ToOffset);


/**
 * @brief Gets the right part of a JSON path
 * @param Path       The JSON path
 * @param FromOffset The offset (in bytes) from where the right part is taken from
 * @return The right part between [FromOffset, Path.Length) of the JSON path.
 * @note The life time of the returned JSON path must have a longer life time than the orignal \a 'Path'.
 */
tJsonPath JsonPathRight(tJsonPath Path, size_t FromOffset);


/**
 * @brief Gets the middle part of a JSON path
 * @param Path       The JSON path
 * @param FromOffset The offset (in bytes) from where the middle part is taken from
 * @param ToOffset   The offset (in bytes) up to where the left part is taken from
 * @return The middle part between [FromOffset, ToOffset) of the JSON path.
 * @note The life time of the returned JSON path must have a longer life time than the orignal \a 'Path'.
 * @note The value of \a 'ToOffset' should be greater than \a 'FromOffset' - the returned path will have a zero length otherwise.
 */
tJsonPath JsonPathMiddle(tJsonPath Path, size_t FromOffset, size_t ToOffset);


/**
 * @brief Gets the next character from a JSON path
 * @param Path      The JSON path
 * @param Offset    The offset to the start of the next character to get
 * @param IsEscaped Used to return a boolean value that indicates whether the next character has been escaped or not
 * @param Character Used to return the next character
 * @return The encoded length of the character that was returned.
 * @return A zero value is returned if a character could not be returned.
 * @note Add the returned length to the \a `Offset` value to advance it to the next character in the path.
 */
size_t JsonPathGetNextCharacter(tJsonPath Path, size_t Offset, bool *IsEscaped, tJsonCharacter *Character);


/**
 * @brief Gets the previous character from a JSON path
 * @param Path      The JSON path
 * @param Offset    The offset to the end of the previous character to get
 * @param IsEscaped Used to return a boolean value that indicates whether the previous character has been escaped or not
 * @param Character Used to return the previous character
 * @return The encoded length of the character that was returned.
 * @return A zero value is returned if a character could not be returned.
 * @note Pass \a 'Path.Length' in the \a 'Offset' parameter to get the last character in the path.
 * @note Subtract the returned length from the \a `Offset` value to advance it to the previous character in the path.
 */
size_t JsonPathGetPreviousCharacter(tJsonPath Path, size_t Offset, bool *IsEscaped, tJsonCharacter *Character);


/**
 * @brief Gets the string value of a JSON path
 * @param Path   The JSON path
 * @param String Used to return the string value
 * @return A true value if the path is successfully returned in the string.
 * @return A false value if the path could not be returned in the string.
 */
bool JsonPathGetString(tJsonPath Path, tJsonString *String);


/**
 * @brief Compares a JSON path with a string
 * @param Path   The JSON path
 * @param String The string
 * @return A true value if the path and string are equal.
 * @return A false value if the path and string are not equal.
 */
bool JsonPathCompareString(tJsonPath Path, tJsonString *String);


/**
 * @brief Gets a component of a JSON path
 * @param Path          The JSON path
 * @param ComponentType Used to return the path component type
 * @param Component     Used to return the path component name
 * @return The length of the path component.
 * @return A zero value is returned if the path component could not be returned.
 */
size_t JsonPathGetComponent(tJsonPath Path, tJsonType *ComponentType, tJsonPath *Component);


#endif
