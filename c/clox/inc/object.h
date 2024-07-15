#pragma once

#include "chunk.h"
#include "common.h"
#include "table.h"
#include "value.h"

/** @file object.h
 * @brief Macros and API exposed by the OBJECT module
 */

/** Return true iff the value is a Upvalue. */
#define IS_UPVALUE(value) isObjType(value, OBJ_UPVALUE)

/** Return the Upvalue object in this Value. */
#define AS_UPVALUE(value) ((ObjUpvalue*)AS_OBJ(value))

/** Extract the enumerated value type from a value. */
#define OBJ_TYPE(value) (AS_OBJ(value)->type)

/** Return true iff the value is a Class. */
#define IS_CLASS(value) isObjType(value, OBJ_CLASS)

/** Return true iff the value is a Closure. */
#define IS_CLOSURE(value) isObjType(value, OBJ_CLOSURE)

/** Return true iff the value is a Function. */
#define IS_FUNCTION(value) isObjType(value, OBJ_FUNCTION)

/** Return true iff the value is a Native Function. */
#define IS_NATIVE(value) isObjType(value, OBJ_NATIVE)

/** Return true iff the value is a Class Instance. */
#define IS_INSTANCE(value) isObjType(value, OBJ_INSTANCE)

/** Return true iff the value is a String. */
#define IS_STRING(value) isObjType(value, OBJ_STRING)

/** Return the Class object in this Value. */
#define AS_CLASS(value) ((ObjClass*)AS_OBJ(value))

/** Return the Closure object in this Value. */
#define AS_CLOSURE(value) ((ObjClosure*)AS_OBJ(value))

/** Return the Function object in this Value. */
#define AS_FUNCTION(value) ((ObjFunction*)AS_OBJ(value))

/** Return the Instance object in this Value. */
#define AS_INSTANCE(value) ((ObjInstance*)AS_OBJ(value))

/** Return the Native Function object in this Value. */
#define AS_NATIVE(value) ((ObjNative*)AS_OBJ(value))

/** Return the String object in this Value. */
#define AS_STRING(value) ((ObjString*)AS_OBJ(value))

/** Return the C String in this Value. */
#define AS_CSTRING(value) (AS_STRING(value)->chars)

/** Enumerate the possible object types. */
typedef enum {
    OBJ_CLASS,                  ///< Object is a Class
    OBJ_CLOSURE,                ///< Object is a Closure
    OBJ_FUNCTION,               ///< Object is a Function
    OBJ_INSTANCE,               ///< Object is an Class Instance
    OBJ_NATIVE,                 ///< Object is a Native Function
    OBJ_STRING,                 ///< Object contains a string
    OBJ_UPVALUE,                ///< Object is an Upvalue
} ObjType;

/** Base structure for all Objects */
struct Obj {
    ObjType type;               ///< Designate the type of the object
    bool isMarked;              ///< set by GC for reachable objects
    struct Obj *next;           ///< next eldest object
};

/** Object that is a Function */
struct ObjFunction {
    Obj obj;                    ///< Inherit from Obj
    int arity;                  ///< number of parameters
    int upvalueCount;           ///< number of upvalues
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

/** Object that is an Upvalue */
struct ObjUpvalue {
    Obj obj;                    ///< Inherit from obj
    Value *location;            ///< where to find the storage
    Value closed;               ///< storage for upvalue when closed
    ObjUpvalue *next;           ///< make a linked list for searching
};

/** Object that is a closure */
struct ObjClosure {
    Obj obj;                    ///< Inherit from Obj
    ObjFunction *function;      ///< compiled function
    ObjUpvalue **upvalues;      ///< list of upvalues
    int upvalueCount;           ///< number of upvalues
};

/** Object that is a class */
struct ObjClass {
    Obj obj;                    ///< Inherit from Obj
    ObjString *name;            ///< class name
};

/** Object that is a instance */
struct ObjInstance {
    Obj obj;                    ///< Inherit from Obj
    ObjClass *klass;            ///< the class of the instance
    Table fields;               ///< instance properties
};

extern ObjClass *newClass (ObjString *name);
extern ObjClosure *newClosure (ObjFunction *function);
extern ObjFunction *newFunction ();
extern ObjInstance *newInstance (ObjClass *klass);
extern ObjNative *newNative (NativeFn function);
extern ObjString *takeString (char *chars, int length);
extern ObjString *copyString (const char *chars, int length);
extern ObjUpvalue *newUpvalue (Value *slot);
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
