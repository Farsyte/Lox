#include "memory.h"

#include <stdio.h>              // perror

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

    if (newSize && !newPointer) {
        perror ("realloc");
        abort ();
    }
    return newPointer;
}
