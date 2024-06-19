#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main(int argc, char **argv)
{
    (void)argc;         /* intentionally not used */
    (void)argv;         /* intentionally not used */

    initVM();

    Chunk chunk[1];
    initChunk(chunk);

    int constant = addConstant(chunk, 1.2);
    writeChunk(chunk, OP_CONSTANT, 123);
    writeChunk(chunk, constant, 123);
    writeChunk(chunk, OP_NEGATE, 123);
    writeChunk(chunk, OP_RETURN, 123);
    disassembleChunk(chunk, "test chunk");
    interpret(chunk);

    freeChunk(chunk);

    freeVM();
    return 0;
}
