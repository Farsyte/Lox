#pragma once

#include "common.h"

/** @file value.h
 * @brief Macros and API exported  by the value module
 */

/** Enumeration of Value Types
 */
enum ValueType {
    VAL_BOOL,                   ///< either TRUE or FALSE
    VAL_NIL,                    ///< just NIL
    VAL_NUMBER,                 ///< a floating point number
    VAL_OBJ,                    ///< an object on the heap
};

/** Representation of a Value.
 *
 * This is a discriminated union of the storage
 * for each valid type of data in Lox.
 */
struct Value {
    ValueType type;             ///< selects the union member
    union {
        bool boolean;           ///< true or false
        double number;          ///< floating point number
        Obj *obj;               ///< allocated heap memory
    } as;                       ///< union of different types of values
};

/** Ask if a Value is a Boolean */
#define IS_BOOL(value) ((value).type == VAL_BOOL)

/** Ask if a Value is a NIL */
#define IS_NIL(value) ((value).type == VAL_NIL)

/** Ask if a Value is a Number */
#define IS_NUMBER(value) ((value).type == VAL_NUMBER)

/** Ask if a Value is an Object */
#define IS_OBJ(value) ((value).type == VAL_OBJ)

/** Convert a Value to a native boolean
 * Results undefined if .type is not VAL_BOOL
 */
#define AS_BOOL(value) ((value).as.boolean)

/** Convert a Value to a native number
 * Results undefined if .type is not VAL_NUMBER
 */
#define AS_NUMBER(value) ((value).as.number)

/** Convert a Value to an Object pointer
 * Results undefined if .type is not VAL_OBJ
 */
#define AS_OBJ(value) ((value).as.obj)

/** Promote a native boolean to a Value */
#define BOOL_VAL(value) ((Value){VAL_BOOL, {.boolean = value}})

/** Produce a Nil Value */
#define NIL_VAL ((Value){VAL_NIL, {.number = 0}})

/** Promote a native number to a Value */
#define NUMBER_VAL(value) ((Value){VAL_NUMBER, {.number = value}})

/** Promote an object (pointer) to a Value */
#define OBJ_VAL(object) ((Value){VAL_OBJ, {.obj = (Obj*)(object)}})

/** Expandable Array of Values
 */
struct ValueArray {
    int capacity;               ///< storage capacity available
    int count;                  ///< storage capacity in use
    Value *values;              ///< storage area with capacity
};

extern bool valuesEqual (Value a, Value b);

extern void initValueArray (ValueArray *array);
extern void writeValueArray (ValueArray *array, Value value);
extern void freeValueArray (ValueArray *array);
extern void printValue (Value value);

extern void bistValue ();
extern void postValue ();
