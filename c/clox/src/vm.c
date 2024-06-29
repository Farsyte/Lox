#include "vm.h"

#include "chunk.h"

#include <stdio.h>

VM vm;

void
initVM (
    )
{
}

void
freeVM (
    )
{
}

static InterpretResult
run (
    )
{
#define READ_BYTE()     (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
    for (;;) {

        // Convert the byte to an OpCode enum value, so the C compiler
        // can warn us if there are any OpCode num values missing from
        // the switch.

        OpCode instruction;
        Value constant;

        switch (instruction = (OpCode) READ_BYTE ()) {

        case OP_CONSTANT:
            constant = READ_CONSTANT ();
            printValue (constant);
            printf ("\n");
            break;

        case OP_RETURN:
            return INTERPRET_OK;

        }
    }
#undef  READ_CONSTANT
#undef  READ_BYTE
}

InterpretResult
interpret (
    Chunk *chunk)
{
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return run ();
}
