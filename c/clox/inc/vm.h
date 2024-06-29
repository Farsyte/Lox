#pragma once

#include "common.h"

struct vm_s {
    Chunk *chunk;
};

extern VM vm;

extern void initVM (
    );

extern void freeVM (
    );

extern void postVM (
    );
extern void bistVM (
    );
