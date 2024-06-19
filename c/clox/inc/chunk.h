#pragma once

#include "common.h"
#include "value.h"

typedef uint8_t Byte;

typedef enum {
    OP_CONSTANT,
    OP_NEGATE,
    OP_RETURN,
} OpCode;

typedef struct {
    int count;
    int capacity;
    Byte* code;
    int* lines;
    ValueArray constants[1];
} Chunk;

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, Byte byte, int line);

int addConstant(Chunk* chunk, Value value);
