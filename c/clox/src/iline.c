#include "iline.h"

#include "memory.h"

#include <stdio.h>

void
initILine (
    ILine *iline)
{
    iline->count = 0;
    iline->capacity = 0;
    iline->data = 0;
}

void
freeILine (
    ILine *iline)
{
    FREE_ARRAY (ILineData,
        iline->data,
        iline->capacity);

    initILine (iline);
}

void
writeILine (
    ILine *iline,
    int line)
{
    assert (NULL != iline, "iline must not be NULL.");

    int count = iline->count;
    int offset = 0;

    if (count > 0)
        offset = iline->data[count - 1].offset + 1;

    if (count > 0 && iline->data[count - 1].line == line) {
        iline->data[count - 1].offset = offset;
        return;
    }

    if (iline->capacity <= iline->count) {
        int oldCapacity = iline->capacity;
        int newCapacity = GROW_CAPACITY (oldCapacity);
        iline->data = GROW_ARRAY (ILineData,
            iline->data,
            oldCapacity,
            newCapacity);

        iline->capacity = newCapacity;
    }

    iline->data[count].offset = offset;
    iline->data[count].line = line;
    iline->count = count + 1;
}

int
getLine (
    ILine *iline,
    int offset)
{
    if ((iline->data == NULL) || (iline->count < 1))
        return 0;

    int hi = iline->count - 1;
    int lo = 0;

    while (lo < hi) {
        int md = lo + (hi - lo) / 2;

        if (iline->data[md].offset < offset) {
            lo = md + 1;
        } else {
            hi = md;
        }
    }

    return iline->data[lo].line;
}
