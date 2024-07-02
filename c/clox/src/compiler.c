#include "compiler.h"

#include "chunk.h"
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

/** Operator Precedence enumeration
 */
enum precedence_e {
    PREC_NONE,                  ///< "="
    PREC_ASSIGNMENT,            ///< "or"
    PREC_OR,                    ///< "and"
    PREC_AND,                   ///< "=="
    PREC_EQUALITY,              ///< "!="
    PREC_COMPARISON,            ///< "<", ">", "<=", ">="
    PREC_TERM,                  ///< "+", "0"
    PREC_FACTOR,                ///< "*", "/"
    PREC_UNARY,                 ///< "!", "-"
    PREC_CALL,                  ///< ".", "()"
    PREC_PRIMARY,               // identifiers etc.
};

Parser parser;                  ///< Storage for the parser state.
Chunk *compilingChunk;          ///< chunk currently being compiled.

/* Forward Declarations */

static void expression (
    );
static void parsePrecedence (
    Precedence precedence);

/** Return a pointer to the current target chunk */
static Chunk *
currentChunk (
    )
{
    return compilingChunk;
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

/** Emit a byte into the current chunk.
 *
 * @param byte the opcode (or data) to emit.
 */
static void
emitByte (
    uint8_t byte)
{
    writeChunk (currentChunk (), byte, parser.previous.line);
}

/** Emit two bytes into the current chunk.
 *
 * @param byte1 the first opcode (or data) to emit.
 * @param byte2 the second byte of data to emit.
 */
static void
emitBytes (
    uint8_t byte1,
    uint8_t byte2)
{
    emitByte (byte1);
    emitByte (byte2);
}

/** Emit a RETURN opcode to the chunk.
 */
static void
emitReturn (
    )
{
    emitByte (OP_RETURN);
}

/** Add a CONSTANT to the bytecode stream
 *
 * This function inserts the constant into the pool then write the
 * constant pool index into the chunk.
 *
 * @param value the value of the constant
 */
static uint8_t
makeConstant (
    Value value)
{
    int constant = addConstant (currentChunk (), value);

    if (constant > UINT8_MAX) {
        error ("Too many constants in one chunk.");
        return 0;
    }

    return (uint8_t) constant;
}

/** Construct a CONSTANT operation in the chunk.
 *
 * Emits OP_CONSTANT, then an immediate byte picking
 * the constant out of the constant pool.
 *
 * @param value
 */
static void
emitConstant (
    Value value)
{
    emitBytes (OP_CONSTANT, makeConstant (value));
}

/** Shut down the compiler.
 */
static void
endCompiler (
    )
{
    emitReturn ();
}

/** Compile a binary operation to the chunk.
 */
static void
binary (
    )
{
    TokenType operatorType = parser.previous.type;
    ParseRule *rule = getRule (operatorType);

    parsePrecedence ((Precedence) (rule->precedence + 1));

    switch (operatorType) {
        // *INDENT-OFF*
    case TOKEN_PLUS:            emitByte(OP_ADD);         break;
    case TOKEN_MINUS:           emitByte(OP_SUBTRACT);    break;
    case TOKEN_STAR:            emitByte(OP_MULTIPLY);    break;
    case TOKEN_SLASH:           emitByte(OP_DIVIDE);      break;

    default: ERROR_LOG (0, "Should be UNREACHABLE.", 0);  return;
        // *INDENT-ON*
    }
}

/** Compile a Grouping to the chunk.
 */
static void
grouping (
    )
{
    expression ();
    consume (TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

/** Compile a number to the chunk.
 */
static void
number (
    )
{
    double value = strtod (parser.previous.start, NULL);

    emitConstant (value);
}

/** Compile a unary operation to the chunk.
 */
static void
unary (
    )
{
    TokenType operatorType = parser.previous.type;

    // Compile the operand.
    parsePrecedence (PREC_UNARY);

    // Emit the operator instruction.
    switch (operatorType) {
    case TOKEN_MINUS:
        emitByte (OP_NEGATE);
        break;
    default:
        ERROR_LOG (0, "Should be UNREACHABLE.", 0);
        return;
    }
}

/** Compile an expression at the specified precedence.
 */
static void
parsePrecedence (
    Precedence precedence)
{
    // What goes here?
    (void) precedence;
    STUB (0);
}

/** Compile an expression to the chunk.
 */
static void
expression (
    )
{
    parsePrecedence (PREC_ASSIGNMENT);
}

/** Compile the source code into the chunk.
 */
bool
compile (
    const char *source,
    Chunk *chunk)
{
    initScanner (source);
    compilingChunk = chunk;
    parser.hadError = false;
    parser.panicMode = false;

    advance ();
    expression ();
    consume (TOKEN_EOF, "Expect end of expression.");
    endCompiler ();
    return !parser.hadError;
}

void
call_unused_compiler_functions (
    // TODO remove this function
    )
{
    error (0);                  // todo remove this line
    number ();                  // todo remove this line
    grouping ();                // todo remove this line
    unary ();                   // todo remove this line
    binary ();                  // todo remove this line
    parsePrecedence (0);        // todo remove this line
    STUB (0);
}
