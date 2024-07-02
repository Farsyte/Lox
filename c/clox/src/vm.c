#include "vm.h"

#include "chunk.h"
#include "compiler.h"
#include "debug.h"

#include <math.h>
#include <stdio.h>

VM vm;

/** Reset the VM stack to empty.
 */
static void
resetStack (
    )
{
    vm.sp = vm.stack;
}

/** Initialize the VM completely.
 *
 * After this call, the VM does not have a reference
 * to a source chunk, or a valid IP. It also has an
 * empty stack.
 */
void
initVM (
    )
{
    vm.chunk = NULL;
    vm.ip = NULL;
    resetStack ();
}

/** Release storage owned by the VM.
 *
 * All memory allocated by the VM is released.
 */
void
freeVM (
    )
{
}

/** Push a value onto the VM stack.
 *
 * This adjusts the stack pointer so that several
 * values pushed do not overwrite each other.
 *
 * @param value data to be pushed.
 */
void
push (
    Value value)
{
    *vm.sp = value;
    vm.sp++;
}

/** Consume a value from the VM stack.
 *
 * This adjusts the stack pointer so that
 * the value returned is no longer on the stack.
 *
 * @return the value from the stack.
 */
Value
pop (
    )
{
    vm.sp--;
    return *vm.sp;
}

/** Run the bytecodes in the VM.
 *
 * This function steps through the bytecode, interpreting
 * each per the bytecode definition, which may include
 * consuming inline values, or jumping around.
 *
 * @return a code indicating success or (which) failure.
 */
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

/** Interpret a chunk using the VM.
 *
 * Loads the chunk into the VM, setting the IP
 * to the start of the bytecode list, and runs it.
 *
 * @param chunk contains the bytecode sequence
 * @return a code indicating success or (which) failure.
 */
InterpretResult
interpretChunk (
    Chunk *chunk)
{
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return run ();
}

/** Interpret source code using the VM.
 *
 * Compile the source code into a chunk of bytecode,
 * then interpret the bytecode.
 *
 * @param source contains Lux source code
 * @return a code indicating success or (which) failure.
 */
InterpretResult
interpret (
    const char *source)
{
    Chunk chunk;

    initChunk (&chunk);

    if (!compile (source, &chunk)) {
        freeChunk (&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run ();

    freeChunk (&chunk);
    return result;
}
