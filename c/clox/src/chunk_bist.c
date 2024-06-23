#include "chunk.h"

#include "assert.h"
#include "bist.h"
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
        writeChunk (c, (OpCode) i, 123);
    }

    assert (256 == c->count, "after 256x writeChunk, count must be 256.");
    assert (256 <= c->capacity,
        "after 256x writeChunk, capacity must be at least 256.");
    assert (NULL != c->code,
        "after 256x writeChunk, code pointer must not be NULL.");
    for (int i = 0; i < 256; ++i) {
        assert (i == c->code[i],
            "after 256x writeChunk, bytes with value 0..255 must be in place.");
    }

    // Verify that if we attempt to store any integer
    // not in 0..255 inclusive, what we read back is
    // just the low 8 bits of the value.

    writeChunk (c, (OpCode) 300, 123);

    assert (257 == c->count, "after writeChunk of 300, count must be 257.");
    assert (257 <= c->capacity,
        "after writeChunk of 300, capacity must be at least 257.");
    assert (NULL != c->code,
        "after writeChunk of 300, code pointer must not be NULL.");

    assert (44 == c->code[256],
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
    int old_count;
    int old_ccount;
    Chunk c[1];
    Value tv1 = 133.7;
    Value tv2 = 420.69;

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

    // Check that we can store a CONSTANT OpCode.

    old_count = c->count;
    old_ccount = c->constants->count;
    writeConstant (c, tv1, 123);

    assert (old_ccount + 1 == c->constants->count,
        "after addConstant, constants count must increment.");
    assert (c->constants->count <= c->constants->capacity,
        "after addConstant, constants capacity must be at least the count.");
    assert (NULL != c->constants->values,
        "after addConstant, constants values pointer must not be NULL.");
    assert (tv1 == c->constants->values[old_count],
        "after addConstant, test value must be in place.");

    assert (old_count + 2 == c->count,
        "after writeConstant, count must increment by two.");
    assert (c->count <= c->capacity,
        "after writeConstant, capacity must be at least the count.");
    assert (NULL != c->code,
        "after writeConstant, code pointer must not be NULL.");
    assert (OP_CONSTANT == c->code[old_count],
        "after writeConstant, OP_CONSTANT must be in place.");
    assert (old_ccount == c->code[old_count + 1],
        "after writeConstant, constant index must be in place.");

    // Check that we can store a CONSTANT_LONG OpCode.

    // Need to stuff the constant pool so the next one we
    // write must be more than one byte of index.

    for (int i = 0; i < 999990; ++i) {
        addConstant (c, (double) i);
    }

    old_count = c->count;
    old_ccount = c->constants->count;
    writeConstant (c, tv2, 126);

    assert (old_ccount + 1 == c->constants->count,
        "after addConstant, constants count must increment.");
    assert (c->constants->count <= c->constants->capacity,
        "after addConstant, constants capacity must be at least the count.");
    assert (NULL != c->constants->values,
        "after addConstant, constants values pointer must not be NULL.");
    assert (tv2 == c->constants->values[old_ccount],
        "after addConstant, test value must be in place.");

    assert (old_count + 4 == c->count,
        "after writeConstant, count must increment by four.");
    assert (c->count <= c->capacity,
        "after writeConstant, capacity must be at least the count.");
    assert (NULL != c->code,
        "after writeConstant, code pointer must not be NULL.");
    assert (OP_CONSTANT_LONG == c->code[old_count],
        "after writeConstant, OP_CONSTANT must be in place.");
    assert (((old_ccount) & 255) == c->code[old_count + 1],
        "after writeConstant, constant index must be in place.");
    assert (((old_ccount >> 8) & 255) == c->code[old_count + 2],
        "after writeConstant, constant index must be in place.");
    assert (((old_ccount >> 16) & 255) == c->code[old_count + 3],
        "after writeConstant, constant index must be in place.");

    // Check that we can store a RETURN OpCode.

    old_count = c->count;
    writeChunk (c, OP_RETURN, 126);
    assert (old_count + 1 == c->count,
        "after writeChunk, count must increment.");
    assert (c->count <= c->capacity,
        "after writeChunk, capacity must be at least the count.");
    assert (NULL != c->code,
        "after writeChunk, code pointer must not be NULL.");
    assert (OP_RETURN == c->code[old_count],
        "after writeChunk, OP_RETURN must be in place.");

    for (int i = 0; i < c->iline->count; ++i)
        fprintf (stderr, "iline[%d]: line %d through offset %d\n",
            i, c->iline->data[i].line, c->iline->data[i].offset);

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
