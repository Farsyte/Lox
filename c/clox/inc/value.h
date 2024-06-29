#pragma once

#include "common.h"

/** @file value.h
 * @brief Macros and API exported  by the value module
 */

struct value_array_s {
    int capacity;
    int count;
    Value *values;
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
