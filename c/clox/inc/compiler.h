#pragma once

#include "vm.h"

extern Parser parser;

bool compile (
    const char *source,
    Chunk *chunk);

void bistCompiler (
    );
void postCompiler (
    );
