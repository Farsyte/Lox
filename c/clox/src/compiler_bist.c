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
    // *INDENT-OFF*
    const char source[] =
        "() {}\t; ,.-+*/\t\t// comment to eol\n"
        "= ! < >\t/* line-spanning\n"
        "comment */\t!= <= >= ==\n";
    // *INDENT-ON*

    const size_t src_len = sizeof source - 1;
    const char *source_end = source + src_len;

    compile (source);

    INVAR (source_end == scanner.start,
        "initScaner must set scanner.start to the given source.");
    INVAR (source_end == scanner.current,
        "initScaner must set scanner.current to the given source.");
    INVAR (4 == scanner.line, "initScaner must set scanner.line to 1.");
}
