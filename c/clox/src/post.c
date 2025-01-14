#include "post.h"

#include "chunk.h"
#include "compiler.h"
#include "memory.h"
#include "object.h"
#include "scanner.h"
#include "table.h"
#include "value.h"
#include "vm.h"

/** @file post.c
 * @brief Power-On Self Test Framework
 *
 * This file provides POST related code that is
 * not specific to one module.
 */

/** Run all Power-On Self Tests.
 *
 * Run all of the POST methods in a hard-coded order.
 */
void
postAll ()
{
    postMemory ();
    postChunk ();
    postValue ();
    postTable ();
    postObject ();
    postScanner ();
    postCompiler ();
    postVM ();
}
