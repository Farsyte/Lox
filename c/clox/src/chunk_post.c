
#include "chunk.h"
#include "post.h"

int
post_chunk (
    )
{
    int exit_code = POST_PASS;

    POST_CHECK (16 == sizeof (OpCode[16]),
        "OpCode lists need to be one byte per OpCode.");

    return exit_code;
}
