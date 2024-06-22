#include "chunk.h"

#include "assert.h"
#include "memory.h"

#include <stdio.h>

static int addConstant (
    Chunk *chunk,
    Value value);

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
    assert (op == (op & 0xFF), "OpCode must fit in 8 bits.");

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

void
writeConstant (
    Chunk *chunk,
    Value value,
    int line)
{
    int index = addConstant (chunk, value);

    if (index == (index & 0xFF)) {
        writeChunk (chunk, OP_CONSTANT, line);
        writeChunk (chunk, index, line);
    } else {
        assert (index == (index & 0xFFFFFF),
            "index MUST fit within 24 bits.");
        writeChunk (chunk, OP_CONSTANT_LONG, line);
        writeChunk (chunk, (index) & 255, line);
        writeChunk (chunk, (index >> 8) & 255, line);
        writeChunk (chunk, (index >> 16) & 255, line);
    }
}

int
addConstant (
    Chunk *chunk,
    Value value)
{
    // TODO: add deduplication, but avoiding O(N²) runtime for
    // inserting N constants. Parallel array of (Value, Index) sorted
    // by Value, that we can check with a binary search?

    int at = chunk->constants->count;

    writeValueArray (chunk->constants, value);
    return at;
}
