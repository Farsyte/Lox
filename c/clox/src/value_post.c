#include "value.h"

#include "assert.h"

void
post_value (
    )
{
    assert (8 == sizeof (Value), "Value must be a 64 bit type.");
    Value val = 1.0;

    val /= 2.0;
    assert (0.5 == val, "Value does not appear to be floating point.");

    ValueArray *a;

    assert (16 == sizeof *a, "Expecting ValueArray to be 16 bytes.");

    assert (8 == sizeof a->values[0],
        "Expecting ValueArray.values[0] size to be eight bytes.");

}
