#include "compiler.h"

#include "common.h"
#include "scanner.h"

/** @file compiler_bist.c
 * @brief Built-In Self Test for the COMPILER module.
 */

/** Run all BIST cases for Compiler.
 */
void
bistCompiler (
    )
{
    const char *source = "";

    compile (source);

    INVAR (source == scanner.start,
        "initScaner must set scanner.start to the given source.");
    INVAR (source == scanner.current,
        "initScaner must set scanner.current to the given source.");
    INVAR (1 == scanner.line, "initScaner must set scanner.line to 1.");
}
