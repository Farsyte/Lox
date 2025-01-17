#pragma once

#include "common.h"
#include "value.h"

/** @file chunk.h
 * @brief API exposed by the Chunk module
 */

/** Enumerate defined operation codes.
 */
typedef enum {
    OP_CONSTANT,                ///< Push a constant onto the stack
    OP_NIL,                     ///< Push a nil onto the stack
    OP_TRUE,                    ///< Push a false onto the stack
    OP_FALSE,                   ///< Push a true onto the stack
    OP_POP,                     ///< discard the top value from the stack
    OP_GET_LOCAL,               ///< push value of local variable onto the stack
    OP_SET_LOCAL,               ///< pop top of stack into local variable
    OP_GET_GLOBAL,              ///< push value of global variable onto the stack
    OP_DEFINE_GLOBAL,           ///< define a global variable
    OP_SET_GLOBAL,              ///< pop top of stack into global variable
    OP_GET_UPVALUE,             ///< push value of upvalue variable onto the stack
    OP_SET_UPVALUE,             ///< pop top of stack into upvalue variable
    OP_GET_PROPERTY,            ///< push value of instance property
    OP_SET_PROPERTY,            ///< pop top of stack into instance property

    OP_GET_SUPER,               ///< execute a "super" access

    OP_EQUAL,                   ///< compare top two values for equality
    OP_GREATER,                 ///< compare top two values for order
    OP_LESS,                    ///< compare top two values for order

    OP_ADD,                     ///< add top of stack to 2nd, push result
    OP_SUBTRACT,                ///< subtract top of stack from 2nd, push result
    OP_MULTIPLY,                ///< multiply top of stack by 2nd, push result
    OP_DIVIDE,                  ///< divide top of stack by 2nd, push result

    OP_NOT,                     ///< logical inversion
    OP_NEGATE,                  ///< invert the sign of the top of the stack
    OP_PRINT,                   ///< print the value on the top of the stack
    OP_JUMP,                    ///< unconditional forward branch 
    OP_JUMP_IF_FALSE,           ///< conditional forward branch
    OP_LOOP,                    ///< unconditional backward branch
    OP_CALL,                    ///< function call
    OP_INVOKE,                  ///< optimized method invocation
    OP_SUPER_INVOKE,            ///< optimized superclass method invocation
    OP_CLOSURE,                 ///< make a closure
    OP_CLOSE_UPVALUE,           ///< convert upvalue from stack to heap
    OP_RETURN,                  ///< Stop executing a bytecode sequence
    OP_CLASS,                   ///< make a class
    OP_INHERIT,                 ///< set superclass of new class
    OP_METHOD,                  ///< make a class instance method

    OP__LAST = OP_RETURN        ///< Expose highest opcode for testing
} OpCode;

/** Dynamic Array of Instructions
 */
struct Chunk {
    int count;                  ///< number of bytecode bytes present
    int capacity;               ///< reallocate when we grow past this
    uint8_t *code;              ///< Storage for some bytecodes
    int *lines;                 ///< parallel array of line numbers
    ValueArray constants;       ///< Expandable pool of consetant values
};

extern void initChunk (Chunk *chunk);
extern void freeChunk (Chunk *chunk);
extern void writeChunk (Chunk *chunk, uint8_t byte, int line);
extern int addConstant (Chunk *chunk, Value value);

extern void postChunk ();
extern void bistChunk ();
