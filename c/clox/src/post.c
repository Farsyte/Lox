#include "post.h"

#include "chunk.h"
#include "memory.h"
#include "value.h"

/** @file post.c
 * @brief Power-On Self Test Framework
 *
 * This file provides POST related code that is
 * not specific to one module.
 */

/** Run all Power-On Self Tests.
 *
 * Run all of the POST methods in a hard-coded order.
 * When all tests are complete, allow the program to continue.
 */
void
postAll (
    )
{
    postMemory ();
    postChunk ();
    postValue ();
}
