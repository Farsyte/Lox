#pragma once

#include "token_type.h"

typedef struct {
    TokenType type;
    const char *start;
    int length;
    int line;
} Token;
