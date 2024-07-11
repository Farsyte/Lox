#include "bist.h"

#include "chunk.h"
#include "compiler.h"
#include "memory.h"
#include "object.h"
#include "scanner.h"
#include "table.h"
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
bistAll ()
{
    bistMemory ();
    bistChunk ();
    bistValue ();
    bistTable ();
    bistObject ();
    bistScanner ();
    bistCompiler ();
    bistVM ();

    freeVM ();
    exit (EX_OK);
}
