#pragma once

#include "bist.h"
#include "post.h"

typedef enum
{
    OP_RETURN,
} OpCode;

typedef struct
{

    // TODO centralized "extensible array"
    int count;
    int capacity;
    OpCode *code;

} Chunk;

post_fn post_chunk;
bist_fn bist_chunk;
