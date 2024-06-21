#include "main.h"

#include "bist.h"
#include "common.h"
#include "post.h"

#include <stdio.h>
#include <string.h>

int
main (
    int argc,
    const char *argv[])
{
    (void) argc;                // intentionally unused
    (void) argv;                // intentionally unused

    if (post_all ())
      {
          fprintf (stderr, "\nPOST failed, run aborted.\n");
          return 1;
      }

    if ((argc > 1) && (!strcmp (argv[1], "bist")))
      {
          fprintf (stderr, "\nrunning BIST.\n");
          if (bist_all (argc, argv))
            {
                fprintf (stderr, "\nBIST failed.\n");
                return 1;
            }
          fprintf (stderr, "\nBIST passed.\n");
          return 0;
      }

    return 0;
}
