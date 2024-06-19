#include "common.h"
#include "chunk.h"

int main(int argc, char **argv)
{
    (void)argc;         /* intentionally not used */
    (void)argv;         /* intentionally not used */

    Chunk chunk;
    initChunk(&chunk);
    writeChunk(&chunk, OP_RETURN);
    freeChunk(&chunk);

    return 0;
}
