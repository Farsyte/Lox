#include "post.h"

#include "chunk.h"
#include "iline.h"
#include "value.h"
#include "vm.h"

#include <stdlib.h>

typedef post_fn *post_fnp;

void
post_all (
    )
{
    post_fnp post_list[] = {
        post_chunk,
        post_value,
        post_iline,
        post_vm,
        // add more tets here.
    };

    post_fnp *post_last =
        post_list + (sizeof post_list / sizeof post_list[0]);

    for (post_fnp * ent = post_list; ent < post_last; ++ent)
        (*ent) ();
}
