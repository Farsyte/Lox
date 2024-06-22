#include "value.h"

#include "assert.h"
#include "memory.h"

void
initValueArray (
    ValueArray *array)
{
    array->values = NULL;
    array->capacity = 0;
    array->count = 0;
}

void
freeValueArray (
    ValueArray *array)
{
    FREE_ARRAY (Value,
        array->values,
        array->capacity);

    initValueArray (array);
}

void
writeValueArray (
    ValueArray *array,
    Value value)
{
    assert (NULL != array, "array must not be NULL.");

    if (array->capacity <= array->count) {
        int oldCapacity = array->capacity;
        int newCapacity = GROW_CAPACITY (oldCapacity);
        array->values = GROW_ARRAY (Value,
            array->values,
            oldCapacity,
            newCapacity);

        array->capacity = newCapacity;
    }

    array->values[array->count] = value;
    array->count++;
}
