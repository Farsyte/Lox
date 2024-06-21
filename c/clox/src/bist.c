#include "bist.h"

#include "assert.h"
#include "chunk.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
bist_all (
    int argc,
    const char *argv[])
{
    int exit_code = 0;

    if (argc < 2)
      {
          // add tests here

          exit_code |= bist_chunk ();
      }
    else
      {

          (void) argv;          // TODO parse parameters

          for (int argi = 2; argi < argc; ++argi)
            {
                const char *opt = argv[argi];

                if (!strcmp (opt, "chunk"))
                  {
                      exit_code |= bist_chunk ();
                  }

                // add calls to bist_foo here

                fprintf (stderr, "BIST: unrecognized option '%s'\n", opt);
                exit_code = 1;
            }

      }
    return (exit_code);
}
