#pragma once

#include "chunk.h"
#include "value.h"

typedef struct {
    Chunk *chunk;
    OpCode *ip;
    Value *stack_base;
    Value *stack_limit;
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
