#pragma once

typedef enum
{
    OP_RETURN,
} OpCode;

typedef struct
{
    OpCode *code;
} Chunk;

extern int post_chunk (
    );
extern int bist_chunk (
    );
