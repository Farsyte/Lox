#include "vm.h"

#include "chunk.h"
#include "compiler.h"
#include "debug.h"

#include <math.h>
#include <stdarg.h>
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

/** Report a runtime error.
 *
 * @param format control the output, like printf.
 */
static void
runtimeError (
    const char *format,
    ...)
{
    va_list args;

    va_start (args, format);
    vfprintf (stderr, format, args);
    va_end (args);
    fputs ("\n", stderr);

    size_t instruction = vm.ip - vm.chunk->code - 1;
    int line = vm.chunk->lines[instruction];

    fprintf (stderr, "[line %d] in script\n", line);
    resetStack ();
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

/** Peek at a value from the VM stack.
 *
 * This returns a value from the VM stack,
 * without adjusting the stack pointer.
 * Calling with a distance of zero returns
 * a copy of the top element of the stack.
 * Calling with a distance of one returns
 * a copy of the element below it.
 * And so on.
 *
 * @param distance how far to dig down
 * @return the value from the stack.
 */
Value
peek (
    int distance)
{
    return vm.sp[-1 - distance];
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
            push (READ_CONSTANT ());
            break;

#define BINARY_OP(op)                           \
            do {                                                        \
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
            if (!IS_NUMBER (peek (0))) {
                runtimeError ("Operand must be a number.");
                return INTERPRET_RUNTIME_ERROR;
            }
            push (NUMBER_VAL (-AS_NUMBER (pop ())));
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
