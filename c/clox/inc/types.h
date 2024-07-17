#pragma once

#include <stdbool.h>

/** @file types.h
 * @brief Collected Type Definitions
 *
 * This file collects all of the "typedef" declarations together in
 * one place, providing a simple resolution of the most common
 * circular definition problem, where header A uses a type defined in
 * header B, but header B also uses a type defined in header A.
 *
 * Note that this does not mean header A knows the size of a structure
 * defined in B, while at the same time header B knows the size of a
 * structure defined in A; this is a true circular dependency that
 * requires a bit of a rethink.
 */

/** Chunk of Bytecodes */
typedef struct Chunk Chunk;

/** Value Representation */
typedef struct Value Value;

/** Expandable Array of Values */
typedef struct ValueArray ValueArray;

/** Instruction Execution Virtual Machine */
typedef struct VM VM;

/** Storage relating to command line options */
typedef struct Options Options;

/** Storage associated with the Scanner */
typedef struct Scanner Scanner;

/** Storage associated with each Token */
typedef struct Token Token;

/** Storage for the Parser (in compiler.c) */
typedef struct Parser Parser;

/** Parse Rule layout */
typedef struct ParseRule ParseRule;

// *INDENT-OFF*
/** Pointer to a Parse Function */
typedef void (*ParseFn) (bool canAssign);
// *INDENT-ON*

/** Base structure for Objects */
typedef struct Obj Obj;

/** String objects */
typedef struct ObjString ObjString;

/** Hash Table */
typedef struct Table Table;

/** Hash Table Entry */
typedef struct Entry Entry;

/** Compiler State */
typedef struct Compiler Compiler;

/** Local Variables
 */
typedef struct Local Local;

/** Function Objects
 */
typedef struct ObjFunction ObjFunction;

/** Stack Frame
 */
typedef struct CallFrame CallFrame;

/** Native Function
 */
typedef Value (*NativeFn) (int argCount, Value *args);

/** Native Function Object
 */
typedef struct ObjNative ObjNative;

/** Closure Object
 */
typedef struct ObjClosure ObjClosure;

/** Upvalue state
 */
typedef struct Upvalue Upvalue;

/** Upvalue Object
 */
typedef struct ObjUpvalue ObjUpvalue;

/** Class Object
 */
typedef struct ObjClass ObjClass;

/** Instance Object
 */
typedef struct ObjInstance ObjInstance;

/** Bound Method
 */
typedef struct ObjBoundMethod ObjBoundMethod;
