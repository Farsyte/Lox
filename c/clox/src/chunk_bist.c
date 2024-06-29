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
    Chunk c[1];

    initChunk (c);
    INVAR (0 == c->count, "initChunk did not clear the count");
    INVAR (0 == c->capacity, "initChunk did not clear the capacity");
    INVAR (NULL == c->code, "initChunk did not null the code pointer");

    for (int i = 0; i < 256; ++i)
        writeChunk (c, 0xAA ^ i);
    writeChunk (c, OP_RETURN);

    INVAR (257 == c->count, "writeChunk did not update the count");
    INVAR (512 == c->capacity, "writeChunk did not update the capacity");
    INVAR (NULL != c->code, "writeChunk did not update the code pointer");

    for (int i = 0; i < 256; ++i)
        INVAR ((0xAA ^ i) == c->code[i], "writeChunk did not write the data");
    INVAR (OP_RETURN == c->code[256], "writeChunk did not write the data");

    freeChunk (c);
    INVAR (0 == c->count, "freeChunk did not clear the count");
    INVAR (0 == c->capacity, "freeChunk did not clear the capacity");
    INVAR (NULL == c->code, "freeChunk did not null the code pointer");
}
