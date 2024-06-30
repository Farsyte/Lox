#include "bist.h"

#include "chunk.h"
#include "memory.h"
#include "value.h"
#include "vm.h"

/** @file bist.c
 * @brief Built-In Self Test framework
 *
 * This file provides BIST related code that is
 * not specific to one module.
 */

/** Run all Built-In Self Tests.
 *
 * Run all of the BIST methods in a hard-coded order.
 *
 * Tests use INVAR to check the conditions, so a failed
 * test is an immediate error termination of the program.
 * If all tests pass
 */
void
bistAll (
    )
{
    bistMemory ();
    bistChunk ();
    bistValue ();
    bistVM ();

    exit (EX_OK);
}
