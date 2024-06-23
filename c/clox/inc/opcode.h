#pragma once

#include "bist.h"
#include "post.h"

typedef enum {
    // reserve 0x00 as "not initialized"
    OP_CONSTANT = 1,
    OP_CONSTANT_LONG,
    OP_NEGATE,
    OP_RETURN,
} OpCode;

post_fn post_opcode;
bist_fn bist_opcode;
