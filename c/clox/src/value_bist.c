#include "assert.h"
#include "value.h"

#include <stdio.h>

void
bist_value (
    )
{
    Value val;

    val = 1.0;

    for (;;) {
        Value half = val / 2.0;

        if (half == 0)
            break;
        val = half;
    }

    assert (val <= 5.0e-324, "Value is unable to contain tiny values.");

    val = 1.0;

    for (;;) {
        Value dbl = val * 2.0;

        if (dbl == dbl * 2.0)
            break;
        val = dbl;
    }

    assert (val >= 8.0e+307, "Value is unable to contain huge values.");

    val = 1.0;

    for (;;) {
        Value half = val / 2.0;
        Value sum = 1.0 + half;

        if (sum == 1.0) {
            break;
        }
        val = half;
    }

    // IEEE 754 32-bit floating point: 1.19209e-7
    // IEEE 754 64-bit floating point: 2.22045e-16
    assert (val <= 2.25e-16, "Value 1.0 not stored with enough precision.");
}
