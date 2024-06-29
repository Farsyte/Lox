#pragma once

#include "common.h"

struct vm_s {
    Chunk *chunk;
    uint8_t *ip;
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

extern InterpretResult interpret (
    Chunk *chunk);

extern void postVM (
    );
extern void bistVM (
    );
