#include "memory.h"

/** @file memory.c
 * @brief Memory Handling module
 */

/** Reallocate storage
 *
 * Given a pointer to oldSize bytes of allocated storage, return a
 * pointer to newSize bytes of allocated storage, with the contents of
 * the original area copied to the new area, and release the old
 * storage back to the storage pool.
 *
 * The macro does not use realloc directly, because we want allocation
 * errors to be reported and to terminate the program.
 *
 * @param pointer address of existing storage block
 * @param oldSize size in bytes of existing storage block
 * @param newSize desired size in bytes of new storage block
 */
void *
reallocate (
    void *pointer,
    size_t oldSize,
    size_t newSize)
{
    (void) oldSize;             // not needed by this implementation.

    if (newSize == 0) {
        free (pointer);
        return NULL;
    }

    void *result = realloc (pointer, newSize);

    INVAR (NULL != result, "reallocate failed.");

    return result;
}
