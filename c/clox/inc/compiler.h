#pragma once

#include "object.h"
#include "vm.h"

/** @file compiler.h
 * @brief API exposed by the COMPILER module
 */

ObjFunction *compile (const char *source);

void bistCompiler ();
void postCompiler ();
