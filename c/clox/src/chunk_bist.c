#include "assert.h"
#include "bist.h"
#include "chunk.h"

void
bist_chunk (
    )
{
    Chunk c[1];
    initChunk (c);

    assert (0 == c->count, "after initChunk, count must be zero.");
    assert (0 == c->capacity, "after initChunk, capacity must be zero.");
    assert (NULL == c->code, "after initChunk, code pointer  must be zero.");
}
