#pragma once

// Moved the typedef here so BIST and POST can peek inside.

typedef struct {
    const char *start;
    const char *current;
    int line;
} Scanner;

void initScanner (
    const char *source);
