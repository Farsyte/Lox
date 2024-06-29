#include "debug.h"

#include "chunk.h"
#include "value.h"

#include <stdio.h>

static int constantInstruction (
    const char *name,
    Chunk *chunk,
    int offset);

static int simpleInstruction (
    const char *name,
    int offset);

/** Disassemble the bytecodes in this chunk.
 *
 * Prints the name of the chunk, followed by each
 * bytecode stored in the chunk, disassembled.
 *
 * @param chunk where to get the data
 * @param name what to call it
 */
void
disassembleChunk (
    Chunk *chunk,
    const char *name)
{
    printf ("\nDisassembling %s ...\n", name);

    for (int offset = 0; offset < chunk->count;) {
        offset = disassembleInstruction (chunk, offset);
    }
    printf ("Disassembling %s ... done.\n\n", name);
}

/** Print disassembly of a simple constant OpCode.
 *
 * @param name string representing the instruction
 * @param chunk that provided the instruction
 * @param offset of the instruction bytecode in the chunk
 * @returns offset of the next instruction in the chunk
 */
static int
constantInstruction (
    const char *name,
    Chunk *chunk,
    int offset)
{
    uint8_t constant = chunk->code[offset + 1];

    printf ("%-16s %4d '", name, constant);
    printValue (chunk->constants.values[constant]);
    printf ("'\n");
    return offset + 2;
}

/** Print disassembly of a simple instruction OpCode.
 *
 * @param name string representing the instruction
 * @param offset of the instruction bytecode in the chunk
 * @returns offset of the next instruction in the chunk
 */
static int
simpleInstruction (
    const char *name,
    int offset)
{
    printf ("%s\n", name);
    return offset + 1;
}

/** Disassemble one bytecode from a chunk.
 *
 * @param chunk where to find the bytecode stream
 * @param which bytecode to disassemble
 * @return offset of next bytecode in this chunk
 */
int
disassembleInstruction (
    Chunk *chunk,
    int offset)
{
    printf ("%04d ", offset);

    uint8_t instruction = chunk->code[offset];

    // Casting instruction to type (OpCode) allows the
    // compiler to notify us if this switch statement
    // does not include a case for every enumerated value.
    //
    // Adding a "default" case would remove this protection.

    switch ((OpCode) instruction) {
    case OP_CONSTANT:
        return constantInstruction ("OP_CONSTANT", chunk, offset);
    case OP_RETURN:
        return simpleInstruction ("OP_RETURN", offset);
    }

    printf ("Unknown opcode %d\n", instruction);
    return offset + 1;
}
