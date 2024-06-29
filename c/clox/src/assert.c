#include "assert.h"

#include <stdio.h>

int
_build_msg (
    const char *file,
    int line,
    const char *func,
    const char *pfx,
    const char *cond,
    const char *msg)
{
    printf ("\n");
    fflush (stdout);

    if (file && line)
        fprintf (stderr, "%s:%d: ", file, line);
    if (pfx)
        fprintf (stderr, "%s ", pfx);
    if (func)
        fprintf (stderr, "in %s\n", func);
    if (cond)
        fprintf (stderr, "    %s\n", cond);
    if (msg)
        fprintf (stderr, "%s\n", msg);
    fprintf (stderr, "\n");
    return 0;
}
