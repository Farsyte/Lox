#include "vm.h"

#include "chunk.h"
#include "compiler.h"
#include "debug.h"

#include <math.h>
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
    Value tos = (vm.sp > vm.stack) ? pop () : NAN;

#ifdef  DEBUG_TRACE_EXECUTION
    printf ("\nExecuting ...\n");
#endif

#define READ_BYTE()     (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
    for (;;) {
#ifdef  DEBUG_TRACE_EXECUTION
        printf ("stack:");
        if (isfinite (tos)) {
            // NOTE: vm.stack[0] will have the original NAN value.
            for (Value *slot = vm.stack + 1; slot < vm.sp; slot++) {
                printf (" ");
                printValue (*slot);
            }
            printf (" ");
            printValue (tos);
        } else {
            printf (" empty.");
        }
        printf ("\n");
        disassembleInstruction (vm.chunk, (int) (vm.ip - vm.chunk->code));
#endif
        // Convert the byte to an OpCode enum value, so the C compiler
        // can warn us if there are any OpCode num values missing from
        // the switch.

        OpCode instruction;

        switch (instruction = (OpCode) READ_BYTE ()) {

        case OP_CONSTANT:
            push (tos);
            tos = READ_CONSTANT ();
            // TODO runtime error if result is not finite
            break;

#define BINARY_OP(op)                           \
            do {                                \
                double a = pop();               \
                tos = a op tos;                 \
            } while (false)

        case OP_ADD:
            BINARY_OP (+);
            // TODO runtime error if result is not finite
            break;
        case OP_SUBTRACT:
            BINARY_OP (-);
            // TODO runtime error if result is not finite
            break;
        case OP_MULTIPLY:
            BINARY_OP (*);
            // TODO runtime error if result is not finite
            break;
        case OP_DIVIDE:
            // TODO runtime error if tos is zero
            BINARY_OP (/);
            // TODO runtime error if result is not finite
            break;

#undef  BINARY_OP

        case OP_NEGATE:
            tos = -tos;
            break;

        case OP_RETURN:
            if (vm.sp > vm.stack || isfinite (tos)) {
                printValue (tos);
                // leave the stack looking like tos was just popped.
                *vm.sp = tos;
            } else {
                printf ("OP_RETURN but stack is empty.");
            }
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
interpretChunk (
    Chunk *chunk)
{
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return run ();
}

InterpretResult
interpret (
    const char *source)
{
    compile (source);
    return INTERPRET_OK;
}
