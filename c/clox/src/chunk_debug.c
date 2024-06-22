#include "chunk_debug.h"

#include "value_debug.h"

#include <stdio.h>

static int simpleInstruction (
    const char *opname,
    int offset);

static int constantInstruction (
    const char *name,
    Chunk *chunk,
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

    if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1])
        printf ("   | ");
    else
        printf ("%4d ", chunk->lines[offset]);

    OpCode op = chunk->code[offset];

    switch (op) {
    case OP_CONSTANT:
        return constantInstruction ("OP_CONSTANT", chunk, offset);
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

static int
constantInstruction (
    const char *name,
    Chunk *chunk,
    int offset)
{
    int cix = (int) chunk->code[offset + 1];

    printf ("%-16s [%04d] '", name, cix);
    printValue (chunk->constants->values[cix]);
    printf ("'\n");
    return offset + 2;
}
