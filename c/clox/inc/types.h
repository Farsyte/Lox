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

typedef enum opcode_e OpCode; /**< Enumerated Bytecode Operations */
typedef struct chunk_s Chunk;
