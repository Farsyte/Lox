#include "chunk.h"

#include <stdio.h>

/** @file chunk_post.c
 * @brief Built-In Self Test for the CHUNK module.
 */

/** Run all POST cases for Chunk.
 *
 * - OpCode values must fit in a byte.
 * - Managed storage must be array of bytes.
 */
void
postChunk ()
{
    Chunk *c;

    INVAR (OP__LAST < 256, "OpCode values do not fit in one byte.");

    INVAR (1 == sizeof (c->code[0]), "Chunk data must be array of bytes.");

}
