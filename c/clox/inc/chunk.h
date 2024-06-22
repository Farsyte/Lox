#pragma once

#include "bist.h"
#include "common.h"
#include "post.h"
#include "value.h"

typedef enum {
    OP_CONSTANT,
    OP_RETURN,
} OpCode;

typedef struct {

    // TODO centralized "extensible array"
    int count;
    int capacity;
    OpCode *code;
    int *lines;                 // parallel line number list

    ValueArray constants[1];

} Chunk;

void initChunk (
    Chunk *chunk);

void freeChunk (
    Chunk *chunk);

void writeChunk (
    Chunk *chunk,
    OpCode op,
    int line);

int addConstant (
    Chunk *chunk,
    Value value);

post_fn post_chunk;
bist_fn bist_chunk;
