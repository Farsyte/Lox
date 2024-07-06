#include "debug.h"

#include "chunk.h"
#include "value.h"

#include <stdio.h>

/** Convert TokenType enum value into string.
 *
 * If the provided value is not a TokenType value, return a name
 * formatted in a static buffer, which will be overwritten on the next
 * call that needs it.
 *
 * @param type value from the TokenType enumeration
 * @returns the name of the token type, if type is in the enumeration.
 * @returns a static buffer with a synthesized type string, otherwise.
 */
const char *
tokenTypeString (TokenType type)
{

    // The compiler will inform us if a TokenType value is
    // not in the switch, as long as we remember,
    //     DO NOT ADD A "default:" CASE.

    switch (type) {

        // *INDENT-OFF*

        // Single-character tokens.
    case TOKEN_LEFT_PAREN:              return "TOKEN_LEFT_PAREN";
    case TOKEN_RIGHT_PAREN:             return "TOKEN_RIGHT_PAREN";
    case TOKEN_LEFT_BRACE:              return "TOKEN_LEFT_BRACE";
    case TOKEN_RIGHT_BRACE:             return "TOKEN_RIGHT_BRACE";
    case TOKEN_COMMA:                   return "TOKEN_COMMA";
    case TOKEN_DOT:                     return "TOKEN_DOT";
    case TOKEN_MINUS:                   return "TOKEN_MINUS";
    case TOKEN_PLUS:                    return "TOKEN_PLUS";
    case TOKEN_SEMICOLON:               return "TOKEN_SEMICOLON";
    case TOKEN_SLASH:                   return "TOKEN_SLASH";
    case TOKEN_STAR:                    return "TOKEN_STAR";

        // One or two charcter tokens.
    case TOKEN_BANG:                    return "TOKEN_BANG";
    case TOKEN_BANG_EQUAL:              return "TOKEN_BANG_EQUAL";
    case TOKEN_EQUAL:                   return "TOKEN_EQUAL";
    case TOKEN_EQUAL_EQUAL:             return "TOKEN_EQUAL_EQUAL";
    case TOKEN_GREATER:                 return "TOKEN_GREATER";
    case TOKEN_GREATER_EQUAL:           return "TOKEN_GREATER_EQUAL";
    case TOKEN_LESS:                    return "TOKEN_LESS";
    case TOKEN_LESS_EQUAL:              return "TOKEN_LESS_EQUAL";

        // Literals.
    case TOKEN_IDENTIFIER:              return "TOKEN_IDENTIFIER";
    case TOKEN_STRING:                  return "TOKEN_STRING";
    case TOKEN_NUMBER:                  return "TOKEN_NUMBER";

        // Keywords
    case TOKEN_AND:                     return "TOKEN_AND";
    case TOKEN_CLASS:                   return "TOKEN_CLASS";
    case TOKEN_ELSE:                    return "TOKEN_ELSE";
    case TOKEN_FALSE:                   return "TOKEN_FALSE";
    case TOKEN_FOR:                     return "TOKEN_FOR";
    case TOKEN_FUN:                     return "TOKEN_FUN";
    case TOKEN_IF:                      return "TOKEN_IF";
    case TOKEN_NIL:                     return "TOKEN_NIL";
    case TOKEN_OR:                      return "TOKEN_OR";
    case TOKEN_PRINT:                   return "TOKEN_PRINT";
    case TOKEN_RETURN:                  return "TOKEN_RETURN";
    case TOKEN_SUPER:                   return "TOKEN_SUPER";
    case TOKEN_THIS:                    return "TOKEN_THIS";
    case TOKEN_TRUE:                    return "TOKEN_TRUE";
    case TOKEN_VAR:                     return "TOKEN_VAR";
    case TOKEN_WHILE:                   return "TOKEN_WHILE";

    case TOKEN_ERROR:                   return "TOKEN_ERROR";
    case TOKEN_EOF:                     return "TOKEN_EOF";

        // *INDENT-ON*
    }

    static char ttStaticName[32];

    snprintf (ttStaticName, sizeof ttStaticName, "<TokenType=%d>", (int) type);
    return ttStaticName;
}

/** Print disassembly of a simple constant OpCode.
 *
 * @param name string representing the instruction
 * @param chunk that provided the instruction
 * @param offset of the instruction bytecode in the chunk
 * @returns offset of the next instruction in the chunk
 */
static int
constantInstruction (const char *name, Chunk *chunk, int offset)
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
simpleInstruction (const char *name, int offset)
{
    printf ("%s\n", name);
    return offset + 1;
}

/** Print disassembly of a byte instruction OpCode.
 *
 * @param name string representing the instruction
 * @param chunk wheret o find the bytecodes
 * @param offset of the instruction bytecode in the chunk
 * @returns offset of the next instruction in the chunk
 */
static int
byteInstruction (const char *name, Chunk *chunk, int offset)
{
    uint8_t slot = chunk->code[offset + 1];

    printf ("%-16s %4d\n", name, slot);
    return offset + 2;
}

/** Print disassembly of a jump instruction OpCode.
 *
 * @param name string representing the instruction
 * @param sign direction (forward or backward)
 * @param chunk where to find the bytecodes
 * @param offset of the instruction bytecode in the chunk
 * @returns offset of the next instruction in the chunk
 */
static int
jumpInstruction (const char *name, int sign, Chunk *chunk, int offset)
{
    uint16_t jump = (uint16_t) (chunk->code[offset + 1] << 8);

    jump |= (uint16_t) (chunk->code[offset + 2]);

    printf ("%-16s %4d -> %d\n", name, offset, offset + 3 + sign * jump);
    return offset + 3;
}

/** Disassemble one bytecode from a chunk.
 *
 * @param chunk where to find the bytecode stream
 * @param offset where in the chunk to do work
 * @returns offset of next bytecode in this chunk
 */
int
disassembleInstruction (Chunk *chunk, int offset)
{
    printf ("%04d ", offset);

    if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {
        printf ("   | ");
    } else {
        printf ("%4d ", chunk->lines[offset]);
    }

    uint8_t instruction = chunk->code[offset];

    // The compiler will inform us if an OpCode value is
    // not in the switch, as long as we remember,
    //     DO NOT ADD A "default:" CASE.

    switch ((OpCode) instruction) {

        // *INDENT-OFF*

    case OP_CONSTANT:                   return constantInstruction ("OP_CONSTANT", chunk, offset);

    case OP_NIL:                        return simpleInstruction ("OP_NIL", offset);
    case OP_TRUE:                       return simpleInstruction ("OP_TRUE", offset);
    case OP_FALSE:                      return simpleInstruction ("OP_FALSE", offset);
    case OP_POP:                        return simpleInstruction ("OP_POP", offset);
    case OP_GET_LOCAL:                  return byteInstruction ("OP_GET_LOCAL", chunk, offset);
    case OP_SET_LOCAL:                  return byteInstruction ("OP_SET_LOCAL", chunk, offset);
    case OP_GET_GLOBAL:                 return constantInstruction ("OP_GET_GLOBAL", chunk, offset);
    case OP_DEFINE_GLOBAL:              return constantInstruction ("OP_DEFINE_GLOBAL", chunk, offset);
    case OP_SET_GLOBAL:                 return constantInstruction ("OP_SET_GLOBAL", chunk, offset);

    case OP_ADD:                        return simpleInstruction ("OP_ADD", offset);
    case OP_SUBTRACT:                   return simpleInstruction ("OP_SUBTRACT", offset);
    case OP_MULTIPLY:                   return simpleInstruction ("OP_MULTIPLY", offset);
    case OP_DIVIDE:                     return simpleInstruction ("OP_DIVIDE", offset);

    case OP_EQUAL:                      return simpleInstruction ("OP_EQUAL", offset);
    case OP_GREATER:                    return simpleInstruction ("OP_GREATER", offset);
    case OP_LESS:                       return simpleInstruction ("OP_LESS", offset);

    case OP_NOT:                        return simpleInstruction ("OP_NOT", offset);
    case OP_NEGATE:                     return simpleInstruction ("OP_NEGATE", offset);
    case OP_PRINT:                      return simpleInstruction ("OP_PRINT", offset);
    case OP_JUMP:                       return jumpInstruction ("OP_JUMP", 1, chunk, offset);
    case OP_JUMP_IF_FALSE:              return jumpInstruction ("OP_JUMP_IF_FALSE", 1, chunk, offset);
    case OP_LOOP:                       return jumpInstruction ("OP_LOOP", -1, chunk, offset);
    case OP_CALL:                       return byteInstruction("OP_CALL", chunk, offset);
    case OP_RETURN:                     return simpleInstruction ("OP_RETURN", offset);

        // *INDENT-ON*
    }

    printf ("Unknown opcode %d\n", instruction);
    return offset + 1;
}

/** Disassemble the bytecodes in this chunk.
 *
 * Prints the name of the chunk, followed by each
 * bytecode stored in the chunk, disassembled.
 *
 * @param chunk where to get the data
 * @param name what to call it
 */
void
disassembleChunk (Chunk *chunk, const char *name)
{
    printf ("\nDisassembling %s ...\n", name);

    for (int offset = 0; offset < chunk->count;) {
        offset = disassembleInstruction (chunk, offset);
    }
    printf ("Disassembling %s ... done.\n\n", name);
}
