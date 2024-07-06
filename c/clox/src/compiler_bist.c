#include "compiler.h"

#include "chunk.h"
#include "common.h"
#include "scanner.h"

#include <stdio.h>

extern Scanner scanner;         // peek at scanner state

/** @file compiler_bist.c
 * @brief Built-In Self Test for the COMPILER module.
 */

/** Run all BIST cases for Compiler.
 */
void
bistCompiler ()
{
    printf ("BIST: %s ...\n", "bistCompiler");
    // *INDENT-OFF*
    const char source[] =
        "print 1 + 2; // add and print\n"
        "print 3 * 4; // mul and print\n"
        ;
    // *INDENT-ON*

    const size_t src_len = sizeof source - 1;
    const char *source_end = source + src_len;

    int ending_line = 1;

    for (const char *p = source; p < source_end; ++p)
        if ('\n' == *p)
            ending_line++;

    ObjFunction *function = compile (source);

    INVAR (NULL != function, "compile must return a non-null function");
    INVAR (source_end == scanner.start, "compile must set scanner.start to the given source.");
    INVAR (source_end == scanner.current, "compile must set scanner.current to the given source.");
    INVAR (ending_line == scanner.line, "compile must set scanner.line to 1.");

    printf ("BIST: %s ... done.\n", "bistCompiler");
}
