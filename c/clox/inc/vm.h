#pragma once

#include "common.h"
#include "value.h"

/** Maximum Stack Depth
 *
 * \todo consider using an expandable array.
 */
#define STACK_MAX 256

/** Internal state of the VM
 */
struct vm_s {
    Chunk *chunk;               ///< current chunk being considered
    uint8_t *ip;                ///< bytecode instruction pointer
    Value stack[STACK_MAX];     ///< storage for the data stack
    Value *sp;                  ///< data stack pointer
};

/** Interpreter result
 */
enum interpret_result_e {
    INTERPRET_OK,               ///< everything went well
    INTERPRET_COMPILE_ERROR,    ///< error during compilation
    INTERPRET_RUNTIME_ERROR,    ///< error during interpretation
};

extern void initVM (
    );

extern void freeVM (
    );

extern InterpretResult interpretChunk (
    Chunk *chunk);

extern InterpretResult interpret (
    const char *source);

extern void push (
    Value value);

extern Value pop (
    );

extern Value peek (
    int distance);

extern void postVM (
    );
extern void bistVM (
    );
