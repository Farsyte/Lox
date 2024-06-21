#include "post.h"

#include "chunk.h"

int
post_all (
    )
{
    int exit_status = 0;
    exit_status |= post_chunk ();
    return exit_status;
}
