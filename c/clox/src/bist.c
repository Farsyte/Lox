#include "bist.h"

#include "assert.h"
#include "chunk.h"

#include <stdio.h>
#include <stdlib.h>

int
bist_all (
    int argc,
    const char *argv[])
{
    int exit_code = 0;

    if (argc < 2)
      {
          // add tests here

      }
    else
      {

          (void) argv;          // TODO parse parameters

          for (int argi = 2; argi < argc; ++argi)
            {
                const char *opt = argv[argi];

                // add calls to bist_foo here

                fprintf (stderr, "BIST: unrecognized option '%s'\n", opt);
                exit_code = 1;
            }

      }
    return (exit_code);
}
