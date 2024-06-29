#include "chunk.h"

#include <stdio.h>

void
post_chunk (
    )
{
    assert (16 == sizeof (OpCode[16]),
        "Chunk needs to pack one OpCode per byte.");
}
