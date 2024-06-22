#include "post.h"

#include "chunk.h"

#include <stdlib.h>

typedef post_fn *post_fnp;

void
post_all (
    )
{
    post_fnp post_list[] = {
        post_chunk,
        // add more tets here.
    };

    post_fnp *post_last =
        post_list + (sizeof post_list / sizeof post_list[0]);

    for (post_fnp * ent = post_list; ent < post_last; ++ent)
        (*ent) ();
}
