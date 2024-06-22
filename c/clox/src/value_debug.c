#include "value_debug.h"

#include <math.h>
#include <stdio.h>

void
printValue (
    Value value)
{
    int v = (int) round (value);

    if (value == (double) v)
        printf ("%d", v);
    else
        printf ("%g", value);
}
