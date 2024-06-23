#include "compiler.h"

#include "assert.h"
#include "scanner.h"

void
compile (
    const char *source)
{
    initScanner (source);
}
