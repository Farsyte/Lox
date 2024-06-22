#pragma once

#include "bist.h"
#include "common.h"
#include "post.h"

// In Lox, all (non-string) values are floating point.

typedef double Value;

// TODO centralized "extensible array"
typedef struct {
    int capacity;
    int count;
    Value *values;
} ValueArray;

void initValueArray (
    ValueArray *array);
void writeValueArray (
    ValueArray *array,
    Value value);
void freeValueArray (
    ValueArray *array);

post_fn post_value;
bist_fn bist_value;
