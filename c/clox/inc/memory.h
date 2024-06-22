#pragma once

#include "assert.h"
#include "common.h"

#include <stdlib.h>

#define GROW_CAPACITY(capacity)			\
    ((capacity) < 8 ? 8 : (capacity) * 2)

#define GROW_ARRAY(type, pointer, oldCount, newCount)	\
    (type *)checked_grow_array(\
	sizeof (type), pointer, oldCount, newCount)

void *checked_grow_array (
    size_t size,
    void *pointer,
    size_t oldCount,
    size_t newCount);
