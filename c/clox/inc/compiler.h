#pragma once

#include "vm.h"

/** @file compiler.h
 * @brief API exposed by the COMPILER module
 */

bool compile (const char *source, Chunk *chunk);

void bistCompiler ();
void postCompiler ();
