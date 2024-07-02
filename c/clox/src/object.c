#include "object.h"

#include "memory.h"

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
allocateString (char *chars, int length)
{
    ObjString *string = ALLOCATE_OBJ (ObjString, OBJ_STRING);

    string->length = length;
    string->chars = chars;
    return string;
}

/** Create a String object with a copy of the specified content
 *
 * This function copies the indicated string into freshly allocated
 * space on the heap, then uses allocateString to wrap a String Object
 * around it.
 *
 * @param chars start of the input string
 * @param length number of bytes in the input string
 * @return a String object with a copy of that content
 */
ObjString *
copyString (const char *chars, int length)
{
    char *heapChars = ALLOCATE (char, length + 1);

    memcpy (heapChars, chars, length);
    heapChars[length] = '\0';
    return allocateString (heapChars, length);
}
