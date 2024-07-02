#include "memory.h"

#include <stdio.h>

/** @file memory_bist.c
 * @brief Built-In Self Test for the MEMORY module.
 */

/** Run all BIST cases for Memory.
 */
void
bistMemory ()
{
    INVAR (8 == GROW_CAPACITY (0), "0 grows to 8");
    INVAR (16 == GROW_CAPACITY (8), "8 grows to 16");
    INVAR (32 == GROW_CAPACITY (16), "16 grows to 32");

    int *ptr = NULL;
    int cap = 0;

    INVAR (0 == cap, "test case precondition failed");
    INVAR (NULL == ptr, "test case precondition failed");
    {
        int newCap = GROW_CAPACITY (cap);
        int *newPtr = GROW_ARRAY (int, ptr, cap, newCap);

        INVAR (8 == newCap, "first GROW gives size 8");
        INVAR (NULL != newPtr, "first GROW allocates storage");

        for (int i = cap; i < newCap; ++i)
            newPtr[i] = i * 1337;
        ptr = newPtr;
        cap = newCap;
    }

    INVAR (8 == cap, "test case precondition failed");
    INVAR (NULL != ptr, "test case precondition failed");
    {

        int newCap = GROW_CAPACITY (cap);
        int *newPtr = GROW_ARRAY (int, ptr, cap, newCap);

        INVAR (16 == newCap, "second GROW gives size 16");
        INVAR (NULL != newPtr, "second GROW allocates storage");

        // This condition is "expected but not required"
        // INVAR (ptr != newPtr, "second GROW changes the address");
        for (int i = 0; i < cap; ++i)
            INVAR (i * 1337 == newPtr[i], "data must be copied");

        for (int i = cap; i < newCap; ++i)
            newPtr[i] = i * 1337;
        ptr = newPtr;
        cap = newCap;
    }

    INVAR (16 == cap, "test case precondition failed");
    INVAR (NULL != ptr, "test case precondition failed");
    {

        int newCap = GROW_CAPACITY (cap);
        int *newPtr = GROW_ARRAY (int, ptr, cap, newCap);

        INVAR (32 == newCap, "third GROW gives size 32");
        INVAR (NULL != newPtr, "third GROW allocates storage");

        // This condition is "expected but not required"
        // INVAR (ptr != newPtr, "third GROW changes the address");
        for (int i = 0; i < cap; ++i)
            INVAR (i * 1337 == newPtr[i], "data must be copied");

        for (int i = cap; i < newCap; ++i)
            newPtr[i] = i * 1337;
        ptr = newPtr;
        cap = newCap;
    }

    INVAR (0 < cap, "test case precondition failed");
    INVAR (NULL != ptr, "test case precondition failed");
    {

        int *newPtr = GROW_ARRAY (int, ptr, cap, 0);

        INVAR (NULL == newPtr, "GROW to size zero releases storage");
        ptr = NULL;
        cap = 0;
    }
}
