#include "scanner.h"

#include "assert.h"

Scanner scanner;

void
initScanner (
    const char *source)
{
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

Token
scanToken (
    )
{
    STUB ();
}
