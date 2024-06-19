#include <stdlib.h>
#include <assert.h>

#include "memory.h"

void* reallocate(void* pointer, size_t oldSize, size_t newSize) {

    (void)oldSize;		// not used by this implementation.

    if (newSize == 0) {
	free(pointer);
	return NULL;
    }

    void* result = realloc(pointer, newSize);
    assert(result != NULL); // a LITTLE better than a blind exit(1) ...
    return result;
}
