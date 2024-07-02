#include "vm.h"

/** @file vm_bist.c
 * @brief Built-In Self Test for the VM module.
 */

extern VM vm;                   // peek at VM state

/** Run all BIST cases for Vm.
 */
void
bistVM (
    )
{
    initVM ();
    INVAR (NULL == vm.chunk, "initVM did not null the chunk pointer.");
    INVAR (NULL == vm.ip, "initVM did not null the instruction pointer.");
    INVAR (vm.stack == vm.sp, "initVM did not reset the stack pointer.");

    push (1.0);
    push (1337.5);

    INVAR (1337.5 == pop (), "first pop did not return last value.");
    INVAR (1.0 == pop (), "last pop did not return first value.");

    freeVM ();
}
