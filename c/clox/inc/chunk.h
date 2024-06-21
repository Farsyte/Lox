#pragma once

#include "common.h"

typedef uint8_t Code;

typedef enum
{
    OP_RETURN,
} OpCode;

typedef struct
{
    OpCode *code;
} Chunk;

extern int post_chunk (
    );
extern int bist_chunk (
    );
