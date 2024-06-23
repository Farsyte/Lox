#include "value.h"

#include "assert.h"

#include <stdio.h>

static bist_fn bist_value_value;
static bist_fn bist_value_array;

void
bist_value (
    )
{
    bist_value_value ();
    bist_value_array ();
}

void
bist_value_value (
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

void
bist_value_array (
    )
{
    ValueArray a[1];

    initValueArray (a);
    assert (NULL == a->values,
        "after initValueArray, values pointer must be NULL.");
    assert (0 == a->capacity, "after initValueArray, capacity must be zero.");
    assert (0 == a->count, "after initValueArray, count must be zero.");

    writeValueArray (a, 133.7);
    assert (1 == a->count, "after writeValueArray, count must be one.");
    assert (1 <= a->capacity,
        "after writeValueArray, capacity must be at least one.");
    assert (NULL != a->values,
        "after writeValueArray, values pointer must not be NULL.");
    assert (133.7 == a->values[0],
        "after writeValueArray, test value must be in place.");

    freeValueArray (a);
    assert (NULL == a->values,
        "after freeValueArray, values pointer must be NULL.");
    assert (0 == a->capacity, "after freeValueArray, capacity must be zero.");
    assert (0 == a->count, "after freeValueArray, count must be zero.");
}
