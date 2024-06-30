#include "scanner.h"

/** @file scanner_bist.c
 * @brief Built-In Self Test for the SCANNER module.
 */

/** Run all BIST cases for Scanner.
 */
void
bistScanner (
    )
{
    const char *source = "...";

    initScanner (source);
    INVAR (source == scanner.start,
        "initScaner must set scanner.start to the given source.");
    INVAR (source == scanner.current,
        "initScaner must set scanner.current to the given source.");
    INVAR (1 == scanner.line, "initScaner must set scanner.line to 1.");
}
