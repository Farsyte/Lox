#pragma once

#include "common.h"

#include <stdlib.h>

#define GROW_CAPACITY(capacity)			\
    ((capacity) < 8 ? 8 : (capacity) * 2)

#define GROW_ARRAY(type, pointer, oldCount, newCount)	\
    (type *)realloc( 					\
	(pointer), (newCount) * sizeof (type))
