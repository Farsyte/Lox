#include "memory.h"

#include "assert.h"             // _build_msg

#include <errno.h>              // errno
#include <stdio.h>              // fprintf, stderr
#include <string.h>             // strerror

void *
checked_grow_array (
    size_t size,
    void *pointer,
    size_t oldCount,
    size_t newCount)
{
    (void) oldCount;            // not needed by this implementation.

    size_t newSize = newCount * size;
    void *newPointer = realloc (pointer, newSize);

    if (newSize)
        assert (NULL != newPointer,
            "Unable to allocate memory to grow the array.");

    return newPointer;
}
