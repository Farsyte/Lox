#include "opcode.h"

#include <assert.h>

void
post_opcode (
    )
{
    assert (16 == sizeof (OpCode[16]),
        "Expecting an array of 16 OpCodes to be 16 bytes long.");
}
