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
    writeChunk(&chunk, OP_CONSTANT);
    writeChunk(&chunk, constant);

    writeChunk(&chunk, OP_RETURN);

    disassembleChunk(&chunk, "test chunk");
    freeChunk(&chunk);

    return 0;
}
