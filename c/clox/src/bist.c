#include "bist.h"

#include "assert.h"
#include "chunk.h"

#include <stdio.h>              /* fprintf, stderr */
#include <string.h>             /* strcmp */

int
bist_all (
    int argc,
    const char *argv[])
{
    int exit_code = BIST_PASS;

    if (argc < 3) {

        exit_code |= bist_chunk ();

        // add tests here

    } else {

        for (int argi = 2; argi < argc; ++argi) {
            const char *opt = argv[argi];

            if (!strcmp (opt, "chunk"))
                exit_code |= bist_chunk ();

            // add calls to bist_foo here

            else {
                fprintf (stderr, "BIST: unrecognized option '%s'\n", opt);
                exit_code = BIST_FAIL;
            }
        }

    }
    return (exit_code);
}
