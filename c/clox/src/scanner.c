#include "scanner.h"

#include "common.h"

// Does any code outside scanner.c have any reason
// to look inside the Scanner structure?

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
    STUB (0);
}
