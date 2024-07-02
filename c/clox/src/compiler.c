#include "compiler.h"

#include "common.h"
#include "debug.h"
#include "scanner.h"

#include <stdio.h>
#include <stdlib.h>

/** @file compiler.c
 * @brief Compiler (Lox Parser)
 */

/** Parser state */
struct parser_s {
    Token current;              ///< the current token to parse
    Token previous;             ///< the previous token parsed
    bool hadError;              ///< set if a parse error occurs
    bool panicMode;             ///< if set, move forward silently.
};

Parser parser;                  ///< Storage for the parser state.

static void
expression (
    )
{
    // TODO actually construct expression()
    ERROR_LOG (0, "not yet implemented!", 0);
}

/** Report error at this token.
 *
 * If we are already in panicMode, does nothing.
 * Otherwise, sets panicMode and hadError, and
 * reports the error in a useful form.
 *
 * @param token the token to indicate
 * @param message something to print
 */
static void
errorAt (
    Token *token,
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

/** Report error at the previous token.
 *
 * Applies errorAt() with parser.previous
 * as the token with the error.
 *
 * @param message something to print
 */
static void
error (
    const char *message)
{
    errorAt (&parser.previous, message);
}

/** Report error at the current token.
 *
 * Applies errorAt() with parser.current
 * as the token with the error.
 *
 * @param message something to print
 */
static void
errorAtCurrent (
    const char *message)
{
    errorAt (&parser.current, message);
}

/** Advance to the next token.
 *
 * Saves the current token as the previous token,
 * then scans a token forward. If the token is
 * TOKEN_ERROR, report the error, and scan again;
 * stop when a non-ERROR token is found.
 */
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

/** Consume the current token, which must be the given type.
 *
 * If the current token is the right type, consume it, bringing
 * the next token in as current. Otherwise, report the error.
 */
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

/** Compile the source code into the chunk.
 */
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
