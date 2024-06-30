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
        "comment */\t!= <= >= ==\n"
        "\t\"string\" \"line-spanning\n"
        "string\"\n"
        "123 123.5 123. .5\n"
        "foo f00 _f_ _1_\n"
        "and class else false\n"
        "for fun if nil or\n"
        "print return super this\n"
        "true var while\n"
        ;
    // *INDENT-ON*

    const size_t src_len = sizeof source - 1;
    const char *source_end = source + src_len;

    int ending_line = 1;

    for (const char *p = source; p < source_end; ++p)
        if ('\n' == *p)
            ending_line++;

    compile (source);

    INVAR (source_end == scanner.start,
        "initScaner must set scanner.start to the given source.");
    INVAR (source_end == scanner.current,
        "initScaner must set scanner.current to the given source.");
    INVAR (ending_line == scanner.line,
        "initScaner must set scanner.line to 1.");
}
