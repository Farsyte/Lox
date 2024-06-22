#include "assert.h"
#include "bist.h"
#include "chunk.h"
#include "chunk_debug.h"

#include <stdio.h>

#define HERE() fprintf(stderr, "%s:%d: HERE.\n", __FILE__, __LINE__)

static bist_fn bist_chunk_numbers;
static bist_fn bist_chunk_opcodes;

void
bist_chunk (
    )
{
    bist_chunk_numbers ();
    bist_chunk_opcodes ();
}

static void
bist_chunk_numbers (
    )
{
    Chunk c[1];

    initChunk (c);

    assert (0 == c->count, "after initChunk, count must be zero.");
    assert (0 == c->capacity, "after initChunk, capacity must be zero.");
    assert (NULL == c->code, "after initChunk, code pointer must be NULL.");

    // Check that we can actually store integers
    // with values from 0 to 255 and read back the
    // same value.

    for (int i = 0; i < 256; ++i) {
        writeChunk (c, (OpCode) i);
    }

    assert (256 == c->count, "after 256x writeChunk, count must be 256.");
    assert (256 <= c->capacity,
        "after 256x writeChunk, capacity must be at least 256.");
    assert (NULL != c->code,
        "after 256x writeChunk, code pointer must not be NULL.");
    for (int i = 0; i < 256; ++i) {
        assert (i == (int) c->code[i],
            "after 256x writeChunk, bytes with value 0..255 must be in place.");
    }

    // Verify that if we attempt to store any integer
    // not in 0..255 inclusive, what we read back is
    // just the low 8 bits of the value.

    writeChunk (c, (OpCode) 300);

    assert (257 == c->count, "after writeChunk of 300, count must be 257.");
    assert (257 <= c->capacity,
        "after writeChunk of 300, capacity must be at least 257.");
    assert (NULL != c->code,
        "after writeChunk of 300, code pointer must not be NULL.");

    assert (44 == (int) c->code[256],
        "after writing 300, readback OpCode should be 44.");

    freeChunk (c);
    assert (0 == c->count, "after freeChunk, count must be zero.");
    assert (0 == c->capacity, "after freeChunk, capacity must be zero.");
    assert (NULL == c->code, "after freeChunk, code pointer must be NULL.");

}

static void
bist_chunk_opcodes (
    )
{

    Chunk c[1];

    initChunk (c);

    assert (0 == c->count, "after initChunk, count must be zero.");
    assert (0 == c->capacity, "after initChunk, capacity must be zero.");
    assert (NULL == c->code, "after initChunk, code pointer must be NULL.");

    assert (0 == c->constants->count,
        "after initChunk, constants count must be zero.");
    assert (0 == c->constants->capacity,
        "after initChunk, constants capacity must be zero.");
    assert (NULL == c->constants->values,
        "after initChunk, constants pointer must be NULL.");

    // Check that we can store a RETURN OpCode.

    writeChunk (c, OP_RETURN);
    assert (1 == c->count, "after writeChunk, count must be one.");
    assert (1 <= c->capacity,
        "after writeChunk, capacity must be at least one.");
    assert (NULL != c->code,
        "after writeChunk, code pointer must not be NULL.");
    assert (OP_RETURN == c->code[0],
        "after writeChunk, OP_RETURN must be in place.");

    Value tv = 133.7;

    addConstant (c, tv);
    assert (1 == c->constants->count,
        "after addConstant, constants count must be one.");
    assert (1 <= c->constants->capacity,
        "after addConstant, constants capacity must be at least one.");
    assert (NULL != c->constants->values,
        "after addConstant, constants values pointer must not be NULL.");
    assert (tv == c->constants->values[0],
        "after addConstant, test value must be in place.");

    disassembleChunk (c, "test chunk");

    freeChunk (c);
    assert (0 == c->count, "after freeChunk, count must be zero.");
    assert (0 == c->capacity, "after freeChunk, capacity must be zero.");
    assert (NULL == c->code, "after freeChunk, code pointer must be NULL.");

    assert (0 == c->constants->count,
        "after initChunk, constants count must be zero.");
    assert (0 == c->constants->capacity,
        "after initChunk, constants capacity must be zero.");
    assert (NULL == c->constants->values,
        "after initChunk, constants pointer must be NULL.");
}
