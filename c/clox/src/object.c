#include "object.h"

#include "memory.h"
#include "vm.h"

#include <stdio.h>
#include <string.h>

/** @file object.c
 * @brief Implementation for the OBJECT module
 */

/** Allocate an object with the given type.
 */
#define ALLOCATE_OBJ(type, objectType)          ((type*)allocateObject(sizeof (type), objectType))

static Obj *
allocateObject (size_t size, ObjType type)
{
    Obj *object = (Obj *) reallocate (NULL, 0, size);

    object->type = type;
    object->next = vm.objects;
    vm.objects = object;
    return object;
}

/** Create a String object referring to the given content
 *
 * This function constructs a StringObj pointing to the
 * content provided.
 *
 * @param chars start of the input string
 * @param length number of bytes in the input string
 * @return a String object with a copy of that content
 */
static ObjString *
allocateString (char *chars, int length, uint32_t hash)
{
    ObjString *string = ALLOCATE_OBJ (ObjString, OBJ_STRING);

    string->length = length;
    string->chars = chars;
    string->hash = hash;
    return string;
}

/** Compute the hash value of the string.
 *
 * This uses the "FNV-1a" hash, which is both
 * effective aind short.
 *
 * @param key start of the string to hash
 * @param length number of bytes to include
 * @return the 32-bit hash value of the string
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
 * This function constructs a StringObj pointing to the
 * content provided.
 *
 * To be clear: this function DOES take ownership of the
 * memory chars points to.
 *
 * @param chars start of the input string
 * @param length number of bytes in the input string
 * @return a String object with a copy of that content
 */
ObjString *
takeString (char *chars, int length)
{
    uint32_t hash = hashString (chars, length);

    return allocateString (chars, length, hash);
}

/** Create a String object with a copy of the specified content
 *
 * This function copies the indicated string into freshly allocated
 * space on the heap, then uses allocateString to wrap a String Object
 * around it.
 *
 * To be clear: this function DOES NOT take ownership of the
 * memory chars points to.
 *
 * @param chars start of the input string
 * @param length number of bytes in the input string
 * @return a String object with a copy of that content
 */
ObjString *
copyString (const char *chars, int length)
{
    uint32_t hash = hashString (chars, length);
    char *heapChars = ALLOCATE (char, length + 1);

    memcpy (heapChars, chars, length);
    heapChars[length] = '\0';
    return allocateString (heapChars, length, hash);
}

void
printObject (Value value)
{
    switch (OBJ_TYPE (value)) {

        // *INDENT-OFF*

    case OBJ_STRING:    printf("%s", AS_CSTRING(value));        return;

        // *INDENT-ON*
    }
    STUB ("Report runtime error (Reached UNREACHABLE code).");
}
