#pragma once

#include "common.h"
#include "table.h"
#include "value.h"

/** Maximum Stack Depth
 *
 * \todo consider using an expandable array.
 */
#define STACK_MAX 256

/** Internal state of the VM
 */
struct VM {
    Chunk *chunk;               ///< current chunk being considered
    uint8_t *ip;                ///< bytecode instruction pointer
    Value stack[STACK_MAX];     ///< storage for the data stack
    Value *sp;                  ///< data stack pointer
    Table strings;
    Obj *objects;               ///< linked list of all objects
};

/** Interpreter result
 */
enum InterpretResult {
    INTERPRET_OK,               ///< everything went well
    INTERPRET_COMPILE_ERROR,    ///< error during compilation
    INTERPRET_RUNTIME_ERROR,    ///< error during interpretation
};

extern VM vm;                   ///< export the singleton VM instance.

extern void initVM ();
extern void freeVM ();
extern InterpretResult interpretChunk (Chunk *chunk);
extern InterpretResult interpret (const char *source);
extern void push (Value value);
extern Value pop ();
extern Value peek (int distance);

extern void postVM ();
extern void bistVM ();
