#include "post.h"

#include "chunk.h"

int
post_all (
    )
{
    int exit_status = POST_PASS;
    exit_status |= post_chunk ();
    return exit_status;
}
