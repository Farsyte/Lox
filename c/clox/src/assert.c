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
    fprintf (stderr, "%s:%d:", file, line);
    if (pfx)
        fprintf (stderr, " %s", pfx);
    if (func)
        fprintf (stderr, " in %s\n", func);
    if (cond)
        fprintf (stderr, "    %s\n", cond);
    if (cond)
        fprintf (stderr, "%s\n", msg);
    return 0;
}
