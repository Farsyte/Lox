#include "common.h"
#include "debug.h"
#include "vm.h"

#include <stdio.h>

VM vm[1];

static InterpretResult run();
static void resetStack();

void initVM() {
    resetStack();
}

void freeVM() {
}

static void resetStack() {
    vm->stackTop = vm->stack;
}

void push(Value value) {
    *vm->stackTop = value;
    vm->stackTop++;
}

Value pop() {
    vm->stackTop--;
    return *vm->stackTop;
}

InterpretResult interpret(Chunk* chunk) {
    vm->chunk = chunk;
    vm->ip = vm->chunk->code;
    return run();
}

static InterpretResult run() {
#define READ_BYTE()	(*vm->ip++)
#define	READ_CONSTANT()	(vm->chunk->constants->values[READ_BYTE()])

    for (;;) {
#ifdef	DEBUG_TRACE_EXECUTION
	disassembleInstruction(
	    vm->chunk,
	    (int)(vm->ip - vm->chunk->code));
#endif

	Byte instruction;
	switch (instruction = READ_BYTE()) {

	case OP_CONSTANT: {
	    Value constant = READ_CONSTANT();
	    printValue(constant);
	    printf("\n");
	    break;
	}

	case OP_RETURN: {
	    return INTERPRET_OK;
	}

	}
    }
#undef	READ_BYTE
#undef	READ_CONSTANT
}
