#pragma once

#include "vm.h"

bool compile (
    const char *source,
    Chunk *chunk);

void bistCompiler (
    );
void postCompiler (
    );
