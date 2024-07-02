#include "value.h"

#include "memory.h"

#include <stdio.h>

/** @file value.c
 * @brief Support functions for the value module
 */

/** Initialize Value Array.
 *
 * Place ValueArray into initial state
 * - no values stored
 * - current capacity is zero
 * - no value storage owned
 *
 * @param array memory to be initialized
 */
void
initValueArray (
    ValueArray *array)
{
    INVAR (NULL != array, "NULL array passed to initValueArray");

    array->values = NULL;
    array->capacity = 0;
    array->count = 0;
}

/** Write an OpCode onto the end of the array.
 *
 * @param array what to write into
 * @param code value to add to it
 */
void
writeValueArray (
    ValueArray *array,
    Value value)
{
    if (array->capacity < array->count + 1) {
        int oldCapacity = array->capacity;

        array->capacity = GROW_CAPACITY (oldCapacity);
        array->values = GROW_ARRAY (Value,
            array->values,
            oldCapacity,
            array->capacity);
    }

    array->values[array->count] = value;
    array->count++;
}

/** Release resources owned by ValueArray.
 *
 * This function frees all memory owned by the array
 * and returns it to its initial state.
 *
 * This does NOT include doing a ~free(array)~
 * because array itself may not have been allocated.
 *
 * @param array the array to manipulate
 */
void
freeValueArray (
    ValueArray *array)
{
    FREE_ARRAY (Value,
        array->values,
        array->capacity);

    initValueArray (array);
}

/** Print a value.
 *
 * This function presents the value, which Lox always stores in a
 * floating point variable; but if the value is an integer value,
 * print it without the decimal point and especially not in
 * exponential notation.
 *
 * @param value the data to be printed
 */
void
printValue (
    Value value)
{
    if (IS_NIL (value)) {
        printf ("nil");
        return;
    }

    if (IS_BOOL (value)) {
        if (AS_BOOL (value)) {
            printf ("true");

        } else {
            printf ("false");

        }
        return;
    }

    if (IS_NUMBER (value)) {
        double number = AS_NUMBER (value);

        if (number == (int) number) {
            printf ("%.0f", number);
        } else {
            printf ("%g", number);
        }
        return;
    }

    printf ("Value{ty=%d, bv=%d, nv=%g}",
        value.type, value.as.boolean, value.as.number);

}
