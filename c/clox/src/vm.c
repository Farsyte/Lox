#include "vm.h"

#include "chunk.h"
#include "debug.h"

#include <stdio.h>

VM vm;

static void
resetStack (
    )
{
    vm.sp = vm.stack;
}

void
initVM (
    )
{
    vm.chunk = NULL;
    vm.ip = NULL;
    resetStack ();
}

void
freeVM (
    )
{
}

void
push (
    Value value)
{
    *vm.sp = value;
    vm.sp++;
}

Value
pop (
    )
{
    vm.sp--;
    return *vm.sp;
}

static InterpretResult
run (
    )
{
#ifdef  DEBUG_TRACE_EXECUTION
    printf ("\nExecuting ...\n");
#endif

#define READ_BYTE()     (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
    for (;;) {
#ifdef  DEBUG_TRACE_EXECUTION
        disassembleInstruction (vm.chunk, (int) (vm.ip - vm.chunk->code));
#endif
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
#ifdef  DEBUG_TRACE_EXECUTION
            printf ("Executing ... done.\n\n");
#endif
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
