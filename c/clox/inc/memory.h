#pragma once

#include "common.h"

/** @file memory.h
 * @brief Macros and API exposed by memory module
 */

#define GROW_MIN_SIZE   8
#define GROW_MUL_FAC    2

#define GROW_CAPACITY(capacity)                                         \
    ((capacity) < GROW_MIN_SIZE ? GROW_MIN_SIZE : (capacity) * GROW_MUL_FAC)

#define GROW_ARRAY(type, pointer, oldCount, newCount)           \
    (type *)reallocate(pointer, sizeof (type) * (oldCount),     \
                       sizeof (type) * (newCount))

#define FREE_ARRAY(type, pointer, oldCount)             \
    reallocate(pointer, sizeof (type) * (oldCount), 0)

extern void *reallocate (
    void *pointer,
    size_t oldSize,
    size_t newSize);

extern void postMemory (
    );
extern void bistMemory (
    );
