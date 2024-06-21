#include "chunk.h"

#include "assert.h"
#include "bist.h"
#include "post.h"

#include <stdio.h>

int
post_chunk (
    )
{
    int exit_code = 0;
    POST_CHECK_EQ_uint (16, sizeof (OpCode[16]),
        "OpCode lists need to be one byte per OpCode.");

    return exit_code;
}

int
bist_chunk (
    )
{
    return BIST_PASS;
}
