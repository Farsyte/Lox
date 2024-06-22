#include "assert.h"
#include "value.h"

void
post_value (
    )
{
    assert (8 == sizeof (Value), "Value must be a 64 bit type.");
    Value val = 1.0;

    val /= 2.0;
    assert (0.5 == val, "Value does not appear to be floating point.");
}
