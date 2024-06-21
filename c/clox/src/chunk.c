#include "chunk.h"

#include "assert.h"
#include "bist.h"
#include "post.h"

#include <stdio.h>
#include <stdlib.h>

int
post_chunk (
    )
{
    int exit_code = POST_PASS;

    POST_CHECK_EQ_uint (16, sizeof (OpCode[16]),
        "OpCode lists need to be one byte per OpCode.");

    return exit_code;
}

int
bist_chunk (
    )
{
    int exit_code = BIST_PASS;

    BIST_CHECK (sizeof (Chunk) <= 8,
        "Chunk objects are larger than expected");

    return exit_code;;
}
