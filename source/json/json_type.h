#ifndef JSON_TYPE_H
#define JSON_TYPE_H


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


#endif
