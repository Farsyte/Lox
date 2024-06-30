#include "compiler.h"

#include "common.h"
#include "debug.h"
#include "scanner.h"

#include <stdio.h>

void
compile (
    const char *source)
{
    initScanner (source);

    int line = -1;

    for (;;) {
        Token token = scanToken ();

        if (token.line != line) {
            printf ("%4d ", token.line);
            line = token.line;
        } else {
            printf ("   | ");
        }
        printf ("%2d '%.*s'%.*s[%s]\n",
            token.type, token.length, token.start,
            16 - token.length, "                ",
            tokenTypeString (token.type));

        if (token.type == TOKEN_EOF)
            break;
    }
}
