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
#include <time.h>

VM vm;

static bool vmInitialized = 0;

#ifdef  DEBUG_TRACE_EXECUTION
int _DEBUG_TRACE_EXECUTION = 500;
#endif

/** Native Function: clock()
 *
 * @param argCount number of parameters in the function call
 * @param args list of Values passed to the function
 * @returns Value containing seconds since an epoch
 */
static Value
clockNative (int argCount, Value *args)
{
    (void) argCount;                    // not used by this function
    (void) args;                        // not used by this function
#ifdef  DEBUG_TRACE_EXECUTION
    if (_DEBUG_TRACE_EXECUTION) {
        printf ("\n");
        fflush (stdout);
        fprintf (stderr, "clock() disabling DEBUG_TRACE_EXECUTION\n");
        _DEBUG_TRACE_EXECUTION = 0;
    }
#endif
    return NUMBER_VAL ((double) clock () / CLOCKS_PER_SEC);
}

/** Native Function: gc()
 *
 * This function triggers the garbage collector. It swallows all
 * parameters presented. In the future, the values passed to this
 * function may manipulate the GC.
 *
 * For now, gc() returns nil. In the future it may return
 * information relating to the GC.
 *
 * @param argCount number of parameters in the function call
 * @param args list of Values passed to the function
 * @returns Value containing seconds since an epoch
 */
static Value
gcNative (int argCount, Value *args)
{
    (void) argCount;                    // not used by this function
    (void) args;                        // not used by this function
    DEBUG_LINE (0, "TODO: trigger GC, please!");
    return NIL_VAL;
}

/** Reset the VM stack to empty.
 */
static void
resetStack ()
{
    vm.sp = vm.stack;
    vm.frameCount = 0;
    vm.openUpvalues = NULL;
}

/** Report a runtime error.
 *
 * @param format control the output,
 like printf.
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

    for (int i = vm.frameCount - 1; i >= 0; i--) {
        CallFrame *frame = &vm.frames[i];
        ObjFunction *function = frame->closure->function;
        size_t instruction = frame->ip - function->chunk.code - 1;

        fprintf (stderr, "[line %d] in ", function->chunk.lines[instruction]);
        if (function->name == NULL) {
            fprintf (stderr, "script\n");
        } else {
            fprintf (stderr, "%s()\n", function->name->chars);
        }

    }

    resetStack ();

    // ADDED MUCH LATER: Did I forget to set a "stop running" thing?

}

/** Define a Native Function
 */
static void
defineNative (const char *name, NativeFn function)
{
    push (OBJ_VAL (copyString (name, (int) strlen (name))));
    push (OBJ_VAL (newNative (function)));
    tableSet (&vm.globals, AS_STRING (vm.stack[0]), vm.stack[1]);
    pop ();
    pop ();
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
    INVAR (!vmInitialized, "refused, VM is already initialized.");
    vmInitialized = true;

    resetStack ();
    vm.objects = NULL;

    vm.grayCount = 0;
    vm.grayCapacity = 0;
    vm.grayStack = NULL;

#ifdef DEBUG_FREELESS_GC
    vm.unfree = NULL;
    vm.unfree_link = &vm.unfree;
#endif

    initTable (&vm.globals);
    initTable (&vm.strings);
    defineNative ("clock", clockNative);
    defineNative ("gc", gcNative);
}

/** Release storage owned by the VM.
 *
 * All memory allocated by the VM is released.
 */
void
freeVM ()
{
    INVAR (vmInitialized, "refused, VM is not initialized.");
    vmInitialized = false;

    freeTable (&vm.strings);
    freeTable (&vm.globals);
    freeObjects ();

#ifdef DEBUG_FREELESS_GC
    freeUnfree ();
#endif
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
    INVAR (vmInitialized, "refused, VM is not initialized.");
    INVAR (vm.sp < vm.stack + STACK_MAX, "STACK OVERFLOW");
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
    INVAR (vmInitialized, "refused, VM is not initialized.");
    INVAR (vm.sp > vm.stack, "STACK UNDERFLOW");
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
    INVAR (vmInitialized, "refused, VM is not initialized.");
    INVAR (vm.sp > vm.stack, "STACK UNDERFLOW");
    return vm.sp[-1 - distance];
}

/** Call a function.
 *
 * @param function what to call
 * @param argCount how many args are being sent
 * @returns false if the argument count is wrong
 * @returns false if the frame stack is full.
 * @returns true if the call was started
 */
static bool
call (ObjClosure *closure, int argCount)
{
    INVAR (vmInitialized, "refused, VM is not initialized.");

    if (argCount != closure->function->arity) {
        runtimeError ("Expected %d arguments but got %d.", closure->function->arity, argCount);
        return false;
    }

    if (vm.frameCount == FRAMES_MAX) {
        runtimeError ("Stack overflow.");
        return false;
    }

    CallFrame *frame = &vm.frames[vm.frameCount++];

    frame->closure = closure;
    frame->ip = closure->function->chunk.code;
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
    INVAR (vmInitialized, "refused, VM is not initialized.");

    if (IS_OBJ (callee)) {
        switch (OBJ_TYPE (callee)) {

        case OBJ_CLOSURE:
            return call (AS_CLOSURE (callee), argCount);

        case OBJ_FUNCTION:
            // return call (AS_FUNCTION (callee), argCount);
            UNREACHABLE ("obsoleted in 25.1.2");

        case OBJ_NATIVE:{
                NativeFn native = AS_NATIVE (callee)->function;
                Value result = native (argCount, vm.sp - argCount);

                vm.sp -= argCount + 1;
                push (result);
                return true;
            }

        case OBJ_STRING:
            break;

        case OBJ_UPVALUE:
            break;

            // yes, force me to look at this switch
            // any time a new Object Type is added.

        }
    }
    runtimeError ("Can only call functions and classes.");
    return false;
}

/** Capture an Upvalue
 *
 * @param local where the Upvalue value is stored
 * @returns an Upvalue object on the heap
 */
static ObjUpvalue *
captureUpvalue (Value *local)
{
    ObjUpvalue *prevUpvalue = NULL;
    ObjUpvalue *upvalue = vm.openUpvalues;

    while (upvalue != NULL && upvalue->location > local) {
        prevUpvalue = upvalue;
        upvalue = upvalue->next;
    }

    if (upvalue != NULL && upvalue->location == local) {
        return upvalue;
    }

    ObjUpvalue *createdUpvalue = newUpvalue (local);

    if (prevUpvalue == NULL) {
        vm.openUpvalues = createdUpvalue;
    } else {
        prevUpvalue->next = createdUpvalue;
    }

    return createdUpvalue;
}

/** Close every open upvalue that can be closed.
 *
 * @param last where to stop the stack scan
 */
static void
closeUpvalues (Value *last)
{
    while (vm.openUpvalues != NULL && vm.openUpvalues->location >= last) {
        ObjUpvalue *upvalue = vm.openUpvalues;

        upvalue->closed = *upvalue->location;
        upvalue->location = &upvalue->closed;
        vm.openUpvalues = upvalue->next;
    }
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
    INVAR (vmInitialized, "refused, VM is not initialized.");

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
    INVAR (vmInitialized, "refused, VM is not initialized.");

#ifdef  DEBUG_TRACE_EXECUTION
    printf ("\nExecuting ...\n");
#endif

    CallFrame *frame = &vm.frames[vm.frameCount - 1];

#define READ_BYTE()     (*frame->ip++)
#define READ_SHORT()    (frame->ip += 2, (uint16_t)((frame->ip[-2] << 8) | frame->ip[-1]))
#define READ_CONSTANT() (frame->closure->function->chunk.constants.values[READ_BYTE()])
#define READ_STRING()   (AS_STRING(READ_CONSTANT()))

    for (;;) {
#ifdef  DEBUG_TRACE_EXECUTION
        if (_DEBUG_TRACE_EXECUTION > 0) {
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
            disassembleInstruction (&frame->closure->function->chunk, (int) (frame->ip - frame->closure->function->chunk.code));
            _DEBUG_TRACE_EXECUTION--;
            if (!_DEBUG_TRACE_EXECUTION) {
                printf ("(no more debug traces after this)\n");
            }
        }
#endif
        // Convert the byte to an OpCode enum value, so the C compiler
        // can warn us if there are any OpCode num values missing from
        // the switch.

        OpCode instruction;
        ObjString *name;
        uint8_t slot;
        uint16_t offset;
        int argCount;
        ObjFunction *function;
        ObjClosure *closure;

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

        case OP_GET_UPVALUE:
            slot = READ_BYTE ();
            push (*frame->closure->upvalues[slot]->location);
            break;

        case OP_SET_UPVALUE:
            slot = READ_BYTE ();
            *frame->closure->upvalues[slot]->location = pop ();
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

        case OP_CLOSURE:
            function = AS_FUNCTION (READ_CONSTANT ());
            closure = newClosure (function);
            push (OBJ_VAL (closure));

            for (int i = 0; i < closure->upvalueCount; i++) {
                uint8_t isLocal = READ_BYTE ();
                uint8_t index = READ_BYTE ();

                if (isLocal) {
                    closure->upvalues[i] = captureUpvalue (frame->slots + index);
                } else {
                    closure->upvalues[i] = frame->closure->upvalues[index];
                }
            }

            break;

        case OP_CLOSE_UPVALUE:
            closeUpvalues (vm.sp - 1);
            pop ();
            break;

        case OP_RETURN:
            Value result = pop ();

            closeUpvalues (frame->slots);

            vm.frameCount--;
            if (vm.frameCount == 0) {
                // Exit interpreter.
                pop ();
#ifdef  DEBUG_TRACE_EXECUTION
                printf ("Executing ... done.\n\n");
#endif
                return INTERPRET_OK;
            }

            vm.sp = frame->slots;
            push (result);
            frame = &vm.frames[vm.frameCount - 1];
            break;
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
    INVAR (vmInitialized, "refused, VM is not initialized.");

    ObjFunction *function = compile (source);

    if (function == NULL)
        return INTERPRET_COMPILE_ERROR;

    push (OBJ_VAL (function));

    ObjClosure *closure = newClosure (function);

    pop ();
    push (OBJ_VAL (closure));
    call (closure, 0);
    return run ();
}
