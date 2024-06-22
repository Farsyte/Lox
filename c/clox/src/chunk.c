#include "chunk.h"

#include "assert.h"
#include "memory.h"

#include <stdio.h>

void
initChunk (
    Chunk *chunk)
{
    assert (NULL != chunk, "chunk must not be NULL.");

    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    chunk->lines = NULL;

    initValueArray (chunk->constants);
}

void
freeChunk (
    Chunk *chunk)
{
    FREE_ARRAY (OpCode,
        chunk->code,
        chunk->capacity);

    FREE_ARRAY (int,
        chunk->lines,
        chunk->capacity);

    freeValueArray (chunk->constants);
    initChunk (chunk);
}

void
writeChunk (
    Chunk *chunk,
    OpCode op,
    int line)
{
    assert (NULL != chunk, "chunk must not be NULL.");

    if (chunk->capacity <= chunk->count) {
        int oldCapacity = chunk->capacity;
        int newCapacity = GROW_CAPACITY (oldCapacity);
        chunk->code = GROW_ARRAY (OpCode,
            chunk->code,
            oldCapacity,
            newCapacity);

        chunk->lines = GROW_ARRAY (int,
            chunk->lines,
            oldCapacity,
            newCapacity);

        chunk->capacity = newCapacity;
    }

    chunk->code[chunk->count] = op;
    chunk->lines[chunk->count] = line;
    chunk->count++;
}

int
addConstant (
    Chunk *chunk,
    Value value)
{
    int at = chunk->constants->count;

    writeValueArray (chunk->constants, value);
    return at;
}
