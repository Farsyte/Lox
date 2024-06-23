#pragma once

#include "chunk.h"

typedef struct {
    Chunk *chunk;
} VM;

void initVM (
    );
void freeVM (
    );

void post_vm (
    );
void bist_vm (
    );
