#ifndef JSON_ELEMENT_H
#define JSON_ELEMENT_H

#include "json_string.h"


/**
 * @brief Enumeration used to define different JSON element types
 */
typedef enum
{
    json_TypeRoot,         /**< Root element */
    json_TypeKey,          /**< Key element*/
    json_TypeValueString,  /**< String value element */
    json_TypeValueLiteral, /**< Literal value element (for \a null, \a boolean or \a numeric values) */
    json_TypeObject,       /**< Object element */
    json_TypeArray         /**< Array element */
} tJsonType;


/**
 * @brief Type used for a JSON element
 */
typedef struct tJsonElement
{
    tJsonType            Type;   /**< The element type */
    struct tJsonElement *Parent; /**< The parent element */
    struct tJsonElement *Next;   /**< The next (sibling) element */
    struct tJsonElement *Child;  /**< The child element */
    tJsonString          Name;   /**< The element name */
} tJsonElement;


/**
 * @brief Allocates a new JSON element
 * @param Type   The type of element to allocate
 * @param Parent The parent of the new JSON element
 * @return The allocated JSON element
 * @return A \a `NULL` value is returned if a new JSON element could not be allocated
 * @note Use \a `JsonElementFree()` to free the element when it is no longer required
 */
tJsonElement *JsonElementAllocate(tJsonType Type, tJsonElement *Parent);


/**
 * @brief Frees an allocated JSON element
 * @param Element A pointer to an allocated JSON element
 * @return None
 * @note The parameter \a `Element` must point to an element that was allocated by \a `JsonElementAllocate()`
 */
void JsonElementFree(tJsonElement **Element);


/**
 * @brief Sets up a JSON element
 * @param Element The element to set up
 * @return None
 * @note The type of parameter \a `Element` will be set to \a `json_TypeRoot`
 * @note Use \a `JsonElementCleanUp()` to clean up the element when it is no longer required
 */
void JsonElementSetUp(tJsonElement *Element);


/**
 * @brief Cleans up a JSON element
 * @param Element The element to clean up
 * @return None
 */
void JsonElementCleanUp(tJsonElement *Element);


/**
 * @brief Finds a JSON element
 * @param Element The element
 * @param Path    The path of elements to find
 * @param Create  Indicates if the elements in the path should be created if they do not exist
 * @return The last element in the path that was found
 * @return A \a `NULL` value is returned if any of the elements in the path could not be found (if \a `Create` is zero) or created (if \a `Create` is non-zero)
 * @note The type of parameter \a `Element` must be equal to \a `json_TypeRoot` - use \a `JsonElementSetUp()` to set up an appropriate element
 */
tJsonElement *JsonElementFind(tJsonElement *Element, const uint8_t *Path, int Create);


/**
 * @brief Moves the child elements from one element to another
 * @param To   The element to move the children to
 * @param From The element to move the children from
 * @return The first child element that was moved
 * @return A \a `NULL` value is returned if the children could not be moved
 * @note If the parameter \a `To` has any children then these will get freed
 * @note The parameter \a `From` will not have any children after they have been moved (they are not shared)
 */
tJsonElement *JsonElementMoveChild(tJsonElement *To, tJsonElement *From);


#endif
