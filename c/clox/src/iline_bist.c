#include "assert.h"
#include "bist.h"
#include "iline.h"

#include <stdio.h>

void
bist_iline (
    )
{
    ILine iline[1];

    initILine (iline);
    assert (0 == iline->count, "after initILine, count should be zero.");
    assert (0 == iline->capacity,
        "after initILine, capacity should be zero.");
    assert (NULL == iline->data, "after initILine, data should be NULL.");

    assert (0 == getLine (iline, 0),
        "getLine should return 0 after initILine but before any writeILine.");

    writeILine (iline, 101);
    assert (1 == iline->count, "after writeILine, count should be one.");
    assert (iline->count <= iline->capacity,
        "after writeILine, capacity should not be less than count.");
    assert (NULL != iline->data,
        "after writeILine, data should not be NULL.");
    assert (0 == iline->data[0].offset,
        "after writeILine, data[0].offset should be zero.");
    assert (101 == iline->data[0].line,
        "after writeILine, data[0].line should be 101.");

    writeILine (iline, 101);
    assert (1 == iline->count, "after 2nd writeILine, count should be one.");
    assert (iline->count <= iline->capacity,
        "after 2nd writeILine, capacity should not be less than count.");
    assert (NULL != iline->data,
        "after 2nd writeILine, data should not be NULL.");
    assert (1 == iline->data[0].offset,
        "after 2nd writeILine, data[0].offset should be one.");
    assert (101 == iline->data[0].line,
        "after 2nd writeILine, data[0].line should be 101.");

    writeILine (iline, 103);
    assert (2 == iline->count, "after 3rd writeILine, count should be two.");
    assert (iline->count <= iline->capacity,
        "after 3rd writeILine, capacity should not be less than count.");
    assert (NULL != iline->data,
        "after 3rd writeILine, data should not be NULL.");
    assert (2 == iline->data[1].offset,
        "after 3rd writeILine, data[1].offset should be two.");
    assert (103 == iline->data[1].line,
        "after 3rd writeILine, data[1].line should be 103.");

    writeILine (iline, 103);
    assert (2 == iline->count, "after 4th writeILine, count should be two.");
    assert (iline->count <= iline->capacity,
        "after 4th writeILine, capacity should not be less than count.");
    assert (NULL != iline->data,
        "after 4th writeILine, data should not be NULL.");
    assert (3 == iline->data[1].offset,
        "after 4th writeILine, data[1].offset should be three.");
    assert (103 == iline->data[1].line,
        "after 4th writeILine, data[1].line should be 103.");

    assert (103 == getLine (iline, 3),
        "getLine(iline, 3) should return 103.");
    assert (103 == getLine (iline, 2),
        "getLine(iline, 2) should return 103.");
    assert (101 == getLine (iline, 1),
        "getLine(iline, 1) should return 101.");
    assert (101 == getLine (iline, 0),
        "getLine(iline, 0) should return 101.");

    assert (103 == getLine (iline, 99),
        "getLine returns highest line for offsets higher than recorded.");
    assert (101 == getLine (iline, -99),
        "getLine returns lowest line for offsets lower than recorded.");

    freeILine (iline);
    assert (0 == iline->count, "after freeILine, count should be zero.");
    assert (0 == iline->capacity,
        "after freeILine, capacity should be zero.");
    assert (NULL == iline->data, "after freeILine, data should be NULL.");

}
