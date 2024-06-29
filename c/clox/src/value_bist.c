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
}
