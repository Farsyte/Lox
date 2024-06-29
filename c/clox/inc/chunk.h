#pragma once

#include "common.h"

typedef enum {
    OP_RETURN,
} OpCode;

extern post_fn post_chunk;
extern bist_fn bist_chunk;
