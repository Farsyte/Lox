#include "memory.h"

#include "object.h"
#include "vm.h"

/** @file memory.c
 * @brief Memory Handling module
 */

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

    if (newSize == 0) {
        free (pointer);
        return NULL;
    }

    void *result = realloc (pointer, newSize);

    INVAR (NULL != result, "reallocate failed.");

    return result;
}

/** Free the given object and the storage it owns.
 *
 * @param object which object to free.
 */
static void
freeObject (Obj * object)
{
    switch (object->type) {

    case OBJ_FUNCTION:{
            ObjFunction *function = (ObjFunction *) object;

            freeChunk (&function->chunk);
            FREE (ObjFunction, object);
            return;
        }

    case OBJ_STRING:{
            ObjString *string = (ObjString *) object;
            FREE_ARRAY (char, string->chars, string->length + 1);
            FREE (ObjString, object);

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
