#pragma once

#include "common.h"

/** @file debug.h
 * @brief Macros and APIs exported from the debug module
 */

extern void disassembleChunk (
    Chunk *chunk,
    const char *name);

extern int disassembleInstruction (
    Chunk *chunk,
    int offset);
