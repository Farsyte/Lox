#pragma once

#include "common.h"

/** Maximum Stack Depth
 *
 * \todo consider using an expandable array.
 */
#define STACK_MAX 256

struct vm_s {
    Chunk *chunk;
    uint8_t *ip;
    Value stack[STACK_MAX];
    Value *sp;
};

enum interpret_result_e {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
};

extern VM vm;

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

extern void postVM (
    );
extern void bistVM (
    );
