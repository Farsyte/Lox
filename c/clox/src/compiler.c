#include "compiler.h"

#include "common.h"

static void
initScanner (
    const char *source)
{
    (void) source;
    STUB (0);
}

void
compile (
    const char *source)
{
    initScanner (source);
}
