#include "compiler.h"

#include "common.h"
#include "debug.h"
#include "scanner.h"

#include <stdio.h>

struct parser_s {
    Token current;
    Token previous;
};

Parser parser;

static void
errorAtCurrent (
    const char *source)
{
    (void) source;
    // TODO actually construct expression()
    ERROR_LOG (0, "not yet implemented!", 0);
}

static void
expression (
    )
{
    // TODO actually construct expression()
    ERROR_LOG (0, "not yet implemented!", 0);
}

static void
consume (
    TokenType tt,
    const char *msg)
{
    (void) tt;
    (void) msg;
    // TODO actually construct consume()
    ERROR_LOG (0, "not yet implemented!", 0);
}

static void
advance (
    )
{
    parser.previous = parser.current;

    for (;;) {
        parser.current = scanToken ();
        if (parser.current.type != TOKEN_ERROR)
            break;
        errorAtCurrent (parser.current.start);
    }
}

bool
compile (
    const char *source,
    Chunk *chunk)
{
    (void) chunk;               // not used (yet)
    initScanner (source);
    advance ();
    expression ();
    consume (TOKEN_EOF, "Expect end of expression.");
    STUB ("update compile() to use chunk");
}
