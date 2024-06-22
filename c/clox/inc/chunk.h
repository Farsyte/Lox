#pragma once

#include "bist.h"
#include "common.h"
#include "iline.h"
#include "post.h"
#include "value.h"

typedef enum {
    OP__NULL = 0,               // reserve 0x00 as "not initialized"
    OP_CONSTANT,
    OP_CONSTANT_LONG,
    OP_RETURN,
} OpCode;

typedef struct {

    // TODO centralized "extensible array"
    int count;
    int capacity;
    OpCode *code;

    ILine iline[1];             // recover line number for any op index

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

void writeConstant (
    Chunk *chunk,
    Value value,
    int line);

int addConstant (
    Chunk *chunk,
    Value value);

post_fn post_chunk;
bist_fn bist_chunk;
