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
    INVAR (IS_NUMBER (NUMBER_VAL (0)), "Value stores numbers");
    INVAR (IS_BOOL (BOOL_VAL (0)), "Value stores booleans");
    INVAR (IS_NIL (NIL_VAL (0)), "Value stores nil");

    INVAR (0 == AS_NUMBER (NUMBER_VAL (0)), "Value has zero");
    INVAR (1 == AS_NUMBER (NUMBER_VAL (1)), "Value has one");
    INVAR (0.5 == AS_NUMBER (NUMBER_VAL (0.5)), "Value has one-half");

    INVAR (false == AS_BOOL (BOOL_VAL (false)), "Value has zero");
    INVAR (true == AS_BOOL (BOOL_VAL (true)), "Value has one");

    ValueArray array;

    initValueArray (&array);
    INVAR (0 == array.count, "initValueArray did not clear the count");
    INVAR (0 == array.capacity, "initValueArray did not clear the capacity");
    INVAR (NULL == array.values,
        "initValueArray did not null the code pointer");

    for (int i = 0; i < 256; ++i)
        writeValueArray (&array, NUMBER_VAL (123.5 * i));
    writeValueArray (&array, NUMBER_VAL (1337.0));

    INVAR (257 == array.count, "writeValueArray did not update the count");
    INVAR (512 == array.capacity,
        "writeValueArray did not update the capacity");
    INVAR (NULL != array.values,
        "writeValueArray did not update the code pointer");

    for (int i = 0; i < 256; ++i)
        INVAR (IS_NUMBER (array.values[i]),
            "writeValueArray did not write the data as a number");
    for (int i = 0; i < 256; ++i)
        INVAR ((123.5 * i) == AS_NUMBER (array.values[i]),
            "writeValueArray did not write the data value");
    INVAR (IS_NUMBER (array.values[256]),
        "writeValueArray did not write the data is a number");
    INVAR (1337.0 == AS_NUMBER (array.values[256]),
        "writeValueArray did not write the data value");

    freeValueArray (&array);
    INVAR (0 == array.count, "freeValueArray did not clear the count");
    INVAR (0 == array.capacity, "freeValueArray did not clear the capacity");
    INVAR (NULL == array.values,
        "freeValueArray did not null the code pointer");
}
