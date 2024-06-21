
#include "bist.h"
#include "chunk.h"

int
bist_chunk (
    )
{
    int exit_code = BIST_PASS;

    BIST_CHECK (sizeof (Chunk) <= 8,
        "Chunk objects are larger than expected");

    return exit_code;
}
