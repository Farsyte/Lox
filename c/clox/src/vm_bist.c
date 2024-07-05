#include "vm.h"

#include "chunk.h"
#include "compiler.h"

#include <stdio.h>

/** @file vm_bist.c
 * @brief Built-In Self Test for the VM module.
 */

extern VM vm;                   // peek at VM state

/** Run all BIST cases for Vm.
 */
void
bistVM ()
{
    printf ("BIST: %s ...\n", "bistVM");

    initVM ();
    INVAR (NULL == vm.chunk, "initVM did not null the chunk pointer.");
    INVAR (NULL == vm.ip, "initVM did not null the instruction pointer.");
    INVAR (vm.stack == vm.sp, "initVM did not reset the stack pointer.");

    push (NIL_VAL);
    push (BOOL_VAL (false));
    push (BOOL_VAL (true));
    push (NUMBER_VAL (1.0));
    push (NUMBER_VAL (1337.5));

    INVAR (IS_NUMBER (peek (0)), "1st pop will be a number");
    INVAR (IS_NUMBER (peek (1)), "2nd pop will be a number");
    INVAR (IS_BOOL (peek (2)), "3rd pop will be a boolean");
    INVAR (IS_BOOL (peek (3)), "4th pop will be a boolean");
    INVAR (IS_NIL (peek (4)), "5th pop will be a boolean");

    INVAR (1337.5 == AS_NUMBER (pop ()), "1st pop did not return last value.");
    INVAR (1.0 == AS_NUMBER (pop ()), "2nd pop did not return first number value.");
    INVAR (true == AS_BOOL (pop ()), "3rd pop did not return last boolean value.");
    INVAR (false == AS_BOOL (pop ()), "4th pop did not return first boolean value.");
    INVAR (IS_NIL (pop ()), "5th pop did not return nil value.");

    {

        // *INDENT-OFF*
        const char source[] =
            "print 1 + 2; // add and print\n"
            "print 3 * 4; // mul and print\n"
            ;
        // *INDENT-ON*

        InterpretResult result = interpret (source);

        INVAR (INTERPRET_OK == result, "simple compilation failed");
    }

    freeVM ();

    printf ("BIST: %s ... done.\n", "bistVM");
}
