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
}

void
writeChunk (
    Chunk *chunk,
    OpCode op)
{
    assert (NULL != chunk, "chunk must not be NULL.");

    if (chunk->capacity <= chunk->count) {
        int oldCapacity = chunk->capacity;
        int newCapacity = GROW_CAPACITY (oldCapacity);
        chunk->code = GROW_ARRAY (OpCode,
            chunk->code,
            oldCapacity,
            newCapacity);

        chunk->capacity = newCapacity;
    }

    chunk->code[chunk->count] = op;
    chunk->count++;
}
