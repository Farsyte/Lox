#include "compiler.h"

#include "common.h"
#include "debug.h"
#include "scanner.h"

#include <stdio.h>
#include <stdlib.h>

struct parser_s {
    Token current;
    Token previous;
    bool hadError;
    bool panicMode;
};

Parser parser;

static void
expression (
    )
{
    // TODO actually construct expression()
    ERROR_LOG (0, "not yet implemented!", 0);
}

static void
errorAt (
    Token * token,
    const char *message)
{
    if (parser.panicMode)
        return;
    parser.panicMode = true;
    fprintf (stderr, "[line %d] Error", token->line);

    if (token->type == TOKEN_EOF) {
        fprintf (stderr, " at end");
    } else if (token->type == TOKEN_ERROR) {
        // Nothing.
    } else {
        fprintf (stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf (stderr, ": %s\n", message);
    parser.hadError = true;
}

static void
error (
    const char *message)
{
    errorAt (&parser.previous, message);
}

static void
errorAtCurrent (
    const char *message)
{
    errorAt (&parser.current, message);
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

static void
consume (
    TokenType type,
    const char *message)
{
    if (parser.current.type == type) {
        advance ();
        return;
    }

    errorAtCurrent (message);
}

bool
compile (
    const char *source,
    Chunk *chunk)
{
    (void) chunk;               // not used (yet)
    initScanner (source);
    parser.hadError = false;
    parser.panicMode = false;

    advance ();
    expression ();
    consume (TOKEN_EOF, "Expect end of expression.");
    return !parser.hadError;
}

void
call_unused_compiler_functions (
    // TODO remove this function
    )
{
    error (0);
    STUB (0);
}
