#include "scanner.h"

#include "assert.h"
#include "bist.h"

void
bist_scanner (
    )
{
    Token t;

    initScanner ("");

    t = scanToken ();
    assert (TOKEN_EOF == t.type,
        "scanToken should return TOKEN_EOF for the empty string");

    // TODO assert more things

    t = scanToken ();
    assert (TOKEN_EOF == t.type, "scanToken should keep returning TOKEN_EOF");

    // TODO assert more things
}
