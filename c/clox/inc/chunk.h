#pragma once

#include "common.h"

typedef enum {
    OP_RETURN,
} OpCode;

typedef struct {
    uint8_t* code;
} Chunk;
