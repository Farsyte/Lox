#include "bist.h"

#include "chunk.h"
#include "memory.h"
#include "value.h"

/** @file bist.c
 * @brief Built-In Self Test framework
 *
 * This file provides BIST related code that is
 * not specific to one module.
 */

/** Run all Built-In Self Tests.
 *
 * Run all of the BIST methods in a hard-coded order.
 * When all tests are complete, terminate the program.
 */
void
bistAll (
    )
{
    bistMemory ();
    bistChunk ();
    bistValue ();
    exit (0);
}
