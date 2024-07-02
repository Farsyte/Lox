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
typedef enum opcode_e OpCode;

/** Chunk of Bytecodes */
typedef struct chunk_s Chunk;

/** Value Representation */
typedef double Value;

/** Expandable Array of Values */
typedef struct value_array_s ValueArray;

/** Instruction Execution Virtual Machine */
typedef struct vm_s VM;

/** Result code from Interpreter */
typedef enum interpret_result_e InterpretResult;

/** Storage relating to command line options */
typedef struct options_s Options;

/** Storage associated with the Scanner */
typedef struct scanner_s Scanner;

/** Datatype for enumerated TokenType constants */
typedef enum token_type_e TokenType;

/** Storage associated with each Token */
typedef struct token_s Token;

/** Storage for the Parser (in compiler.c) */
typedef struct parser_s Parser;
