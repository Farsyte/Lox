#include "memory.h"

#include "object.h"
#include "vm.h"

#include <stdio.h>
#include <string.h>

/** @file memory.c
 * @brief Memory Handling module
 */

/** Enable REDZONE protection with 256 byte redzones. */
#define REDZONE 256

// #define REDZONE_VERBOSE

#ifdef  REDZONE
/** Check a Redzone.
 *
 * @param red start of the redzone
 * @param exp expected value in the redzone
 */
void
redcheck (uint8_t *red, uint8_t exp)
{
    for (size_t i = 0; i < REDZONE; i++) {
        if (red[i] != exp) {
            fprintf (stderr, "    REDZONE FAIL: expect %02X, observe %02X, at %p\n", exp, red[i], red + i);
            abort ();
        }
    }
}
#endif

/** Reallocate storage
 *
 * Given a pointer to oldSize bytes of allocated storage, return a
 * pointer to newSize bytes of allocated storage, with the contents of
 * the original area copied to the new area, and release the old
 * storage back to the storage pool.
 *
 * The macro does not use realloc directly, because we want allocation
 * errors to be reported and to terminate the program.
 *
 * @param pointer address of existing storage block
 * @param oldSize size in bytes of existing storage block
 * @param newSize desired size in bytes of new storage block
 * @returns NULL if the new size was zero, otherwise
 * @returns a pointer to allocated storage of the new size
 */
void *
reallocate (void *pointer, size_t oldSize, size_t newSize)
{
    (void) oldSize;                     // not needed by this implementation.

#ifdef  REDZONE_VERBOSE
    fprintf (stderr, "reallocate request:\n");
    fprintf (stderr, "    %p start of caller memory\n", pointer);
    fprintf (stderr, "    %p   end of caller memory\n", pointer + oldSize);
    fprintf (stderr, "    %lu old size\n", oldSize);
    fprintf (stderr, "    %lu new size\n", newSize);
#endif

#ifdef  REDZONE
    if (oldSize > 0) {
        void *tail = pointer + oldSize;
        void *head = pointer - REDZONE;

#ifdef  REDZONE_VERBOSE
        fprintf (stderr, "reallocate checking redzones::\n");
        fprintf (stderr, "    %p start of head redzone\n", head);
        fprintf (stderr, "    %p   end of head redzone (caller 'pointer' value)\n", head + REDZONE);
        fprintf (stderr, "    %p start of tail redzone\n", tail);
        fprintf (stderr, "    %p   end of tail redzone\n", tail + REDZONE);
#endif

        redcheck (head, 0x5A);
        redcheck (tail, 0x5F);

        pointer = head;

        oldSize += REDZONE;
        oldSize += REDZONE;
    }
#endif

    if (newSize == 0) {
        if (NULL != pointer) {
            free (pointer);
        }
        return NULL;
    }
#ifdef  REDZONE
    if (newSize > 0) {
        newSize += REDZONE;
        newSize += REDZONE;
    }
#endif

#ifdef  REDZONE_VERBOSE
    fprintf (stderr, "reallocate calls to realloc:\n");
    fprintf (stderr, "    %p start of area to reallocate\n", pointer);
    fprintf (stderr, "    %p   end of area to reallocate\n", pointer + oldSize);
    fprintf (stderr, "    %lu new size\n", newSize);
#endif

    void *result = realloc (pointer, newSize);

#ifdef  REDZONE_VERBOSE
    fprintf (stderr, "    %p result of realloc\n", result);
#endif

    INVAR (NULL != result, "realloc  failed.");

#ifdef  REDZONE
    if (NULL != result) {
        void *head = result;
        void *tail = result + newSize - REDZONE;

#ifdef  REDZONE_VERBOSE
        fprintf (stderr, "reallocate writing redzones::\n");
        fprintf (stderr, "    %p start of head redzone\n", head);
        fprintf (stderr, "    %p   end of head redzone (caller 'result' value)\n", head + REDZONE);
        fprintf (stderr, "    %p start of tail redzone\n", tail);
        fprintf (stderr, "    %p   end of tail redzone\n", tail + REDZONE);
#endif

        memset (head, 0x5A, REDZONE);
        memset (tail, 0x5F, REDZONE);

        result = head + REDZONE;
    }
#endif

    return result;
}

/** Free the given object and the storage it owns.
 *
 * @param object which object to free.
 */
static void
freeObject (Obj *object)
{
    switch (object->type) {

    case OBJ_CLOSURE:{
            ObjClosure *closure = (ObjClosure *) object;

            FREE_ARRAY (ObjUpvalue *, closure->upvalues, closure->upvalueCount);
            FREE (ObjClosure, object);

            return;
        }

    case OBJ_FUNCTION:{
            ObjFunction *function = (ObjFunction *) object;

            freeChunk (&function->chunk);
            FREE (ObjFunction, object);

            return;
        }

    case OBJ_NATIVE:{
            FREE (ObjNative, object);
            return;
        }

    case OBJ_STRING:{
            ObjString *string = (ObjString *) object;
            FREE_ARRAY (char, string->chars, string->length + 1);
            FREE (ObjString, object);

            return;
        }

    case OBJ_UPVALUE:{
            FREE (ObjUpvalue, object);
            return;
        }

    }
    UNREACHABLE ("corrupted object type");
}

/** Free all Objects.
 */
void
freeObjects ()
{
    Obj *object = vm.objects;

    while (object != NULL) {
        Obj *next = object->next;

        freeObject (object);
        object = next;
    }
}
