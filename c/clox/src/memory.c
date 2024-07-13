#include "memory.h"

#include "compiler.h"
#include "debug.h"
#include "object.h"
#include "vm.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

/** @file memory.c
 * @brief Memory Handling module
 */

/** Return a pointer to data allocated from the brk
 *
 * And especially, sbrk(0) returns the current brk,
 * and as we allocate memory, it goes just above brk.
 *
 * @param increment bytes to allocate
 * @returns a pointer to those bytes in the brk
 */
extern void *sbrk (intptr_t increment);

/** best estimate of the start of the heap memory */
static void *heap_base = 0;

/** static limit on the number of allocations */
#define MAX_HEAP_COUNT 10000

/** Number of allocations that have been recorded. */
static size_t heap_count = 0;

/** allocation history */
static void *heap_ptrs[MAX_HEAP_COUNT];

/** Initialize the memory subsystem.
 */
void
initMemory ()
{
    heap_base = sbrk (0);
    heap_count = 0;
}

/** Recover the heap allocation sequence number for this pointer.
 *
 * @param ptr a value from a previous call to reallocate
 * @returns a sequence number 1..N, or 0 if no seq was assigned.
 */
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

/** Assign a heap allocation sequence number to this pointer.
 *
 * Sequence numbers start at 1 and go up,.
 *
 * @param ptr A value that will be returned by reallocate
 */
static void
addSeq (void *ptr)
{
    INVAR (heap_count < MAX_HEAP_COUNT, "MAX_HEAP_COUNT is not big enough.");
    heap_ptrs[heap_count++] = ptr;
}

/** Construct a repeatable string representing this heap allocation
 *
 * @param ptr A value that will be returned by reallocate
 * @returns a string that can be compared from run to run
 */
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

/** Annotate this object as reachable.
 *
 * @param object some Obj that was reached
 */
void
markObject (Obj *object)
{
    if (object == NULL)
        return;

#ifdef DEBUG_LOG_GC
    printf ("%s mark ", printableHeapAddr (object));
    printValue (OBJ_VAL (object));
    printf ("\n");
#endif

    object->isMarked = true;

    if (vm.grayCapacity < vm.grayCount + 1) {
        vm.grayCapacity = GROW_CAPACITY (vm.grayCapacity);
        vm.grayStack = (Obj **) realloc (vm.grayStack, sizeof (Obj *) * vm.grayCapacity);
        INVAR (NULL != vm.grayStack, "unable to allocate memory for gray stack");
    }

    vm.grayStack[vm.grayCount++] = object;
}

/** Annotate this value as reachable.
 *
 * @param value some Value that was reached
 */
void
markValue (Value value)
{
    if (IS_OBJ (value))
        markObject (AS_OBJ (value));
}

/** Mark contents of an array as reachable.
 *
 * @param array the list of values to mark
 */
static void
markArray (ValueArray *array)
{
    for (int i = 0; i < array->count; i++) {
        markValue (array->values[i]);
    }
}

/** Blacken this object.
 *
 * Mark all of the other objects reachable from this object.
 *
 * @param object the object to blacken
 */
static void
blackenObject (Obj *object)
{
    switch (object->type) {

    case OBJ_CLOSURE:{
            return;
        }

    case OBJ_FUNCTION:{
            ObjFunction *function = (ObjFunction *) object;

            markObject ((Obj *) function->name);
            markArray (&function->chunk.constants);
            return;
        }

    case OBJ_UPVALUE:{
            ObjUpvalue *upvalue = (ObjUpvalue *) object;

            markValue (upvalue->closed);
            return;
        }

    case OBJ_NATIVE:{
            return;
        }

    case OBJ_STRING:{
            return;
        }
    }
    UNREACHABLE ("object type corrupted");
}

/** Free the given object and the storage it owns.
 *
 * @param object which object to free.
 */
static void
freeObject (Obj *object)
{
#ifdef DEBUG_LOG_GC
    printf ("%s free type %d\n", printableHeapAddr (object), object->type);
#endif

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

/** Assure all "roots" are marked as reachable.
 */
static void
markRoots ()
{
    for (Value *slot = vm.stack; slot < vm.sp; slot++) {
        markValue (*slot);
    }
    for (int i = 0; i < vm.frameCount; i++) {
        markObject ((Obj *) vm.frames[i].closure);
    }
    for (ObjUpvalue * upvalue = vm.openUpvalues; upvalue != NULL; upvalue = upvalue->next) {
        markObject ((Obj *) upvalue);
    }
    markTable (&vm.globals);
    markCompilerRoots ();
}

/** Blacken the grey objects
 */
static void
traceReferences ()
{
    while (vm.grayCount > 0) {
        Obj *object = vm.grayStack[--vm.grayCount];

        blackenObject (object);
    }
}

/** Run the Mark-Sweep Garbage Collector
 */
void
collectGarbage ()
{
#ifdef DEBUG_LOG_GC
    printf ("-- gc begin\n");
#endif

    markRoots ();
    traceReferences ();

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

    free (vm.grayStack);
}
