#include "vm.h"

#include "chunk.h"
#include "compiler.h"
#include "debug.h"
#include "memory.h"
#include "object.h"

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

VM vm;

/** Reset the VM stack to empty.
 */
static void
resetStack ()
{
    vm.sp = vm.stack;
}

/** Report a runtime error.
 *
 * @param format control the output, like printf.
 */
static void
runtimeError (const char *format, ...)
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
initVM ()
{
    vm.chunk = NULL;
    vm.ip = NULL;
    resetStack ();
    vm.objects = NULL;
    initTable (&vm.strings);
}

/** Release storage owned by the VM.
 *
 * All memory allocated by the VM is released.
 */
void
freeVM ()
{
    freeTable (&vm.strings);
    freeObjects ();
}

/** Push a value onto the VM stack.
 *
 * This adjusts the stack pointer so that several
 * values pushed do not overwrite each other.
 *
 * @warning no protection from stack overflow.
 *
 * @param value data to be pushed.
 */
void
push (Value value)
{
    *vm.sp = value;
    vm.sp++;
}

/** Consume a value from the VM stack.
 *
 * This adjusts the stack pointer so that
 * the value returned is no longer on the stack.
 *
 * @warning no protection from stack underflow.
 *
 * @returns the value from the stack.
 */
Value
pop ()
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
 * @warning no protection from stack underflow.
 *
 * @param distance how far to dig down
 * @returns the value from the stack.
 */
Value
peek (int distance)
{
    return vm.sp[-1 - distance];
}

/** Return true if the value is falsey.
 *
 * Follows the RUBY convention that nil and false are falsey
 * and everything else (including 0 and 0.0) is true.
 *
 * @param value what to check
 * @returns true if input is false or nil
 * @returns false otherwise.
 */
static bool
isFalsey (Value value)
{
    return IS_NIL (value) || (IS_BOOL (value) && !AS_BOOL (value));
}

/** Concatenate two strings on the stack
 */
static void
concatenate ()
{
    ObjString *b = AS_STRING (pop ());
    ObjString *a = AS_STRING (pop ());
    int length = a->length + b->length;
    char *chars = ALLOCATE (char, length + 1);

    memcpy (chars, a->chars, a->length);
    memcpy (chars + a->length, b->chars, b->length);
    chars[length] = '\0';

    ObjString *result = takeString (chars, length);

    push (OBJ_VAL (result));

}

/** Run the bytecodes in the VM.
 *
 * This function steps through the bytecode, interpreting
 * each per the bytecode definition, which may include
 * consuming inline values, or jumping around.
 *
 * @returns a code indicating which failure, if there was one
 * @returns otherwise, a code indicating success
 */
static InterpretResult
run ()
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

        case OP_NIL:
            push (NIL_VAL);
            break;
        case OP_TRUE:
            push (BOOL_VAL (true));
            break;
        case OP_FALSE:
            push (BOOL_VAL (false));
            break;
        case OP_POP:
            (void) pop ();
            break;

#define BINARY_OP(valueType, op)                                        \
            do {                                                        \
                if (!IS_NUMBER(peek(0)) || (!IS_NUMBER(peek(1)))) {     \
                    runtimeError("Operands must be numbers.");          \
                    return INTERPRET_RUNTIME_ERROR;                     \
                }                                                       \
                Value b = pop();                                        \
                Value a = pop();                                        \
                push(valueType(AS_NUMBER(a) op AS_NUMBER(b)));          \
            } while (false)

            // *INDENT-OFF*

        case OP_ADD: {
            if (IS_STRING(peek(0)) && IS_STRING(peek(1))) {
                concatenate();
            } else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))) {
                double b = AS_NUMBER(pop());
                double a = AS_NUMBER(pop());
                push(NUMBER_VAL(a + b));
            } else {
                runtimeError("Operands must be two numbers or two strings.");
            }
            break;
        }

        case OP_SUBTRACT: BINARY_OP (NUMBER_VAL, -); break;
        case OP_MULTIPLY: BINARY_OP (NUMBER_VAL, *); break;
        case OP_DIVIDE:   BINARY_OP (NUMBER_VAL, /); break;

        case OP_GREATER:  BINARY_OP (BOOL_VAL,   >); break;
        case OP_LESS:     BINARY_OP (BOOL_VAL,   <); break;

            // *INDENT-ON*

#undef  BINARY_OP

        case OP_EQUAL:{
                Value b = pop ();
                Value a = pop ();

                push (BOOL_VAL (valuesEqual (a, b)));
                break;
            }

        case OP_NOT:
            push (BOOL_VAL (isFalsey (pop ())));
            break;

        case OP_NEGATE:
            if (!IS_NUMBER (peek (0))) {
                runtimeError ("Operand must be a number.");
                return INTERPRET_RUNTIME_ERROR;
            }
            push (NUMBER_VAL (-AS_NUMBER (pop ())));
            break;

        case OP_PRINT:
            printValue (pop ());
            printf ("\n");
            break;

        case OP_RETURN:
            // Exit interpreter.
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
 * @returns a code indicating a failure code if a failure occurs
 * @returns a code indicating success if all went well
 */
InterpretResult
interpretChunk (Chunk *chunk)
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
 * @returns a code indicating a failure code if a failure occurs
 * @returns a code indicating success if all went well
 */
InterpretResult
interpret (const char *source)
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
