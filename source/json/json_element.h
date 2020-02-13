#ifndef JSON_ELEMENT_H
#define JSON_ELEMENT_H

#include "json_string.h"


typedef enum
{
    json_TypeRoot,
    json_TypeKey,
    json_TypeValueString,
    json_TypeValueLiteral,
    json_TypeObject,
    json_TypeArray
} tJsonType;


typedef struct tJsonElement
{
    tJsonType            Type;
    struct tJsonElement *Parent;
    struct tJsonElement *Next;
    struct tJsonElement *Child;
    tJsonString          Name;
} tJsonElement;


tJsonElement *JsonElementAllocate(tJsonType Type, tJsonElement *Parent);


void JsonElementFree(tJsonElement *Element);


void JsonElementSetUp(tJsonElement *Element);


void JsonElementCleanUp(tJsonElement *Element);


tJsonElement *JsonElementFind(tJsonElement *Element, const uint8_t *Path, int Create);


tJsonElement *JsonElementMoveChild(tJsonElement *Element, tJsonElement *Value);


#endif
