#include "compiler.h"

#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "scanner.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** @file compiler.c
 * @brief Compiler (Lox Parser)
 */

/** Parser state */
struct Parser {
    Token current;              ///< the current token to parse
    Token previous;             ///< the previous token parsed
    bool hadError;              ///< set if a parse error occurs
    bool panicMode;             ///< if set, move forward silently.
};

/** Operator Precedence enumeration
 */
typedef enum {
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
} Precedence;

/** Parse Rules
 */
struct ParseRule {
    ParseFn prefix;             ///< call when token seen as unary operator
    ParseFn infix;              ///< call when token seen as binary operator
    Precedence precedence;      ///< operator precedence of binary operator
};

/** Local Variable
 */
struct Local {
    Token name;                 ///< name of the local variable
    int depth;                  ///< scope depth of block defining it
};

/** Compiler State
 */
struct Compiler {
    Local locals[UINT8_COUNT];  ///< storage for local variables
    int localCount;             ///< number of local variables in scope
    int scopeDepth;             ///< number of blocks surrounding current code
};

Parser parser;                  ///< Storage for the parser state.
Compiler *current = NULL;       ///< the current compiler state
Chunk *compilingChunk;          ///< chunk currently being compiled.

/* Forward Declarations */

static void expression ();
static void statement ();
static void declaration ();
static void synchronize ();
static ParseRule *getRule (TokenType type);
static void parsePrecedence (Precedence precedence);

/** Return a pointer to the current target chunk */
static Chunk *
currentChunk ()
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
errorAt (Token *token, const char *message)
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
error (const char *message)
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
errorAtCurrent (const char *message)
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
advance ()
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
consume (TokenType type, const char *message)
{
    if (parser.current.type == type) {
        advance ();
        return;
    }

    errorAtCurrent (message);
}

/** Check if the next token has this type.
 *
 * @param type desired token type
 * @returns true if the types match
 * @returns false if they do not match
 */
static bool
check (TokenType type)
{
    return parser.current.type == type;
}

/** Try to match the next token.
 *
 * If the next token matches, consume it and return true;
 * otherwise, return false.
 *
 * @param type which token type is wanted
 * @returns true if token is matched and consumed
 * @returns false if the next token does not match
 */
static bool
match (TokenType type)
{
    if (!check (type))
        return false;
    advance ();
    return true;
}

/** Emit a byte into the current chunk.
 *
 * @param byte the opcode (or data) to emit.
 */
static void
emitByte (uint8_t byte)
{
    writeChunk (currentChunk (), byte, parser.previous.line);
}

/** Emit two bytes into the current chunk.
 *
 * @param byte1 the first opcode (or data) to emit.
 * @param byte2 the second byte of data to emit.
 */
static void
emitBytes (uint8_t byte1, uint8_t byte2)
{
    emitByte (byte1);
    emitByte (byte2);
}

/** Emit a RETURN opcode to the chunk.
 */
static void
emitReturn ()
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
makeConstant (Value value)
{
    int constant = addConstant (currentChunk (), value);

    if (constant > UINT8_MAX) {
        error ("Too many constants in one chunk.");
        return 0;
    }

    return (uint8_t) constant;
}

/** Make an identifier constant
 *
 * Not sure if the word "constant" is now inaccurate.
 *
 * @param name the token containing the name
 * @returns the index into the table for the variable
 */
static uint8_t
identifierConstant (Token *name)
{
    return makeConstant (OBJ_VAL (copyString (name->start, name->length)));
}

/** See if two identifiers are the same.
 *
 * @param a 1st identifier to compare
 * @param b 2nd identifier to compare
 * @returns true if they are textually equal
 * @returns false if they are distinct
 */
static bool
identifiersEqual (Token *a, Token *b)
{
    if (a->length != b->length)
        return false;
    return 0 == memcmp (a->start, b->start, a->length);
}

/** Resolve a reference to a local variable.
 *
 * @param compiler the current compiler state
 * @param name the identifier lexeme
 * @returns -1 if the identifier was not found
 * @returns the index of the match in the compiler locals
 */
static int
resolveLocal (Compiler *compiler, Token *name)
{
    for (int i = compiler->localCount - 1; i >= 0; i--) {
        Local *local = &compiler->locals[i];

        if (identifiersEqual (name, &local->name)) {
            return i;
        }
    }
    return -1;
}

/** Add a local variable to the current scope.
 */
static void
addLocal (Token name)
{
    if (current->localCount == UINT8_COUNT) {
        error ("Too many local variables in function.");
        return;
    }
    Local *local = &current->locals[current->localCount++];

    local->name = name;
    local->depth = current->scopeDepth;
}

/** Compile a local variable declaration.
 */
static void
declareVariable ()
{
    if (current->scopeDepth == 0)
        return;
    Token *name = &parser.previous;

    for (int i = current->localCount - 1; i >= 0; i--) {
        Local *local = &current->locals[i];

        if (local->depth != -1 && local->depth < current->scopeDepth) {
            break;
        }

        if (identifiersEqual (name, &local->name)) {
            error ("Already a variable with this name in this scope.");
        }
    }

    addLocal (*name);
}

/** Construct a CONSTANT operation in the chunk.
 *
 * Emits OP_CONSTANT, then an immediate byte picking
 * the constant out of the constant pool.
 *
 * @param value
 */
static void
emitConstant (Value value)
{
    emitBytes (OP_CONSTANT, makeConstant (value));
}

/** Initialize the state of the compiler
 */
static void
initCompiler (Compiler *compiler)
{
    compiler->localCount = 0;
    compiler->scopeDepth = 0;
    current = compiler;
}

/** Shut down the compiler.
 */
static void
endCompiler ()
{
    emitReturn ();
#ifdef DEBUG_PRINT_CODE
    if (!parser.hadError) {
        disassembleChunk (currentChunk (), "code");
    }
#endif
}

static void
beginScope ()
{
    current->scopeDepth++;
}

static void
endScope ()
{
    current->scopeDepth--;
    while (current->localCount > 0 && current->locals[current->localCount - 1].depth > current->scopeDepth) {
        emitByte (OP_POP);
        current->localCount--;
    }

}

/** Compile a binary operation to the chunk.
 */
static void
binary (bool canAssign)
{
    (void) canAssign;                   // not used

    TokenType operatorType = parser.previous.type;
    ParseRule *rule = getRule (operatorType);

    parsePrecedence ((Precedence) (rule->precedence + 1));

    switch (operatorType) {
        // *INDENT-OFF*

    case TOKEN_BANG_EQUAL:     emitBytes(OP_EQUAL, OP_NOT);    break;
    case TOKEN_EQUAL_EQUAL:    emitByte(OP_EQUAL);             break;
    case TOKEN_GREATER:        emitByte(OP_GREATER);           break;
    case TOKEN_GREATER_EQUAL:  emitBytes(OP_LESS, OP_NOT);     break;
    case TOKEN_LESS:           emitByte(OP_LESS);              break;
    case TOKEN_LESS_EQUAL:     emitBytes(OP_GREATER, OP_NOT);  break;

    case TOKEN_PLUS:           emitByte(OP_ADD);               break;
    case TOKEN_MINUS:          emitByte(OP_SUBTRACT);          break;
    case TOKEN_STAR:           emitByte(OP_MULTIPLY);          break;
    case TOKEN_SLASH:          emitByte(OP_DIVIDE);            break;

    default: ERROR_LOG (0, "Should be UNREACHABLE.", 0);  return;
        // *INDENT-ON*
    }
}

/** Compile a Literal op to the chunk.
 */
static void
literal (bool canAssign)
{
    (void) canAssign;                   // not used
    switch (parser.previous.type) {
        // *INDENT-OFF*
    case TOKEN_FALSE:   emitByte(OP_FALSE);     break;
    case TOKEN_NIL:     emitByte(OP_NIL);       break;
    case TOKEN_TRUE:    emitByte(OP_TRUE);      break;

    default: ERROR_LOG (0, "Should be UNREACHABLE.", 0);  return;
        // *INDENT-ON*
    }
}

/** Compile a Grouping to the chunk.
 */
static void
grouping (bool canAssign)
{
    (void) canAssign;                   // not used
    expression ();
    consume (TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

/** Compile a number to the chunk.
 */
static void
number (bool canAssign)
{
    (void) canAssign;                   // not used
    double value = strtod (parser.previous.start, NULL);

    emitConstant (NUMBER_VAL (value));
}

/** Compile a string to the chunk.
 */
static void
string (bool canAssign)
{
    (void) canAssign;                   // not used
    emitConstant (OBJ_VAL (copyString (parser.previous.start + 1, parser.previous.length - 2)));
}

/** Do the work of putting a variable into the chunk.
 *
 * @param name the token containing the variable namep
 */
static void
namedVariable (Token name, bool canAssign)
{
    uint8_t getOp, setOp;
    int arg = resolveLocal (current, &name);

    if (arg != -1) {
        getOp = OP_GET_LOCAL;
        setOp = OP_SET_LOCAL;
    } else {
        arg = identifierConstant (&name);
        getOp = OP_GET_GLOBAL;
        setOp = OP_SET_GLOBAL;
    }

    if (canAssign && match (TOKEN_EQUAL)) {
        expression ();
        emitBytes (setOp, (uint8_t) arg);
    } else {
        emitBytes (getOp, (uint8_t) arg);
    }
}

/** Compile a variable to the chunk.
 */
static void
variable (bool canAssign)
{
    namedVariable (parser.previous, canAssign);
}

/** Compile a unary operation to the chunk.
 */
static void
unary (bool canAssign)
{
    (void) canAssign;                   // not used

    TokenType operatorType = parser.previous.type;

    // Compile the operand.
    parsePrecedence (PREC_UNARY);

    // Emit the operator instruction.
    switch (operatorType) {
    case TOKEN_BANG:
        emitByte (OP_NOT);
        break;
    case TOKEN_MINUS:
        emitByte (OP_NEGATE);
        break;
    default:
        ERROR_LOG (0, "Should be UNREACHABLE.", 0);
        return;
    }
}

ParseRule rules[] = {

    // *INDENT-OFF*

    // Single-character tokens.
    [TOKEN_LEFT_PAREN]     =  {  grouping,   NULL,     PREC_NONE        },   //  "("
    [TOKEN_RIGHT_PAREN]    =  {  NULL,       NULL,     PREC_NONE        },   //  ")"
    [TOKEN_LEFT_BRACE]     =  {  NULL,       NULL,     PREC_NONE        },   //  "{"
    [TOKEN_RIGHT_BRACE]    =  {  NULL,       NULL,     PREC_NONE        },   //  "}"
    [TOKEN_COMMA]          =  {  NULL,       NULL,     PREC_NONE        },   //  ", "
    [TOKEN_DOT]            =  {  NULL,       NULL,     PREC_NONE        },   //  "."
    [TOKEN_MINUS]          =  {  unary,      binary,   PREC_TERM        },   //  "-"
    [TOKEN_PLUS]           =  {  NULL,       binary,   PREC_TERM        },   //  "+"
    [TOKEN_SEMICOLON]      =  {  NULL,       NULL,     PREC_NONE        },   //  ";"
    [TOKEN_SLASH]          =  {  NULL,       binary,   PREC_FACTOR      },   //  "/"
    [TOKEN_STAR]           =  {  NULL,       binary,   PREC_FACTOR      },   //  "*"

    // One or two charcter tokens.
    [TOKEN_BANG]           =  {  unary,      NULL,     PREC_NONE        },   //  "!"
    [TOKEN_BANG_EQUAL]     =  {  NULL,       binary,   PREC_EQUALITY    },   //  "!="
    [TOKEN_EQUAL]          =  {  NULL,       NULL,     PREC_NONE        },   //  "="
    [TOKEN_EQUAL_EQUAL]    =  {  NULL,       binary,   PREC_EQUALITY    },   //  "=="
    [TOKEN_GREATER]        =  {  NULL,       binary,   PREC_COMPARISON  },   //  ">"
    [TOKEN_GREATER_EQUAL]  =  {  NULL,       binary,   PREC_COMPARISON  },   //  ">="
    [TOKEN_LESS]           =  {  NULL,       binary,   PREC_COMPARISON  },   //  "<"
    [TOKEN_LESS_EQUAL]     =  {  NULL,       binary,   PREC_COMPARISON  },   //  "<="

    // Literals.
    [TOKEN_IDENTIFIER]     =  {  variable,   NULL,     PREC_NONE        },   //  regex: [A-Za-z_][0-9A-Za-z_]*
    [TOKEN_STRING]         =  {  string,     NULL,     PREC_NONE        },   //  regex: "[^"]*"
    [TOKEN_NUMBER]         =  {  number,     NULL,     PREC_NONE        },   //  regex: [0-9]+ | [0-9]+\\.[0-9]+

    // Keywords
    [TOKEN_AND]            =  {  NULL,       NULL,     PREC_NONE        },   //  "and"
    [TOKEN_CLASS]          =  {  NULL,       NULL,     PREC_NONE        },   //  "class"
    [TOKEN_ELSE]           =  {  NULL,       NULL,     PREC_NONE        },   //  "else"
    [TOKEN_FALSE]          =  {  literal,    NULL,     PREC_NONE        },   //  "false"
    [TOKEN_FOR]            =  {  NULL,       NULL,     PREC_NONE        },   //  "for"
    [TOKEN_FUN]            =  {  NULL,       NULL,     PREC_NONE        },   //  "fun"
    [TOKEN_IF]             =  {  NULL,       NULL,     PREC_NONE        },   //  "if"
    [TOKEN_NIL]            =  {  literal,    NULL,     PREC_NONE        },   //  "nil"
    [TOKEN_OR]             =  {  NULL,       NULL,     PREC_NONE        },   //  "or"
    [TOKEN_PRINT]          =  {  NULL,       NULL,     PREC_NONE        },   //  "print"
    [TOKEN_RETURN]         =  {  NULL,       NULL,     PREC_NONE        },   //  "return"
    [TOKEN_SUPER]          =  {  NULL,       NULL,     PREC_NONE        },   //  "super"
    [TOKEN_THIS]           =  {  NULL,       NULL,     PREC_NONE        },   //  "this"
    [TOKEN_TRUE]           =  {  literal,    NULL,     PREC_NONE        },   //  "true"
    [TOKEN_VAR]            =  {  NULL,       NULL,     PREC_NONE        },   //  "var"
    [TOKEN_WHILE]          =  {  NULL,       NULL,     PREC_NONE        },   //  "while"

    [TOKEN_ERROR]          =  {  NULL,       NULL,     PREC_NONE        },   //  not a valid token
    [TOKEN_EOF]            =  {  NULL,       NULL,     PREC_NONE        },   //  end of the file

    // *INDENT-ON*

};

/** Compile an expression at the specified precedence.
 */
static void
parsePrecedence (Precedence precedence)
{
    advance ();
    ParseFn prefixRule = getRule (parser.previous.type)->prefix;

    if (prefixRule == NULL) {
        error ("Expect expression.");
        return;
    }
    bool canAssign = precedence <= PREC_ASSIGNMENT;

    prefixRule (canAssign);
    while (precedence <= getRule (parser.current.type)->precedence) {
        advance ();
        ParseFn infixRule = getRule (parser.previous.type)->infix;

        infixRule (canAssign);
    }
    if (canAssign && match (TOKEN_EQUAL)) {
        error ("Invalid assignment target.");
    }
}

static uint8_t
parseVariable (const char *errorMessage)
{
    consume (TOKEN_IDENTIFIER, errorMessage);
    declareVariable ();
    if (current->scopeDepth > 0)
        return 0;
    return identifierConstant (&parser.previous);
}

static void
defineVariable (uint8_t global)
{
    if (current->scopeDepth > 0) {
        return;
    }
    emitBytes (OP_DEFINE_GLOBAL, global);
}

static ParseRule *
getRule (TokenType type)
{
    return &rules[type];
}

/** Compile an expression to the chunk.
 */
static void
expression ()
{
    parsePrecedence (PREC_ASSIGNMENT);
}

static void
block ()
{
    while (!check (TOKEN_RIGHT_BRACE) && !check (TOKEN_EOF)) {
        declaration ();
    }
    consume (TOKEN_RIGHT_BRACE, "Expect '}' after block.");
}

static void
varDeclaration ()
{
    uint8_t global = parseVariable ("Expect variable name.");

    if (match (TOKEN_EQUAL)) {
        expression ();
    } else {
        emitByte (OP_NIL);
    }
    consume (TOKEN_SEMICOLON, "Expect ';' after variable declaration.");
    defineVariable (global);
}

/** Compile an expression statement to the chunk.
 */
static void
expressionStatement ()
{
    expression ();
    consume (TOKEN_SEMICOLON, "Expect ';' after expression.");
    emitByte (OP_POP);
}

static void
declaration ()
{
    if (match (TOKEN_VAR)) {
        varDeclaration ();
    } else {
        statement ();
    }

    if (parser.panicMode)
        synchronize ();
}

static void
printStatement ()
{
    expression ();
    consume (TOKEN_SEMICOLON, "Expect ';' after value.");
    emitByte (OP_PRINT);
}

static void
synchronize ()
{
    parser.panicMode = false;

    while (parser.current.type != TOKEN_EOF) {
        if (parser.previous.type == TOKEN_SEMICOLON)
            return;

        switch (parser.current.type) {

            // *INDENT_OFF*

        case TOKEN_CLASS:
        case TOKEN_FUN:
        case TOKEN_VAR:
        case TOKEN_FOR:
        case TOKEN_IF:
        case TOKEN_WHILE:
        case TOKEN_PRINT:
        case TOKEN_RETURN:
            return;

        default:
            break;

            // *INDENT_ON*
        }

        advance ();
    }
}

/** Compile a statement to the chunk.
 */
static void
statement ()
{
    if (match (TOKEN_PRINT)) {
        printStatement ();
    } else if (match (TOKEN_LEFT_BRACE)) {
        beginScope ();
        block ();
        endScope ();
    } else {
        expressionStatement ();
    }
}

/** Compile the source code into the chunk.
 */
bool
compile (const char *source, Chunk *chunk)
{
    initScanner (source);
    Compiler compiler;

    initCompiler (&compiler);
    compilingChunk = chunk;
    parser.hadError = false;
    parser.panicMode = false;

    advance ();
    while (!match (TOKEN_EOF)) {
        declaration ();
    }

    endCompiler ();
    return !parser.hadError;
}
