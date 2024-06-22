#pragma once

#include "bist.h"
#include "post.h"

typedef struct {
    int offset;
    int line;
} ILineData;

typedef struct {
    int count;
    int capacity;
    ILineData *data;
} ILine;

void initILine (
    ILine *iline);

void freeILine (
    ILine *iline);

void writeILine (
    ILine *iline,
    int line);

int getLine (
    ILine *iline,
    int offset);

post_fn post_iline;
bist_fn bist_iline;
