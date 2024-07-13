#pragma once

#include "object.h"
#include "vm.h"

/** @file compiler.h
 * @brief API exposed by the COMPILER module
 */

extern ObjFunction *compile (const char *source);
extern void markCompilerRoots ();

void bistCompiler ();
void postCompiler ();
