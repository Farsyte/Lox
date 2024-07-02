#pragma once

#include "common.h"

/** @file value.h
 * @brief Macros and API exported  by the value module
 */

/** Expandable Array of Values
 */
struct value_array_s {
    int capacity;               ///< storage capacity available
    int count;                  ///< storage capacity in use
    Value *values;              ///< storage area with capacity
};

extern void initValueArray (
    ValueArray *array);

extern void writeValueArray (
    ValueArray *array,
    Value value);

extern void freeValueArray (
    ValueArray *array);

extern void printValue (
    Value value);

extern void bistValue (
    );
extern void postValue (
    );
