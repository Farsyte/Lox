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
        printf ("stack:");
        if (vm.sp > vm.stack) {
            for (Value *slot = vm.stack; slot < vm.sp; slot++) {
                printf (" ");
                printValue (*slot);
            }
            printf ("\n");
        } else {
            printf (" empty.\n");
        }
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
            push (constant);
            break;

#define BINARY_OP(op)                           \
            do {                                \
                double b = pop();               \
                double a = pop();               \
                push (a op b);                  \
            } while (false)

        case OP_ADD:
            BINARY_OP (+);
            break;
        case OP_SUBTRACT:
            BINARY_OP (-);
            break;
        case OP_MULTIPLY:
            BINARY_OP (*);
            break;
        case OP_DIVIDE:
            BINARY_OP (/);
            break;

#undef  BINARY_OP

        case OP_NEGATE:
            push (-pop ());
            break;

        case OP_RETURN:
            printValue (pop ());
            printf ("\n");
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
