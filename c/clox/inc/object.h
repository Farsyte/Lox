#pragma once

#include "chunk.h"
#include "common.h"
#include "value.h"

/** @file object.h
 * @brief Macros and API exposed by the OBJECT module
 */

/** Extract the enumerated value type from a value. */
#define OBJ_TYPE(value) (AS_OBJ(value)->type)

/** Return true iff the value is a Function. */
#define IS_FUNCTION(value) isObjType(value, OBJ_FUNCTION)

/** Return true iff the value is a Native Function. */
#define IS_NATIVE(value) isObjType(value, OBJ_NATIVE)

/** Return true iff the value is a String. */
#define IS_STRING(value) isObjType(value, OBJ_STRING)

/** Return the Function object in this Value. */
#define AS_FUNCTION(value) ((ObjFunction*)AS_OBJ(value))

/** Return the Native Function object in this Value. */
#define AS_NATIVE(value) ((ObjNative*)AS_OBJ(value))

/** Return the String object in this Value. */
#define AS_STRING(value) ((ObjString*)AS_OBJ(value))

/** Return the C String in this Value. */
#define AS_CSTRING(value) (AS_STRING(value)->chars)

/** Enumerate the possible object types. */
typedef enum {
    OBJ_FUNCTION,               ///< Object is a Function
    OBJ_NATIVE,                 ///< Object is a Native Function
    OBJ_STRING,                 ///< Object contains a string
} ObjType;

/** Base structure for all Objects */
struct Obj {
    ObjType type;               ///< Designate the type of the object
    struct Obj *next;           ///< next eldest object
};

/** Object that is a Function */
struct ObjFunction {
    Obj obj;                    ///< Inherit from Obj
    int arity;                  ///< number of parameters
    Chunk chunk;                ///< compiled bytecode for the function
    ObjString *name;            ///< function name in an ObjString
};

/** Object that is a Native Function */
struct ObjNative {
    Obj obj;                    ///< Inherit from obj
    NativeFn function;          ///< the glue function
};

/** Object that is a string */
struct ObjString {
    Obj obj;                    ///< Inherit from Obj
    int length;                 ///< length of string
    char *chars;                ///< pointer to start of string
    uint32_t hash;              ///< hash code for the string
};

extern ObjFunction *newFunction ();
extern ObjNative *newNative (NativeFn function);
extern ObjString *takeString (char *chars, int length);
extern ObjString *copyString (const char *chars, int length);
extern void printObject (Value value);

extern void postObject ();
extern void bistObject ();

/** INLINE function to check object types
 *
 * @param value the value object to exmaine
 * @param type the enumerated object type of interest
 * @returns false if the value is not an object
 * @returns false if the object type does not match
 * @returns true if the value is an object of the correct type
 */
static inline bool
isObjType (Value value, ObjType type)
{
    return IS_OBJ (value) && AS_OBJ (value)->type == type;
}
