#include "assert.h"             //
#include "chunk.h"
#include "post.h"

void
post_chunk (
    )
{

    assert (16 == sizeof (OpCode[16]),
        "OpCode lists need to be one byte per OpCode.");

    // Generally this equality would be "overtesting"
    // but I want to force the engineer to return to this code
    // whenever anything is added to or taken away from Chunk.

    Chunk *c;

    assert (40 == sizeof *c,
        "Expecting the base Chunk object to be 40 bytes.");

    assert (1 == sizeof *c->code,
        "Expecting the Chunk Code array entries to be 1 byte.");

    assert (4 == sizeof *c->lines,
        "Expecting the Line array entries to be 4 bytes.");

}
