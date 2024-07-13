#include "memory.h"

#include "debug.h"
#include "object.h"
#include "vm.h"

#include <stdio.h>
#include <string.h>

/** @file memory.c
 * @brief Memory Handling module
 */

extern void *sbrk (intptr_t increment);
static void *heap_base = 0;

#define MAX_HEAP_COUNT 10000
static size_t heap_count = 0;
static void *heap_ptrs[MAX_HEAP_COUNT];

void
initMemory ()
{
    heap_base = sbrk (0);
    heap_count = 0;
}

static size_t
getSeq (void *ptr)
{
    size_t result = heap_count;

    while (result-- > 0) {
        if (ptr == heap_ptrs[result]) {
            return result + 1;
        }
    }
    return 0;
}

static void
addSeq (void *ptr)
{
    INVAR (heap_count < MAX_HEAP_COUNT, "MAX_HEAP_COUNT is not big enough.");
    heap_ptrs[heap_count++] = ptr;
}

const char *
printableHeapAddr (void *ptr)
{
    INVAR (NULL != heap_base, "initMemory was not called");
    if (NULL == ptr)
        return "NULL";
    size_t seq = getSeq (ptr);

    static char buf[64];

    if (seq == 0) {
        snprintf (buf, sizeof buf - 1, "heap+%06lX", ptr - heap_base);
    } else {
        snprintf (buf, sizeof buf - 1, "heap_%03lu", seq);
    }
    return buf;
}

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
    if (newSize > oldSize) {
#ifdef DEBUG_STRESS_GC
        collectGarbage ();
#endif
    }

    (void) oldSize;                     // not needed by this implementation.

    if (newSize == 0) {
        if (NULL != pointer) {
            free (pointer);
        }
        return NULL;
    }

    void *result = realloc (pointer, newSize);

    INVAR (NULL != result, "realloc  failed.");

    if (result != pointer)
        addSeq (result);

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

/** Run the Mark-Sweep Garbage Collector
 */
void
collectGarbage ()
{
#ifdef DEBUG_LOG_GC
    printf ("-- gc begin\n");
#endif
    // will have code soon
#ifdef DEBUG_LOG_GC
    printf ("-- gc end\n");
#endif
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
