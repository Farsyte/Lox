#pragma once

#include "common.h"

/** @file chunk.h
 * @brief API exposed by the Chunk module
 */

/** Enumerate defined operation codes.
 */
enum opcode {
    OP_RETURN,                  ///< Stop executing a bytecode sequence
};

extern void postChunk (
    );
extern void bistChunk (
    );
