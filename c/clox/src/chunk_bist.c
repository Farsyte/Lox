#include "chunk.h"

/** @file chunk_bist.c
 * @brief Built-In Self Test for the CHUNK module.
 */

/** Run all BIST cases for Chunk.
 */
void
bistChunk (
    )
{
    Chunk chunk;

    initChunk (&chunk);
    INVAR (0 == chunk.count, "initChunk did not clear the count");
    INVAR (0 == chunk.capacity, "initChunk did not clear the capacity");
    INVAR (NULL == chunk.code, "initChunk did not null the code pointer");
    INVAR (NULL == chunk.lines, "initChunk did not null the lines pointer");

    for (int i = 0; i < 256; ++i)
        writeChunk (&chunk, 0xAA ^ i, 123 + i);
    writeChunk (&chunk, OP_RETURN, 1337);

    INVAR (257 == chunk.count, "writeChunk did not update the count");
    INVAR (512 == chunk.capacity, "writeChunk did not update the capacity");
    INVAR (NULL != chunk.code, "writeChunk did not update the code pointer");
    INVAR (NULL != chunk.lines,
        "writeChunk did not update the lines pointer");

    for (int i = 0; i < 256; ++i)
        INVAR ((0xAA ^ i) == chunk.code[i],
            "writeChunk did not write the data");
    INVAR (OP_RETURN == chunk.code[256], "writeChunk did not write the data");

    for (int i = 0; i < 256; ++i)
        INVAR ((123 + i) == chunk.lines[i],
            "writeChunk did not write the line");
    INVAR (1337 == chunk.lines[256], "writeChunk did not write the line");

    INVAR (0 == addConstant (&chunk, NUMBER_VAL (1.5)),
        "first constant is at offset zero");
    INVAR (1.5 == AS_NUMBER (chunk.constants.values[0]),
        "verify first constant stored");

    INVAR (1 == addConstant (&chunk, NUMBER_VAL (3.5)),
        "second constant is at offset one");
    INVAR (3.5 == AS_NUMBER (chunk.constants.values[1]),
        "verify second constant stored");

    freeChunk (&chunk);
    INVAR (0 == chunk.count, "freeChunk did not clear the count");
    INVAR (0 == chunk.capacity, "freeChunk did not clear the capacity");
    INVAR (NULL == chunk.code, "freeChunk did not null the code pointer");
}
