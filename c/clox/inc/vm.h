#pragma once

#include "chunk.h"

typedef struct {
    Chunk *chunk;
    OpCode *ip;
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

InterpretResult interpret (
    Chunk *chunk);

void post_vm (
    );
void bist_vm (
    );
