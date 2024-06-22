#include "chunk_debug.h"

#include <stdio.h>

static int simpleInstruction (
    const char *opname,
    int offset);

void
disassembleChunk (
    Chunk *chunk,
    const char *name)
{
    printf ("== %s ==\n", name);

    int offset = 0;

    while (offset < chunk->count) {
        offset = disassembleInstruction (chunk, offset);
    }
}

int
disassembleInstruction (
    Chunk *chunk,
    int offset)
{
    printf ("%04d ", offset);

    OpCode op = chunk->code[offset];

    switch (op) {
    case OP_RETURN:
        return simpleInstruction ("OP_RETURN", offset);
    }
    printf ("unknown opcode %d\n", op);
    return offset + 1;
}

static int
simpleInstruction (
    const char *name,
    int offset)
{
    printf ("%s\n", name);
    return offset + 1;
}
