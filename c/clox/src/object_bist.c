#include "object.h"

#include "error_log.h"
#include "vm.h"

#include <stdio.h>

#define DEBUG_OBJECT_BIST

/** @file object_bist.c
 * @brief Built-In Self Test for the OBJECT module.
 */

extern VM vm;

/** Test function for Native Function Objects
 * @param argCount count of inbound arguments (unused)
 * @param args array of inbound arguments (unused)
 * @returns NIL
 */
static Value
testNative (int argCount, Value *args)
{
    (void) argCount;
    (void) args;
    return NIL_VAL;
}

/** Run all BIST cases for Object.
 */
void
bistObject ()
{
    Obj *objects;
    Value *sp;

    freeVM ();
    initVM ();

    sp = vm.sp;
    INVAR (vm.stack == sp, "bistObject wants to start with a clean slate.");

#ifdef  DEBUG_OBJECT_BIST
    objects = vm.objects;
    while (objects != NULL) {
        Value v = OBJ_VAL (objects);

        printf ("object exists at top of test: '");
        printValue (v);
        printf ("'\n");
        objects = objects->next;
    }
#endif

    ObjString *objString = copyString ("testObject", 10);

    INVAR (NULL != objString, "copyString must return non-NULL");
    INVAR ((Obj *) objString == vm.objects, "new ObjString must be first on the VM objects list.");

    Value valString = OBJ_VAL (objString);

    push (valString);                   // so GC does not reap it

    INVAR (IS_OBJ (valString), "Aftrer init, val must satisfy IS_OBJ");
    INVAR (IS_STRING (valString), "Aftrer init, val must satisfy IS_STRING");

    printf ("valString prints as '");
    printValue (valString);
    printf ("'\n");
    objects = vm.objects;

    ObjNative *objNative = newNative (testNative);

    INVAR (NULL != objNative, "newNative must return non-NULL");
    INVAR ((Obj *) objNative == vm.objects, "new ObjNative must be first on the VM objects list.");

    Value valNative = OBJ_VAL (objNative);

    push (valNative);                   // so GC does not reap it

    INVAR (IS_OBJ (valNative), "Aftrer init, val must satisfy IS_OBJ");
    INVAR (IS_NATIVE (valNative), "Aftrer init, val must satisfy IS_NATIVE");

    printf ("valNative prints as '");
    printValue (valNative);
    printf ("'\n");
    objects = vm.objects;

    ObjFunction *objFunction = newFunction ();

    INVAR (NULL != objFunction, "newFunction must return non-NULL");
    INVAR ((Obj *) objFunction == vm.objects, "new ObjFunction must be first on the VM objects list.");

    Value valFunction = OBJ_VAL (objFunction);

    push (valFunction);                 // so GC does not reap it

    INVAR (IS_OBJ (valFunction), "Aftrer init, val must satisfy IS_OBJ");
    INVAR (IS_FUNCTION (valFunction), "Aftrer init, val must satisfy IS_FUNCTION");

    printf ("valFunction prints as '");
    printValue (valFunction);
    printf ("'\n");

    ObjClosure *objClosure = newClosure (objFunction);

    INVAR (NULL != objClosure, "newClosure must return non-NULL");
    INVAR ((Obj *) objClosure == vm.objects, "new ObjClosure must be first on the VM objects list.");

    Value valClosure = OBJ_VAL (objClosure);

    push (valClosure);                  // so GC does not reap it

    INVAR (IS_OBJ (valClosure), "Aftrer init, val must satisfy IS_OBJ");
    INVAR (IS_CLOSURE (valClosure), "Aftrer init, val must satisfy IS_CLOSURE");

    printf ("valClosure prints as '");
    printValue (valClosure);
    printf ("'\n");

    Value valueStorage = NIL_VAL;
    ObjUpvalue *objUpvalue = newUpvalue (&valueStorage);

    INVAR (NULL != objUpvalue, "newUpvalue must return non-NULL");
    INVAR ((Obj *) objUpvalue == vm.objects, "new ObjUpvalue must be first on the VM objects list.");

    Value valUpvalue = OBJ_VAL (objUpvalue);

    push (valUpvalue);                  // so GC does not reap it

    INVAR (IS_OBJ (valUpvalue), "Aftrer init, val must satisfy IS_OBJ");
    INVAR (IS_UPVALUE (valUpvalue), "Aftrer init, val must satisfy IS_UPVALUE");

    printf ("valUpvalue prints as '");
    printValue (valUpvalue);
    printf ("'\n");

    /* end of test: dump the stack and object list. */

    while (vm.sp > sp) {
        Value vPop = pop ();

        printf ("popped '");
        printValue (vPop);
        printf ("'\n");
    }

#ifdef  DEBUG_OBJECT_BIST
    objects = vm.objects;
    while (objects != NULL) {
        Value v = OBJ_VAL (objects);

        printf ("object exists at end of test: ");
        printValue (v);
        printf ("\n");
        objects = objects->next;
    }
#endif

    freeVM ();
    initVM ();
}
