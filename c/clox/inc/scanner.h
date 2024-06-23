#pragma once

#include "token.h"

typedef struct {
    const char *start;
    const char *current;
    int line;
} Scanner;

void initScanner (
    const char *source);
Token scanToken (
    );

void bist_scanner (
    );
void post_scanner (
    );
