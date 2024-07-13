#pragma once

#include "common.h"

/** @file memory.h
 * @brief Macros and API exposed by memory module
 */

/** Minimum size for GROW based storage. */
#define GROW_MIN_SIZE   8

/** How much to grow the capacity each time */
#define GROW_MUL_FAC    2

/** Allocate memory */
#define ALLOCATE(type, count)                           ((type*)reallocate(NULL, 0, sizeof (type) * (count)))

/** Free memory */
#define FREE(type, pointer)                             (reallocate(pointer, sizeof (type), 0))

/** Compute new capacity from old capacity */
#define GROW_CAPACITY(capacity)                         ((capacity) < GROW_MIN_SIZE ? GROW_MIN_SIZE : (capacity) * GROW_MUL_FAC)

/** Allocate and fill new storage from old */
#define GROW_ARRAY(type, pointer, oldCount, newCount)   ((type *)reallocate(pointer, sizeof (type) * (oldCount), sizeof (type) * (newCount)))

/** Release the growable storage */
#define FREE_ARRAY(type, pointer, oldCount)             (reallocate(pointer, sizeof (type) * (oldCount), 0))

extern void initMemory ();
extern const char *printableHeapAddr (void *ptr);
extern void *reallocate (void *pointer, size_t oldSize, size_t newSize);
extern void collectGarbage ();
extern void freeObjects ();

extern void postMemory ();
extern void bistMemory ();
