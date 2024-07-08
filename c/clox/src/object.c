#include "object.h"

#include "memory.h"
#include "table.h"
#include "vm.h"

#include <stdio.h>
#include <string.h>

/** @file object.c
 * @brief Implementation for the OBJECT module
 */

/** Allocate an object with the given type.
 */
#define ALLOCATE_OBJ(type, objectType)          ((type*)allocateObject(sizeof (type), objectType))

/** Allocate a new object.
 *
 * @param size byte needed for the object
 * @param type enumerated object type value to write
 * @returns a pointer to the new object structure
 */
static Obj *
allocateObject (size_t size, ObjType type)
{
    Obj *object = (Obj *) reallocate (NULL, 0, size);

    object->type = type;
    object->next = vm.objects;
    vm.objects = object;
    return object;
}

/** Allocate a new Closure object.
 *
 * @param function the compiled code for the closure
 * @returns an ObjClosure object on the heap
 */
ObjClosure *
newClosure (ObjFunction *function)
{
    ObjClosure *closure = ALLOCATE_OBJ (ObjClosure, OBJ_CLOSURE);

    closure->function = function;
    return closure;
}

/** Create a new Function object.
 *
 * @returns a pointer to a new Function object.
 */
ObjFunction *
newFunction ()
{
    ObjFunction *function = ALLOCATE_OBJ (ObjFunction, OBJ_FUNCTION);

    function->arity = 0;
    function->name = NULL;
    initChunk (&function->chunk);
    return function;
}

/** Create a new Native Function object.
 *
 * @param function the glue code
 * @returns a pointer to a new Native Function object.
 */
ObjNative *
newNative (NativeFn function)
{
    ObjNative *native = ALLOCATE_OBJ (ObjNative, OBJ_NATIVE);

    native->function = function;
    return native;
}

/** Create a String object referring to the given content
 *
 * This function constructs a StringObj pointing to the
 * content provided. The string itself is also added to
 * the master string table to allow deduplication (which
 * is done before calling allocateString).
 *
 * @param chars start of the input string
 * @param length number of bytes in the input string
 * @param hash precomputed hash value of the string data
 * @returns a String object with a copy of that content
 */
static ObjString *
allocateString (char *chars, int length, uint32_t hash)
{
    ObjString *string = ALLOCATE_OBJ (ObjString, OBJ_STRING);

    string->length = length;
    string->chars = chars;
    string->hash = hash;
    tableSet (&vm.strings, string, NIL_VAL);
    return string;
}

/** Compute the hash value of the string.
 *
 * This uses the "FNV-1a" hash, which is both
 * effective aind short.
 *
 * @param key start of the string to hash
 * @param length number of bytes to include
 * @returns the 32-bit hash value of the string
 */
static uint32_t
hashString (const char *key, int length)
{
    uint32_t hash = 216613626lu;

    for (int i = 0; i < length; ++i) {
        hash ^= (uint8_t) key[i];
        hash *= 16777619;
    }
    return hash;
}

/** Create a String object referring to the given content
 *
 * This method TAKES OWNERSHIP of the memory containing the inbound
 * string data.
 *
 * If there is a string in the VM's string table with the same length
 * and sequence of bytes, free the inbound buffer and return the
 * existing ObjString.
 *
 * Otherwise, use allocateString to create the ObjString and log the
 * string (and the ObjString) in the string table.
 *
 * @param chars start of the input string
 * @param length number of bytes in the input string
 * @returns existing String matching this content, if found in the string table
 * @returns otherwise a new String linking to this content, added to the  string table
 */
ObjString *
takeString (char *chars, int length)
{
    uint32_t hash = hashString (chars, length);
    ObjString *interned = tableFindString (&vm.strings, chars, length, hash);

    if (interned != NULL) {
        FREE_ARRAY (char, chars, length + 1);

        return interned;
    }

    return allocateString (chars, length, hash);
}

/** Create a String object with a copy of the specified content
 *
 * This method DOES NOT TAKE OWNERSHIP of the memory containing the
 * inbound string data.
 *
 * If there is a string in the VM's string table with the same length
 * and sequence of bytes, return the existing ObjString.
 *
 * Otherwise, copy the string data into freshly allocated heap memory,
 * and use allocateString to create the ObjString and log the string
 * (and the ObjString) in the string table.
 *
 * @param chars start of the input string
 * @param length number of bytes in the input string
 * @returns existing String matching this content, if found in the string table
 * @returns otherwise a new String with a copy of this content, added to the string table
 */
ObjString *
copyString (const char *chars, int length)
{
    uint32_t hash = hashString (chars, length);
    ObjString *interned = tableFindString (&vm.strings, chars, length, hash);

    if (interned != NULL)
        return interned;

    char *heapChars = ALLOCATE (char, length + 1);

    memcpy (heapChars, chars, length);
    heapChars[length] = '\0';
    return allocateString (heapChars, length, hash);
}

/** Print a Function object
 *
 * @param function the object to print
 */
static void
printFunction (ObjFunction *function)
{
    if (function->name == NULL) {
        printf ("<script>");
        return;
    }
    printf ("<fn %s>", function->name->chars);
}

/** Print the value of a Value that is an Object.
 *
 * @param value copy of the Value to print.
 */
void
printObject (Value value)
{
    INVAR (IS_OBJ (value), "inbound Value must be an Object.");

    switch (OBJ_TYPE (value)) {

        // *INDENT-OFF*

    case OBJ_CLOSURE:   printFunction(
                            AS_CLOSURE(value)->function);       return;
    case OBJ_FUNCTION:  printFunction(AS_FUNCTION(value));      return;
    case OBJ_NATIVE:    printf("<native fn>");                  return;
    case OBJ_STRING:    printf("%s", AS_CSTRING(value));        return;

        // *INDENT-ON*
    }
    UNREACHABLE ("corrupted object type");
}
