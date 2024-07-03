#pragma once

#include "common.h"
#include "value.h"

/** @file object.h
 * @brief Macros and API exposed by the OBJECT module
 */

#define OBJ_TYPE(value) (AS_OBJ(value)->type)
#define IS_STRING(value) isObjType(value, OBJ_STRING)
#define AS_STRING(value) ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value) (AS_STRING(value)->chars)

/** Enumerate the possible object types. */
enum ObjType {
    OBJ_STRING,                 ///< Object contains a string
};

/** Base structure for all Objects */
struct Obj {
    ObjType type;               ///< Designate the type of the object
    struct Obj *next;           ///< next eldest object
};

/** Object that is a string */
struct ObjString {
    Obj obj;                    ///< Inherit from Obj
    int length;                 ///< length of string
    char *chars;                ///< pointer to start of string
};

extern ObjString *takeString (char *chars, int length);
extern ObjString *copyString (const char *chars, int length);
extern void printObject (Value value);

static inline bool
isObjType (Value value, ObjType type)
{
    return IS_OBJ (value) && AS_OBJ (value)->type == type;
}
