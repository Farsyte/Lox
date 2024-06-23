#include "chunk.h"

#include "assert.h"
#include "post.h"

void
post_chunk (
    )
{
    Chunk c;

    assert (1 == sizeof c.code[0],
        "Expecting the Chunk Code array entries to be 1 byte.");
}
