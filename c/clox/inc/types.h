#pragma once

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

/** Enumerated Bytecode Operations */
typedef enum OpCode OpCode;

/** Chunk of Bytecodes */
typedef struct Chunk Chunk;

/** Value Representation */
typedef struct Value Value;

/** Expandable Array of Values */
typedef struct ValueArray ValueArray;

/** Instruction Execution Virtual Machine */
typedef struct VM VM;

/** Result code from Interpreter */
typedef enum InterpretResult InterpretResult;

/** Storage relating to command line options */
typedef struct Options Options;

/** Storage associated with the Scanner */
typedef struct Scanner Scanner;

/** Datatype for enumerated TokenType constants */
typedef enum TokenType TokenType;

/** Storage associated with each Token */
typedef struct Token Token;

/** Storage for the Parser (in compiler.c) */
typedef struct Parser Parser;

/** Enumerated precedence order */
typedef enum Precedence Precedence;

/** Parse Rule layout */
typedef struct ParseRule ParseRule;

// *INDENT-OFF*
/** Pointer to a Parse Function */
typedef void (*ParseFn) ();
// *INDENT-ON*

/** Enumerated value types */
typedef enum ValueType ValueType;

/** Base structure for Objects */
typedef struct Obj Obj;

/** Enumerated Object Types */
typedef enum ObjType ObjType;

/** String objects */
typedef struct ObjString ObjString;

/** Hash Table */
typedef struct Table Table;

/** Hash Table Entry */
typedef struct Entry Entry;
