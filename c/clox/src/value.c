#include "value.h"

#include "memory.h"
#include "object.h"

#include <stdio.h>
#include <string.h>

/** @file value.c
 * @brief Support functions for the value module
 */

/** Decide if two Value objects are equal.
 *
 * Equality is defined as:
 * - the two objects have the same type code.
 * - the values are equal per the rules for that type
 *
 * Note that for OBJECTS (Functions and Strings), equality
 * is defined as referencing the same Obj structure.
 *
 * @param a the first value to compare
 * @param b the second value to compare
 * @returns true if the values are equal; otherwise, false.
 */
bool
valuesEqual (Value a, Value b)
{
    if (a.type != b.type)
        return false;
    switch (a.type) {

        // *INDENT-OFF*

    case VAL_BOOL:      return AS_BOOL(a) == AS_BOOL(b);
    case VAL_NIL:       return true;
    case VAL_NUMBER:    return AS_NUMBER(a) == AS_NUMBER(b);
    case VAL_OBJ:       return AS_OBJ(a) == AS_OBJ(b);

        // *INDENT-ON*

    }

    UNREACHABLE ("corrupted value type");
    return false;
}

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
initValueArray (ValueArray *array)
{
    INVAR (NULL != array, "NULL array passed to initValueArray");

    array->values = NULL;
    array->capacity = 0;
    array->count = 0;
}

/** Write an OpCode onto the end of the array.
 *
 * @param array what to write into
 * @param value copy of the Value to add to it
 */
void
writeValueArray (ValueArray *array, Value value)
{
    if (array->capacity < array->count + 1) {
        int oldCapacity = array->capacity;

        array->capacity = GROW_CAPACITY (oldCapacity);
        array->values = GROW_ARRAY (Value, array->values, oldCapacity, array->capacity);
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
freeValueArray (ValueArray *array)
{
    FREE_ARRAY (Value, array->values, array->capacity);

    initValueArray (array);
}

/** Print a value.
 *
 * @param value the data to be printed
 */
void
printValue (Value value)
{
    switch (value.type) {

        // *INDENT-OFF*

    case VAL_BOOL:      printf(AS_BOOL(value) ? "true" : "false");      return;
    case VAL_NIL:       printf("nil");                                  return;
    case VAL_NUMBER:    printf("%.16g", AS_NUMBER(value));              return;
    case VAL_OBJ:       printObject(value);                             return;

        // *INDENT-ON*

    }
    UNREACHABLE ("corrupted value type");
}
