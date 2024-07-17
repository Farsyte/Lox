#include "compiler.h"

#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "memory.h"
#include "scanner.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** @file compiler.c
 * @brief Compiler (Lox Parser)
 */

#ifdef DEBUG_PRINT_CODE
int _DEBUG_PRINT_CODE = 1;
#endif

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
    PREC_PRIMARY,               ///< identifiers etc.
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
    bool isCaptured;            ///< as an Upvalue
};

/** Upvalues
 */
struct Upvalue {
    uint8_t index;              ///< index into locals or upvalues
    bool isLocal;               ///< still resident in locals?
};

/** Enumerated Function Types
 */
typedef enum {
    TYPE_FUNCTION,
    TYPE_SCRIPT
} FunctionType;

/** Compiler State
 */
struct Compiler {
    Compiler *enclosing;        ///< compiler for enclosing scope
    ObjFunction *function;      ///< current function being compiled
    FunctionType type;          ///< type of function being compiled
    Local locals[UINT8_COUNT];  ///< storage for local variables
    int localCount;             ///< number of local variables in scope
    Upvalue upvalues[UINT8_COUNT];      ///< array of upvalues
    int scopeDepth;             ///< number of blocks surrounding current code
};

Parser parser;                  ///< Storage for the parser state.
Compiler *current = NULL;       ///< the current compiler state

/* Forward Declarations */

static void expression ();
static void statement ();
static void declaration ();
static ParseRule *getRule (TokenType type);
static void parsePrecedence (Precedence precedence);

/* Function Definitions */

/** Return a pointer to the current target chunk
 *
 * @returns the current chunk pointer
 */
static Chunk *
currentChunk ()
{
    return &current->function->chunk;
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
 *
 * @param type the enumerated type of the token to consume
 * @param message the error message to use if that token is not found
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

/** Emit a LOOP instruction (branch back)
 *
 * @param loopStart offset of the start of the loop
 */
static void
emitLoop (int loopStart)
{
    emitByte (OP_LOOP);

    int offset = currentChunk ()->count - loopStart + 2;

    if (offset > UINT16_MAX)
        error ("Loop body twoo large.");

    emitByte ((offset >> 8) & 0xff);
    emitByte (offset & 0xff);
}

/** Emit a JUMP instruction into the chunk.
 *
 * @param instruction which OpCode to emit
 * @returns the chunk offset of the two-octet immediate
 */
static int
emitJump (uint8_t instruction)
{
    emitByte (instruction);
    emitByte (0xff);
    emitByte (0xff);
    return currentChunk ()->count - 2;
}

/** Emit a RETURN opcode to the chunk.
 */
static void
emitReturn ()
{
    emitByte (OP_NIL);
    emitByte (OP_RETURN);
}

/** Add a CONSTANT to the bytecode stream
 *
 * This function inserts the constant into the pool then write the
 * constant pool index into the chunk.
 *
 * @param value the value of the constant
 * @returns the resulting offset into the constant pool
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

/** Patch a jump offset.
 *
 * @param offset position of the jump immediate to patch
 */
static void
patchJump (int offset)
{
    // -2 to adjust for the bytecode for the jump offset itself.
    int jump = currentChunk ()->count - offset - 2;

    if (jump > UINT16_MAX) {
        error ("Too much code to jump over.");
    }

    currentChunk ()->code[offset] = (jump >> 8) & 0xFF;
    currentChunk ()->code[offset + 1] = jump & 0xFF;
}

/** Initialize the state of the compiler
 *
 * @param compiler the pointer to the compiler state structure to initialize
 * @param type whether this is a Function or a Script compilation
 */
static void
initCompiler (Compiler *compiler, FunctionType type)
{
    compiler->enclosing = current;
    compiler->function = NULL;
    compiler->type = type;
    compiler->localCount = 0;
    compiler->scopeDepth = 0;
    compiler->function = newFunction ();
    current = compiler;

    if (type != TYPE_SCRIPT) {
        current->function->name = copyString (parser.previous.start, parser.previous.length);
    }

    Local *local = &current->locals[current->localCount++];

    local->depth = 0;
    local->isCaptured = false;
    local->name.start = "";
    local->name.length = 0;
}

/** Shut down the compiler.
 *
 * @returns the Function object created by compilation.
 */
static ObjFunction *
endCompiler ()
{
    emitReturn ();
    ObjFunction *function = current->function;

#ifdef DEBUG_PRINT_CODE
    if (_DEBUG_PRINT_CODE && !parser.hadError) {
        disassembleChunk (currentChunk (), function->name != NULL ? function->name->chars : "<script>");
    }
#endif

    current = current->enclosing;
    return function;
}

/** Enter a new local scope
 */
static void
beginScope ()
{
    current->scopeDepth++;
}

/** Leave the current local scope
 */
static void
endScope ()
{
    current->scopeDepth--;
    while (current->localCount > 0 && current->locals[current->localCount - 1].depth > current->scopeDepth) {
        if (current->locals[current->localCount - 1].isCaptured) {
            emitByte (OP_CLOSE_UPVALUE);
        } else {
            emitByte (OP_POP);
        }
        current->localCount--;
    }
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
            if (local->depth == -1) {
                error ("Can't read local variable in its own initializer.'");
            }
            return i;
        }
    }
    return -1;
}

/** Add an Upvaluie to the compiler state.
 *
 * @param compiler the current compiler state
 * @param index index into the locals
 * @param isLocal true if the upvalue is still in the locals
 * @returns the index into the upvalue array
 */
static int
addUpvalue (Compiler *compiler, uint8_t index, bool isLocal)
{
    int upvalueCount = compiler->function->upvalueCount;

    for (int i = 0; i < upvalueCount; i++) {
        Upvalue *upvalue = &compiler->upvalues[i];

        if (upvalue->index == index && upvalue->isLocal == isLocal) {
            return i;
        }
    }

    if (upvalueCount == UINT8_COUNT) {
        error ("Too many closure variables in function.");
        return 0;
    }

    compiler->upvalues[upvalueCount].isLocal = isLocal;
    compiler->upvalues[upvalueCount].index = index;
    return compiler->function->upvalueCount++;
}

/** Resolve a reference to a upvalue.
 *
 * @param compiler the current compiler state
 * @param name the identifier lexeme
 * @returns -1 if the identifier was not found
 * @returns the index of the match in the compiler upvalues
 */
static int
resolveUpvalue (Compiler *compiler, Token *name)
{
    if (compiler->enclosing == NULL)
        return -1;

    int local = resolveLocal (compiler->enclosing, name);

    if (local != -1) {
        compiler->enclosing->locals[local].isCaptured = true;
        return addUpvalue (compiler, (uint8_t) local, true);
    }

    int upvalue = resolveUpvalue (compiler->enclosing, name);

    if (upvalue != -1) {
        return addUpvalue (compiler, (uint8_t) upvalue, false);
    }

    return -1;
}

/** Add a local variable to the current scope.
 *
 * @param name the lexeme containing the variable name
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
    local->depth = -1;
    local->isCaptured = false;
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

/** Parse a variable
 *
 * @param errorMessage message to emit if this is not a variable
 * @returns zero if we are inside a scope
 * @returns otherwise, the index into the constant table
 */
static uint8_t
parseVariable (const char *errorMessage)
{
    consume (TOKEN_IDENTIFIER, errorMessage);
    declareVariable ();
    if (current->scopeDepth > 0)
        return 0;
    return identifierConstant (&parser.previous);
}

/** Mark the local being defined as initialized.
 *
 * Note that "var x;" is really "var x = nil;" and thus
 * leaves "x" in the initialized state here.
 */
static void
markInitialized ()
{
    if (current->scopeDepth == 0)
        return;
    current->locals[current->localCount - 1].depth = current->scopeDepth;
}

/** Compile a variable definition.
 *
 * @param global index of the global
 */
static void
defineVariable (uint8_t global)
{
    if (current->scopeDepth > 0) {
        markInitialized ();
        return;
    }
    emitBytes (OP_DEFINE_GLOBAL, global);
}

/** Compile a function call argument list.
 *
 * @returns the number of arguments.
 */
static uint8_t
argumentList ()
{
    uint8_t argCount = 0;

    if (!check (TOKEN_RIGHT_PAREN)) {
        do {
            expression ();
            if (argCount == 255) {
                error ("Can't have more than 255 arguments.");
            }
            argCount++;
        } while (match (TOKEN_COMMA));
    }
    consume (TOKEN_RIGHT_PAREN, "Expect ')' after arguments.");
    return argCount;
}

/** Compile an "and" expression
 *
 * @param canAssign not used here.
 */
static void
and_ (bool canAssign)
{
    (void) canAssign;                   // not used by the "and" operator.
    int endJump = emitJump (OP_JUMP_IF_FALSE);

    emitByte (OP_POP);
    parsePrecedence (PREC_AND);

    patchJump (endJump);
}

/** Compile a binary operation to the chunk.
 *
 * @param canAssign not used by this function
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

/** Compile a function call operration to the chunk.
 *
 * @param canAssign not used by this function
 */
static void
call (bool canAssign)
{
    (void) canAssign;                   // not used by this operation.
    uint8_t argCount = argumentList ();

    emitBytes (OP_CALL, argCount);
}

/** Class instance field (get and set)
 *
 * @param canAssign allow (or deny) assignment
 */
static void
dot (bool canAssign)
{
    consume (TOKEN_IDENTIFIER, "Expect property name after '.'.");
    uint8_t name = identifierConstant (&parser.previous);

    if (canAssign && match (TOKEN_EQUAL)) {
        expression ();
        emitBytes (OP_SET_PROPERTY, name);
    } else {
        emitBytes (OP_GET_PROPERTY, name);
    }
}

/** Compile a Literal op to the chunk.
 *
 * @param canAssign not used by this function
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
 *
 * @param canAssign not used by this function
 */
static void
grouping (bool canAssign)
{
    (void) canAssign;                   // not used
    expression ();
    consume (TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

/** Compile a number to the chunk.
 *
 * @param canAssign not used by this function
 */
static void
number (bool canAssign)
{
    (void) canAssign;                   // not used
    double value = strtod (parser.previous.start, NULL);

    emitConstant (NUMBER_VAL (value));
}

/** Compile an "or" expression
 *
 * @param canAssign not used here.
 */
static void
or_ (bool canAssign)
{
    (void) canAssign;                   // not used by the "and" operator.

    int elseJump = emitJump (OP_JUMP_IF_FALSE);
    int endJump = emitJump (OP_JUMP);

    patchJump (elseJump);
    emitByte (OP_POP);

    parsePrecedence (PREC_OR);
    patchJump (endJump);
}

/** Compile a string to the chunk.
 *
 * @param canAssign not used by this function
 */
static void
string (bool canAssign)
{
    (void) canAssign;                   // not used
    emitConstant (OBJ_VAL (copyString (parser.previous.start + 1, parser.previous.length - 2)));
}

/** Do the work of putting a variable into the chunk.
 *
 * @param name the lexeme with the variable name
 * @param canAssign not used here.
 */
static void
namedVariable (Token name, bool canAssign)
{
    uint8_t getOp, setOp;
    int arg = resolveLocal (current, &name);

    if (arg != -1) {
        getOp = OP_GET_LOCAL;
        setOp = OP_SET_LOCAL;
    } else if ((arg = resolveUpvalue (current, &name)) != -1) {
        getOp = OP_GET_UPVALUE;
        setOp = OP_SET_UPVALUE;
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
 *
 * @param canAssign true to allow an assignment expression
 */
static void
variable (bool canAssign)
{
    namedVariable (parser.previous, canAssign);
}

/** Compile a unary operation to the chunk.
 *
 * @param canAssign not used by this function
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

/** Parse Rules
 *
 * This table encodes the behavior of the parser based on the next
 * token available. The first value points to the function to call
 * when the token appears as a unary operator. The second column is
 * the function to call when the token appears as a binary operator.
 * The third column indicates the precedence of the token when used
 * as a binary operator.
 */
ParseRule
  rules[] = {

    // *INDENT-OFF*

    // Single-character tokens.
    [TOKEN_LEFT_PAREN]     =  {  grouping,   call,     PREC_CALL        },   //  "("
    [TOKEN_RIGHT_PAREN]    =  {  NULL,       NULL,     PREC_NONE        },   //  ")"
    [TOKEN_LEFT_BRACE]     =  {  NULL,       NULL,     PREC_NONE        },   //  "{"
    [TOKEN_RIGHT_BRACE]    =  {  NULL,       NULL,     PREC_NONE        },   //  "}"
    [TOKEN_COMMA]          =  {  NULL,       NULL,     PREC_NONE        },   //  ", "
    [TOKEN_DOT]            =  {  NULL,       dot,      PREC_CALL        },   //  "."
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
    [TOKEN_AND]            =  {  NULL,       and_,     PREC_AND         },   //  "and"
    [TOKEN_CLASS]          =  {  NULL,       NULL,     PREC_NONE        },   //  "class"
    [TOKEN_ELSE]           =  {  NULL,       NULL,     PREC_NONE        },   //  "else"
    [TOKEN_FALSE]          =  {  literal,    NULL,     PREC_NONE        },   //  "false"
    [TOKEN_FOR]            =  {  NULL,       NULL,     PREC_NONE        },   //  "for"
    [TOKEN_FUN]            =  {  NULL,       NULL,     PREC_NONE        },   //  "fun"
    [TOKEN_IF]             =  {  NULL,       NULL,     PREC_NONE        },   //  "if"
    [TOKEN_NIL]            =  {  literal,    NULL,     PREC_NONE        },   //  "nil"
    [TOKEN_OR]             =  {  NULL,       or_,      PREC_OR          },   //  "or"
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
 *
 * @param precedence threshold for binary operation precedence
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

/** Fetch the correct rule structuer
 *
 * @param type the token type we are processing
 * @returns a pointer to the appropriate rule structure
 */
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

/** Compile a statement block to the current chunk.
 */
static void
block ()
{
    while (!check (TOKEN_RIGHT_BRACE) && !check (TOKEN_EOF)) {
        declaration ();
    }
    consume (TOKEN_RIGHT_BRACE, "Expect '}' after block.");
}

/** Compile a function.
 *
 * @param type either FUNCTION or SCRIPT
 */
static void
function (FunctionType type)
{
    Compiler compiler;

    initCompiler (&compiler, type);
    beginScope ();

    consume (TOKEN_LEFT_PAREN, "Expect '(' after function name.");

    if (!check (TOKEN_RIGHT_PAREN)) {
        do {
            current->function->arity++;
            if (current->function->arity > 255) {
                errorAtCurrent ("Can't have more than 255 parameters.'");
            }
            uint8_t constant = parseVariable ("Expect parameter name.");

            defineVariable (constant);
        } while (match (TOKEN_COMMA));
    }

    consume (TOKEN_RIGHT_PAREN, "Expect ')' after parameters.");
    consume (TOKEN_LEFT_BRACE, "Expect '{' before function body.");
    block ();

    ObjFunction *function = endCompiler ();

    emitBytes (OP_CLOSURE, makeConstant (OBJ_VAL (function)));
    for (int i = 0; i < function->upvalueCount; i++) {
        emitByte (compiler.upvalues[i].isLocal ? 1 : 0);
        emitByte (compiler.upvalues[i].index);
    }
}

/** Compile a class instance method
 */
static void
method ()
{
    consume (TOKEN_IDENTIFIER, "Expect method name.");
    uint8_t constant = identifierConstant (&parser.previous);

    FunctionType type = TYPE_FUNCTION;

    function (type);

    emitBytes (OP_METHOD, constant);
}

/** Compile a class declaration
 */
static void
classDeclaration ()
{
    consume (TOKEN_IDENTIFIER, "Expect class name.");
    Token className = parser.previous;
    uint8_t nameConstant = identifierConstant (&parser.previous);

    declareVariable ();

    emitBytes (OP_CLASS, nameConstant);
    defineVariable (nameConstant);

    namedVariable (className, false);

    consume (TOKEN_LEFT_BRACE, "Expect '{' before class body.");
    while (!check (TOKEN_RIGHT_BRACE) && !check (TOKEN_EOF)) {
        method ();
    }
    consume (TOKEN_RIGHT_BRACE, "Expect '}' after class body.");

    emitByte (OP_POP);
}

/** Compile a fun declaration to its own captive chunk.
 */
static void
funDeclaration ()
{
    uint8_t global = parseVariable ("Excpect function name.");

    markInitialized ();
    function (TYPE_FUNCTION);
    defineVariable (global);
}

/** Compile a var declaration to the current chunk.
 */
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

/** Compile an "for" statement to the chunk.
 */
static void
forStatement ()
{
    beginScope ();
    consume (TOKEN_LEFT_PAREN, "Expect '(' after 'if'.");

    if (match (TOKEN_SEMICOLON)) {
        // No initializer.
    } else if (match (TOKEN_VAR)) {
        varDeclaration ();
    } else {
        expressionStatement ();
    }

    int loopStart = currentChunk ()->count;

    int exitJump = -1;

    if (!match (TOKEN_SEMICOLON)) {
        expression ();
        consume (TOKEN_SEMICOLON, "Expect ';' after condition.");

        // Jump out of the loop if the condition is false.
        exitJump = emitJump (OP_JUMP_IF_FALSE);
        emitByte (OP_POP);              // Condition
    }

    if (!match (TOKEN_RIGHT_PAREN)) {
        int bodyJump = emitJump (OP_JUMP);
        int incrementStart = currentChunk ()->count;

        expression ();
        emitByte (OP_POP);
        consume (TOKEN_RIGHT_PAREN, "Expect ')' after for clauses.");

        emitLoop (loopStart);
        loopStart = incrementStart;
        patchJump (bodyJump);
    }

    statement ();
    emitLoop (loopStart);

    if (exitJump != -1) {
        patchJump (exitJump);
        emitByte (OP_POP);              // Condition
    }

    endScope ();
}

/** Compile an "if" statement to the chunk.
 */
static void
ifStatement ()
{
    consume (TOKEN_LEFT_PAREN, "Expect '(' after 'if'.");
    expression ();
    consume (TOKEN_RIGHT_PAREN, "Expect ')' after condition.");

    int thenJump = emitJump (OP_JUMP_IF_FALSE);

    emitByte (OP_POP);
    statement ();

    int elseJump = emitJump (OP_JUMP);

    patchJump (thenJump);
    emitByte (OP_POP);
    if (match (TOKEN_ELSE))
        statement ();
    patchJump (elseJump);
}

/** Compile a print statement to the chunk.
 */
static void
printStatement ()
{
    expression ();
    consume (TOKEN_SEMICOLON, "Expect ';' after value.");
    emitByte (OP_PRINT);
}

/** Compile a return statement to the chunk.
 */
static void
returnStatement ()
{
    if (current->type == TYPE_SCRIPT) {
        error ("Can't return from top-level code.");
    }

    if (match (TOKEN_SEMICOLON)) {
        emitReturn ();
    } else {
        expression ();
        consume (TOKEN_SEMICOLON, "Expect ';' after return value.");
        emitByte (OP_RETURN);
    }
}

/** Compile a while statement to the chunk.
 */
static void
whileStatement ()
{
    int loopStart = currentChunk ()->count;

    consume (TOKEN_LEFT_PAREN, "Expect '(' after 'while'.");
    expression ();
    consume (TOKEN_RIGHT_PAREN, "Expect ')' after condition.");

    int exitJump = emitJump (OP_JUMP_IF_FALSE);

    emitByte (OP_POP);
    statement ();

    emitLoop (loopStart);

    patchJump (exitJump);
    emitByte (OP_POP);
}

/** Synchronize the parser after an error
 *
 * Skip forward until we are
 * - at EOF
 * - after a SEMICOLON
 * - before CLASS, FUN, VAR, FOR, IF, WHILE, PRINT, or RETURN
 */
static void
synchronize ()
{
    parser.panicMode = false;

    while (parser.current.type != TOKEN_EOF) {
        if (parser.previous.type == TOKEN_SEMICOLON)
            return;

        switch (parser.current.type) {

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
        }

        advance ();
    }
}

/** Compile a declaration to the chunk.
 */
static void
declaration ()
{
    if (match (TOKEN_CLASS)) {
        classDeclaration ();
    } else if (match (TOKEN_FUN)) {
        funDeclaration ();
    } else if (match (TOKEN_VAR)) {
        varDeclaration ();
    } else {
        statement ();
    }

    if (parser.panicMode)
        synchronize ();
}

/** Compile a statement to the chunk.
 */
static void
statement ()
{
    if (match (TOKEN_PRINT)) {
        printStatement ();
    } else if (match (TOKEN_FOR)) {
        forStatement ();
    } else if (match (TOKEN_IF)) {
        ifStatement ();
    } else if (match (TOKEN_RETURN)) {
        returnStatement ();
    } else if (match (TOKEN_WHILE)) {
        whileStatement ();
    } else if (match (TOKEN_LEFT_BRACE)) {
        beginScope ();
        block ();
        endScope ();
    } else {
        expressionStatement ();
    }
}

/** Compile the source code into the chunk.
 *
 * @param source pointer to a C string containing the source to compile
 * @returns true if all went well
 * @returns false if there was a parser error
 */
ObjFunction *
compile (const char *source)
{
    initScanner (source);
    Compiler compiler;

    initCompiler (&compiler, TYPE_SCRIPT);
    parser.hadError = false;
    parser.panicMode = false;

    advance ();
    while (!match (TOKEN_EOF)) {
        declaration ();
    }

    ObjFunction *function = endCompiler ();

    return parser.hadError ? NULL : function;
}

/** Mark all compiler roots as reachable.
 */
void
markCompilerRoots ()
{
    Compiler *compiler = current;

    while (compiler != NULL) {
        markObject ((Obj *) compiler->function);
        compiler = compiler->enclosing;
    }
}
