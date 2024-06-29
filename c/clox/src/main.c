#include "main.h"

#include "bist.h"
#include "common.h"
#include "post.h"

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
 * \todo command line processing
 *
 * \param argc command line arg count
 * \param argv command line arg list
 */
int
main (
    int argc,
    const char **argv)
{
    postAll ();
    bistAll ();

    (void) argc;
    (void) argv;

    STUB ("parameter processing");

    return 0;
}
