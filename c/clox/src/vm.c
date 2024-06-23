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

}

void
freeVM (
    )
{

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
        disassembleInstruction (vm.chunk, vm.ip - vm.chunk->code);
#endif

        OpCode op = READ_BYTE ();

        switch (op) {

        case OP_CONSTANT:
            constant = READ_CONSTANT ();
            printValue (constant);
            printf ("\n");
            break;

        case OP_CONSTANT_LONG:
            constant = READ_CONSTANT_LONG ();
            printValue (constant);
            printf ("\n");
            break;

        case OP_RETURN:
#ifdef DEBUG_TRACE_EXECUTION
            printf ("interpreter: return at IP=%04ld\n",
                vm.ip - vm.chunk->code);
#endif
            return INTERPRET_OK;
        }
    }
#undef READ_CONSTANT_LONG
#undef READ_3BYTE
#undef READ_CONSTANT
#undef READ_BYTE
}
