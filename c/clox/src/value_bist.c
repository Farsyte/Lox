#include "value.h"

#include <stdio.h>

/** @file value_bist.c
 * @brief Built-In Self Test for the VALUE module.
 */

/** Run all BIST cases for Value.
 *
 * Basically, just catch any changes that make Value unable to store
 * numbers, including non-integer values.
 */
void
bistValue (
    )
{
    INVAR (0 == (Value) 0, "Value has zero");
    INVAR (1 == (Value) 1, "Value has one");
    INVAR (0.5 == (Value) 0.5, "Value has one-half");

    ValueArray array;

    initValueArray (&array);
    INVAR (0 == array.count, "initValueArray did not clear the count");
    INVAR (0 == array.capacity, "initValueArray did not clear the capacity");
    INVAR (NULL == array.values,
        "initValueArray did not null the code pointer");

    for (int i = 0; i < 256; ++i)
        writeValueArray (&array, 123.5 * i);
    writeValueArray (&array, 1337.0);

    INVAR (257 == array.count, "writeValueArray did not update the count");
    INVAR (512 == array.capacity,
        "writeValueArray did not update the capacity");
    INVAR (NULL != array.values,
        "writeValueArray did not update the code pointer");

    for (int i = 0; i < 256; ++i)
        INVAR ((123.5 * i) == array.values[i],
            "writeValueArray did not write the data");
    INVAR (1337.0 == array.values[256],
        "writeValueArray did not write the data");

    freeValueArray (&array);
    INVAR (0 == array.count, "freeValueArray did not clear the count");
    INVAR (0 == array.capacity, "freeValueArray did not clear the capacity");
    INVAR (NULL == array.values,
        "freeValueArray did not null the code pointer");
}
