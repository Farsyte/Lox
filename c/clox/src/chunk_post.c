#include "chunk.h"

#include "error_log.h"

#include <stdio.h>

/** @file chunk_post.c
 * @brief Built-In Self Test for the CHUNK module.
 */

/** Run all POST cases for Chunk.
 */
void
postChunk (
    )
{
    INVAR (1 == sizeof (OpCode), "OpCode size must be one byte.");
}
