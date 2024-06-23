#include "assert.h"
#include "compiler.h"

static void initScanner (
    const char *source);

void
compile (
    const char *source)
{
    initScanner (source);
}

static void
initScanner (
    const char *source)
{
    (void) source;              // not implemented yet
    STUB ();
}
