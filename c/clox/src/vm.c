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
    vm.frameCount = 0;
}

/** Report a runtime error.
 *
 * @param format control the output, like printf.
 */
static void
runtimeError (const char *format, ...)
{
    va_list args;

    fprintf (stderr, "\nRUNTIME ERROR: ");
    va_start (args, format);
    vfprintf (stderr, format, args);
    va_end (args);
    fputs ("\n", stderr);

    CallFrame *frame = &vm.frames[vm.frameCount - 1];
    size_t instruction = frame->ip - frame->function->chunk.code - 1;
    int line = frame->function->chunk.lines[instruction];

    fprintf (stderr, "[line %d] in script\n", line);
    resetStack ();

    // ADDED MUCH LATER: Did I forget to set a "stop running" thing?

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
    resetStack ();
    vm.objects = NULL;
    initTable (&vm.globals);
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
    freeTable (&vm.globals);
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

/** Call a function.
 */
static bool
call (ObjFunction *function, int argCount)
{
    CallFrame *frame = &vm.frames[vm.frameCount++];

    frame->function = function;
    frame->ip = function->chunk.code;
    frame->slots = vm.sp - argCount - 1;
    return true;
}

/** Execute the CALL operation.
 *
 * @param callee the functino to call
 * @param argCount the number of arguments being passed
 * @returns true if the call can be made, otherwise
 * @returns false if the callee is not a function or a class
 */
static bool
callValue (Value callee, int argCount)
{
    if (IS_OBJ (callee)) {
        switch (OBJ_TYPE (callee)) {
        case OBJ_FUNCTION:
            return call (AS_FUNCTION (callee), argCount);
        case OBJ_STRING:
            break;
            // yes, force me to look at this switch
            // any time a new Object Type is added.
        }
    }
    runtimeError ("Can only call functions and classes.");
    return false;
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

    CallFrame *frame = &vm.frames[vm.frameCount - 1];

#define READ_BYTE()     (*frame->ip++)
#define READ_SHORT()    (frame->ip += 2, (uint16_t)((frame->ip[-2] << 8) | frame->ip[-1]))
#define READ_CONSTANT() (frame->function->chunk.constants.values[READ_BYTE()])
#define READ_STRING()   (AS_STRING(READ_CONSTANT()))

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
        disassembleInstruction (&frame->function->chunk, (int) (frame->ip - frame->function->chunk.code));
#endif
        // Convert the byte to an OpCode enum value, so the C compiler
        // can warn us if there are any OpCode num values missing from
        // the switch.

        OpCode instruction;
        ObjString *name;
        uint8_t slot;
        uint16_t offset;
        int argCount;

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

        case OP_GET_LOCAL:
            slot = READ_BYTE ();
            push (frame->slots[slot]);
            break;

        case OP_GET_GLOBAL:
            name = READ_STRING ();
            Value value;

            if (!tableGet (&vm.globals, name, &value)) {
                runtimeError ("Undefined variable '%s'.", name->chars);
                return INTERPRET_RUNTIME_ERROR;
            }
            push (value);
            break;

        case OP_DEFINE_GLOBAL:
            name = READ_STRING ();

            tableSet (&vm.globals, name, peek (0));
            pop ();
            break;

        case OP_SET_LOCAL:
            slot = READ_BYTE ();
            frame->slots[slot] = peek (0);
            break;

        case OP_SET_GLOBAL:
            name = READ_STRING ();
            if (tableSet (&vm.globals, name, peek (0))) {
                tableDelete (&vm.globals, name);
                runtimeError ("Undefined variable '%s'.", name->chars);
                return INTERPRET_RUNTIME_ERROR;
            }
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

        case OP_JUMP:
            offset = READ_SHORT ();
            frame->ip += offset;
            break;

        case OP_JUMP_IF_FALSE:
            offset = READ_SHORT ();
            if (isFalsey (peek (0)))
                frame->ip += offset;
            break;

        case OP_LOOP:
            offset = READ_SHORT ();
            frame->ip -= offset;
            break;

        case OP_CALL:
            argCount = READ_BYTE ();
            if (!callValue (peek (argCount), argCount)) {
                return INTERPRET_RUNTIME_ERROR;
            }
            frame = &vm.frames[vm.frameCount - 1];
            break;

        case OP_RETURN:
            // Exit interpreter.
#ifdef  DEBUG_TRACE_EXECUTION
            printf ("Executing ... done.\n\n");
#endif
            return INTERPRET_OK;

        }
    }
#undef  READ_SHORT
#undef  READ_CONSTANT
#undef  READ_STRING
#undef  READ_BYTE
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
    ObjFunction *function = compile (source);

    if (function == NULL)
        return INTERPRET_COMPILE_ERROR;

    push (OBJ_VAL (function));
    call (function, 0);
    return run ();
}
