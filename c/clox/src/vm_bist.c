#include "vm.h"

#include "assert.h"
#include "bist.h"

// This test peeks at the VM state,
// because there is no other way
// for us to observe the results.
//
// yet.

extern VM vm;

void
bist_vm (
    )
{
    Chunk c[1];

    initChunk (c);

    // This does not exhaustively test all operations.

    writeConstant (c, 1.2, 123);
    writeConstant (c, 3.4, 123);
    writeChunk (c, OP_ADD, 126);

    // Force any additional constants to be encoded
    // using OP_CONSTANT_LONG.
    for (int i = 0; i < 999990; ++i)
        addConstant (c, (double) i);

    writeConstant (c, 5.6, 123);
    writeChunk (c, OP_DIVIDE, 126);
    writeChunk (c, OP_NEGATE, 126);

    writeChunk (c, OP_RETURN, 150);

    initVM ();
    interpret (c);

    assert (-(1.2 + 3.4) / 5.6 == *vm.sp, "The final value must be correct.");
    freeVM ();

    freeChunk (c);
}
