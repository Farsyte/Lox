#include "bist.h"

#include "chunk.h"
#include "value.h"

#include <stdlib.h>

typedef bist_fn *bist_fnp;

void
bist_all (
    )
{
    bist_fnp bist_list[] = {
        bist_chunk,
        bist_value,
        // add more tets here.
    };

    bist_fnp *bist_last =
        bist_list + (sizeof bist_list / sizeof bist_list[0]);

    for (bist_fnp * ent = bist_list; ent < bist_last; ++ent)
        (*ent) ();
    exit (EXIT_SUCCESS);
}
