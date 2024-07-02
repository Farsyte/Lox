#include "compiler.h"

#include "chunk.h"
#include "common.h"
#include "scanner.h"

extern Scanner scanner;         // peek at scanner state

/** @file compiler_bist.c
 * @brief Built-In Self Test for the COMPILER module.
 */

/** Run all BIST cases for Compiler.
 */
void
bistCompiler ()
{
    // *INDENT-OFF*
    const char source[] =
        "!(5 - 4 > 3 * 2 == !nil)"
        ;
    // *INDENT-ON*

    const size_t src_len = sizeof source - 1;
    const char *source_end = source + src_len;

    int ending_line = 1;

    for (const char *p = source; p < source_end; ++p)
        if ('\n' == *p)
            ending_line++;

    Chunk chunk;

    initChunk (&chunk);

    compile (source, &chunk);

    INVAR (source_end == scanner.start, "initScaner must set scanner.start to the given source.");
    INVAR (source_end == scanner.current, "initScaner must set scanner.current to the given source.");
    INVAR (ending_line == scanner.line, "initScaner must set scanner.line to 1.");

    freeChunk (&chunk);
}
