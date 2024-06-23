#include "vm.h"

#include "assert.h"
#include "bist.h"

void
bist_vm (
    )
{
    Chunk c[1];
    Value tv1 = 133.7;
    Value tv2 = 420.69;

    initChunk (c);
    writeConstant (c, tv1, 123);
    for (int i = 0; i < 999990; ++i)
        addConstant (c, (double) i);
    writeConstant (c, tv2, 126);
    writeChunk (c, OP_NEGATE, 126);
    writeChunk (c, OP_RETURN, 126);

    initVM ();

    // TODO: how do we capture the results of interpret
    // so this is a test and not just a demo?

    interpret (c);

    freeVM ();

    freeChunk (c);

}
