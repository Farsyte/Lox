#include "chunk.h"

#include "assert.h"

void
initChunk (
    Chunk *chunk)
{
    assert (NULL != chunk, "Attempt to initialize a chunk at NULL");

    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
}
