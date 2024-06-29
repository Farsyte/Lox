#include "main.h"

#include "bist.h"
#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "post.h"
#include "vm.h"

#include <stdio.h>
#include <string.h>

/** @file main.c
 * @brief Implementation of Main Function
 *
 * This file provides the main entry point as defined by the C
 * programming langage, and contains the logic driving cLox based on
 * the command line arguments presented.
 */

/** Main Entry Point.
 *
 * Run POST and BIST.
 *
 * TEMPORARILY: tinker with a Chunk.
 *
 * \todo command line processing
 *
 * @param argc command line arg count
 * @param argv command line arg list
 */
int
main (
    int argc,
    const char **argv)
{
    postAll ();

    Chunk chunk;

    initChunk (&chunk);
    int constant = addConstant (&chunk, 1.2);

    writeChunk (&chunk, OP_CONSTANT, 123);
    writeChunk (&chunk, constant, 123);

    writeChunk (&chunk, OP_RETURN, 123);
    disassembleChunk (&chunk, "test chunk");

    initVM ();
    interpret (&chunk);
    freeVM ();

    freeChunk (&chunk);

    bistAll ();

    (void) argc;
    (void) argv;

    STUB ("parameter processing");

    return 0;
}
