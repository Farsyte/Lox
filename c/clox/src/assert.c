#include "assert.h"

#include <stdio.h>

int
_assert (
    const char *file,
    int line,
    const char *func,
    const char *cond,
    const char *msg)
{
    fprintf (stderr, "%s:%d: assertion failed", file, line);
    if (func)
        fprintf (stderr, " in %s\n", func);
    if (cond)
        fprintf (stderr, "    %s\n", cond);
    if (cond)
        fprintf (stderr, "%s\n", msg);
    return 0;
}
