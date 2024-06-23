#pragma once

#include "chunk.h"
#include "value.h"

#define	STACK_MAX	256

typedef struct {
    Chunk *chunk;
    OpCode *ip;
    Value stack[STACK_MAX];
    Value *sp;
} VM;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpretResult;

void initVM (
    );
void freeVM (
    );

void resetStack (
    );
void push (
    Value value);
Value pop (
    );

InterpretResult interpret (
    Chunk *chunk);

void post_vm (
    );
void bist_vm (
    );
