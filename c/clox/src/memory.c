#include "memory.h"

#include "common.h"

void *
reallocate (
    void *pointer,
    size_t oldSize,
    size_t newSize)
{
    (void) oldSize;
    (void) newSize;
    return pointer;
}
