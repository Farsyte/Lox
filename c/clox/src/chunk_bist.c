#include "assert.h"
#include "bist.h"
#include "chunk.h"

#include <stdio.h>

#define HERE() fprintf(stderr, "%s:%d: HERE.\n", __FILE__, __LINE__)

void
bist_chunk (
    )
{
    Chunk c[1];

    initChunk (c);

    assert (0 == c->count, "after initChunk, count must be zero.");
    assert (0 == c->capacity, "after initChunk, capacity must be zero.");
    assert (NULL == c->code, "after initChunk, code pointer must be NULL.");

    // Check that we can store a RETURN OpCode.

    writeChunk (c, OP_RETURN);
    assert (1 == c->count, "after writeChunk, count must be one.");
    assert (1 <= c->capacity,
        "after writeChunk, capacity must be at least one.");
    assert (NULL != c->code,
        "after writeChunk, code pointer must not be NULL.");
    assert (OP_RETURN == c->code[0],
        "after writeChunk, OP_RETURN must be in place.");

    // Check that we can actually store integers
    // with values from 0 to 255 and read back the
    // same value.

    for (int i = 0; i < 256; ++i) {
        writeChunk (c, (OpCode) i);
    }

    assert (257 == c->count, "after 256x writeChunk, count must be 257.");
    assert (257 <= c->capacity,
        "after 256x writeChunk, capacity must be at least 257.");
    assert (NULL != c->code,
        "after 256x writeChunk, code pointer must not be NULL.");
    for (int i = 0; i < 256; ++i) {
        assert (i == (int) c->code[i + 1],
            "after 256x writeChunk, bytes with value 0..255 must be in place.");
    }

    // Verify that if we attempt to store any integer
    // not in 0..255 inclusive, what we read back is
    // just the low 8 bits of the value.

    writeChunk (c, (OpCode) 300);

    assert (258 == c->count, "after writeChunk of 300, count must be 258.");
    assert (258 <= c->capacity,
        "after writeChunk of 300, capacity must be at least 258.");
    assert (NULL != c->code,
        "after writeChunk of 300, code pointer must not be NULL.");

    assert (44 == (int) c->code[257],
        "after writing 300, readback OpCode should be 44.");

    freeChunk (c);
    assert (0 == c->count, "after freeChunk, count must be zero.");
    assert (0 == c->capacity, "after freeChunk, capacity must be zero.");
    assert (NULL == c->code, "after freeChunk, code pointer must be NULL.");

}
