#pragma once

#include "common.h"

typedef uint8_t Code;

typedef enum
{
    OP_RETURN,
} OpCode;

extern int post_chunk (
    );
extern int bist_chunk (
    );
