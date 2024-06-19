#include "common.h"
#include "chunk.h"
#include "debug.h"

int main(int argc, char **argv)
{
    (void)argc;         /* intentionally not used */
    (void)argv;         /* intentionally not used */

    Chunk chunk;
    initChunk(&chunk);

    int constant = addConstant(&chunk, 1.2);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);

    writeChunk(&chunk, OP_RETURN, 123);

    disassembleChunk(&chunk, "test chunk");
    freeChunk(&chunk);

    return 0;
}
