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
    json_TypeArray,        /**< Array element */
    json_TypeComment,      /**< Comment element */
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
 * @brief Allocates a child for an element
 * @param Element The element to allocate a child for
 * @param Type    The type of child to allocate
 * @return A true value is returned if a child was successfully allocated for the element
 * @return A false value is returned if a child could not be allocated for the element
 */
bool JsonElementAllocateChild(tJsonElement *Element, tJsonType Type);


/**
 * @brief Allocates a sibling for an element
 * @param Element The element to allocate a sibling for
 * @param Type    The type of sibling to allocate
 * @return A true value is returned if a sibling was successfully allocated for the element
 * @return A false value is returned if a sibling could not be allocated for the element
 */
bool JsonElementAllocateNext(tJsonElement *Element, tJsonType Type);


/**
 * @brief Gets the type of a JSON element
 * @param Element The element
 * @return The element type
 */
tJsonType JsonElementGetType(tJsonElement *Element);


/**
 * @brief Gets the child of a JSON element
 * @param Element        The element
 * @param IgnoreComments Indicates whether to disregard any \a 'Comment' child elements 
 * @return The child element
 */
tJsonElement *JsonElementGetChild(tJsonElement *Element, bool IgnoreComments);


/**
 * @brief Gets the next sibling of a JSON element
 * @param Element        The element
 * @param IgnoreComments Indicates whether to disregard any \a 'Comment' sibling elements 
 * @return The next sibling element
 */
tJsonElement *JsonElementGetNext(tJsonElement *Element, bool IgnoreComments);


/**
 * @brief Finds a JSON element
 * @param Element The element
 * @param Path    The path of elements to find
 * @param Create  Indicates if the elements in the path should be created if they do not exist
 * @return The last element in the path that was found
 * @return A \a `NULL` value is returned if any of the elements in the path could not be found (if \a `Create` is zero) or created (if \a `Create` is non-zero)
 * @note The type of parameter \a `Element` must be equal to \a `json_TypeRoot` - use \a `JsonElementSetUp()` to set up an appropriate element
 */
tJsonElement *JsonElementFind(tJsonElement *Element, const uint8_t *Path, bool Create);


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
