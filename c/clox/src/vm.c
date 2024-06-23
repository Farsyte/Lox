#include "vm.h"

#include "assert.h"
#include "chunk_debug.h"
#include "value_debug.h"

#include <stdio.h>

VM vm;

static InterpretResult run (
    );

void
initVM (
    )
{
    resetStack ();
}

void
freeVM (
    )
{
    free (vm.stack_base);
}

#define	MIN_STACK_SIZE	256

// Cap the stack size (or not, if #undef is used).
//
// Benefits to capping the size:
// - detect runaway recursion faster
//
// Benefits to not capping the size:
// - large stack-intensive code does not hit the limit

#define	MAX_STACK_SIZE	65536

void
resetStack (
    )
{
    size_t new_count = MIN_STACK_SIZE;
    size_t new_size = new_count * sizeof *vm.sp;

    vm.stack_base = malloc (new_size);
    assert (NULL != vm.stack_base, "VM: unable to allocate stack.");
    vm.stack_limit = vm.stack_base + new_count;
    vm.sp = vm.stack_base;
}

void
push (
    Value value)
{
    assert (vm.sp >= vm.stack_base, "stack underflow detected in push.");
    assert (vm.sp <= vm.stack_limit, "stack overflow detected in push.");
    if (vm.sp == vm.stack_limit) {
        size_t old_count = vm.stack_limit - vm.stack_base;
        size_t new_count = old_count * 10;
        size_t new_size = new_count * sizeof *vm.sp;

#ifdef MAX_STACK_SIZE
        assert (new_count <= MAX_STACK_SIZE,
            "VM: refusing to grow stack beyond MAX_STACK_SIZE.");
#endif

        vm.stack_base = realloc (vm.stack_base, new_size);
        assert (NULL != vm.stack_base, "VM: stack reallocation failed.");
        vm.stack_limit = vm.stack_base + new_count;

        vm.sp = vm.stack_base + old_count;
    }
    *vm.sp++ = value;
}

Value
pop (
    )
{
    assert (vm.sp >= vm.stack_base, "stack underflow detected in pop.");
    assert (vm.sp <= vm.stack_limit, "stack overflow detected in pop.");
    return *--vm.sp;
}

InterpretResult
interpret (
    Chunk *chunk)
{
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return run ();
}

static InterpretResult
run (
    )
{
    Value constant;

    // Book uses macros. I would reflexively use functions,
    // but it is a weak preference.

#define	READ_BYTE() (*vm.ip++)

#define READ_CONSTANT() (vm.chunk->constants->values[READ_BYTE()])

    // However, needing to fetch three bytes in order
    // needs sequence points, which means a statement
    // block. GCC Statement Expressions are the easy
    // way to make this happen.

#define READ_3BYTE()					\
    ({ /* GCC extension: Statement Expressions */	\
	int __i24 = READ_BYTE();			\
	__i24 |= READ_BYTE() << 8;			\
	__i24 |= READ_BYTE() << 16;			\
	__i24; })

#define READ_CONSTANT_LONG() (vm.chunk->constants->values[READ_3BYTE()])

#ifdef DEBUG_TRACE_EXECUTION
    printf ("interpreter: start at IP=%04ld\n", vm.ip - vm.chunk->code);
#endif

    for (;;) {

#ifdef DEBUG_TRACE_EXECUTION
        printf ("  stack:");
        for (Value *slot = vm.stack_base; slot < vm.sp; slot++) {
            printf (" ");
            printValue (*slot);
        }
        printf ("\n");
        disassembleInstruction (vm.chunk, vm.ip - vm.chunk->code);
#endif

        OpCode op = READ_BYTE ();

        switch (op) {

        case OP_CONSTANT:
            constant = READ_CONSTANT ();
            push (constant);
            break;

        case OP_CONSTANT_LONG:
            constant = READ_CONSTANT_LONG ();
            push (constant);
            break;

#define BINARY_OP(op)				\
	    do {				\
		Value b = pop();		\
		Value a = pop();		\
		Value r = a op b;		\
		push(r);			\
	    } while (0);

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

#undef BINARY_OP

        case OP_NEGATE:
            push (-pop ());
            break;

        case OP_RETURN:
#ifdef DEBUG_TRACE_EXECUTION
            printf ("interpreter: return at IP=%04ld\n",
                vm.ip - vm.chunk->code);
#endif
            printValue (pop ());
            printf ("\n");
            return INTERPRET_OK;
        }
    }
#undef READ_CONSTANT_LONG
#undef READ_3BYTE
#undef READ_CONSTANT
#undef READ_BYTE
}
