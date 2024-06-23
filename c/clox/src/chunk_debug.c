#include "chunk_debug.h"

#include "assert.h"
#include "value_debug.h"

#include <stdio.h>

static int simpleInstruction (
    const char *opname,
    int offset);

static int constantInstruction (
    const char *name,
    Chunk *chunk,
    int offset);

static int constantLongInstruction (
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

    int prev = getLine (chunk->iline, offset - 1);
    int line = getLine (chunk->iline, offset);

    if (offset > 0 && line == prev)
        printf ("   | ");
    else
        printf ("%4d ", line);

    OpCode op = chunk->code[offset];

    switch (op) {
    case OP_CONSTANT:
        return constantInstruction ("OP_CONSTANT", chunk, offset);
    case OP_CONSTANT_LONG:
        return constantLongInstruction ("OP_CONSTANT_LONG", chunk, offset);

    case OP_ADD:
        return simpleInstruction ("OP_ADD", offset);
    case OP_SUBTRACT:
        return simpleInstruction ("OP_SUBTRACT", offset);
    case OP_MULTIPLY:
        return simpleInstruction ("OP_MULTIPLY", offset);
    case OP_DIVIDE:
        return simpleInstruction ("OP_DIVIDE", offset);

    case OP_NEGATE:
        return simpleInstruction ("OP_NEGATE", offset);
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

static int
constantLongInstruction (
    const char *name,
    Chunk *chunk,
    int offset)
{
    int cix = (int) chunk->code[offset + 1];

    cix += 256 * (int) chunk->code[offset + 2];
    cix += 65536 * (int) chunk->code[offset + 3];

    printf ("%-16s [%04d] '", name, cix);
    printValue (chunk->constants->values[cix]);
    printf ("'\n");
    return offset + 4;
}
