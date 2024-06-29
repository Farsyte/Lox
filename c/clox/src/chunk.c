#include "chunk.h"

#include "memory.h"
#include "value.h"

/** @file chunk.c
 * @brief Manage Chunks of Bytecode
 */

/** Initialize Chunk.
 *
 * Place chunk into initial state
 * - no bytecodes stored
 * - current capacity is zero
 * - no bytecode storage owned
 *
 * @param chunk memory to be initialized
 */
void
initChunk (
    Chunk *chunk)
{

    INVAR (NULL != chunk, "NULL chunk passed to initChunk");

    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    initValueArray (&chunk->constants);
}

/** Release resources owned by Chunk.
 *
 * This function frees all memory owned by the chunk
 * and returns it to its initial state.
 *
 * This does NOT include doing a ~free(chunk)~
 * because chunk itself may not have been allocated.
 *
 * @param chunk the chunk to manipulate
 */
void
freeChunk (
    Chunk *chunk)
{
    FREE_ARRAY (uint8_t, chunk->code, chunk->capacity);
    freeValueArray (&chunk->constants);
    initChunk (chunk);
}

/** Write an OpCode onto the end of the chunk.
 *
 * @param chunk what to write into
 * @param code value to add to it
 */
void
writeChunk (
    Chunk *chunk,
    uint8_t byte)
{

    if (chunk->capacity < chunk->count + 1) {
        int oldCapacity = chunk->capacity;

        chunk->capacity = GROW_CAPACITY (oldCapacity);
        chunk->code =
            GROW_ARRAY (uint8_t, chunk->code, oldCapacity, chunk->capacity);
    }

    chunk->code[chunk->count] = byte;
    chunk->count++;
}

/** Add a value to the constant pool.
 *
 * Append the value to the constant pool, and return
 * the index where it was stored.
 */
int
addConstant (
    Chunk *chunk,
    Value value)
{
    writeValueArray (&chunk->constants, value);
    return chunk->constants.count - 1;
}
