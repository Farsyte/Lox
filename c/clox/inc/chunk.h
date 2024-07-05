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
    OP_RETURN,                  ///< Stop executing a bytecode sequence

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
