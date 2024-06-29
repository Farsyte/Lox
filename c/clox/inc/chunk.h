#pragma once

#include "common.h"
#include "value.h"

/** @file chunk.h
 * @brief API exposed by the Chunk module
 */

/** Enumerate defined operation codes.
 */
enum opcode_e {
    OP_CONSTANT,                ///< Push a constant onto the stack
    OP_NEGATE,                  ///< invert the sign of the top of the stack
    OP_RETURN,                  ///< Stop executing a bytecode sequence

    OP__LAST = OP_RETURN        ///< Expose highest opcode for testing
};

/** Dynamic Array of Instructions
 */
struct chunk_s {
    int count;                  ///< number of bytecode bytes present
    int capacity;               ///< reallocate when we grow past this
    uint8_t *code;              ///< Storage for some bytecodes
    int *lines;                 ///< parallel array of line numbers
    ValueArray constants;       ///< Expandable pool of consetant values
};

extern void initChunk (
    Chunk *chunk);

extern void freeChunk (
    Chunk *chunk);

extern void writeChunk (
    Chunk *chunk,
    uint8_t byte,
    int line);

extern int addConstant (
    Chunk *chunk,
    Value value);

extern void postChunk (
    );
extern void bistChunk (
    );
