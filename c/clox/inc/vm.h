#pragma once

#include "common.h"
#include "object.h"
#include "table.h"
#include "value.h"

/** Number of stack frames
 */
#define FRAMES_MAX 64

/** Maximum Stack Depth
 */
#define STACK_MAX (FRAMES_MAX * UINT8_COUNT)

/** Stack Frame for each Function Call
 *
 * The IP in the stack frame is unconventional.
 */
struct CallFrame {
    ObjClosure *closure;        ///< which closure owns the frame
    uint8_t *ip;                ///< this frame's instruction pointer
    Value *slots;               ///< local variable storage
};

/** Internal state of the VM
 */
struct VM {
    CallFrame frames[FRAMES_MAX];       ///< storage for stack frames
    int frameCount;             ///< number of currently active stack frames
    Value stack[STACK_MAX];     ///< storage for the data stack
    Value *sp;                  ///< data stack pointer
    Table globals;              ///< hash table of global variables
    Table strings;              ///< hash table for string deduplication
    ObjUpvalue *openUpvalues;   ///< linked list of ALL open upvalues on the stack
    Obj *objects;               ///< linked list of all objects

    int grayCount;              ///< number of grey items identified
    int grayCapacity;           ///< capacity of the gray item list
    Obj **grayStack;            ///< extensable array of gray items

};

/** Interpreter result
 */
typedef enum {
    INTERPRET_OK,               ///< everything went well
    INTERPRET_COMPILE_ERROR,    ///< error during compilation
    INTERPRET_RUNTIME_ERROR,    ///< error during interpretation
} InterpretResult;

extern VM vm;                   ///< export the singleton VM instance.

extern void initVM ();
extern void freeVM ();
extern InterpretResult interpretChunk (Chunk *chunk);
extern InterpretResult interpret (const char *source);
extern void push (Value value);
extern Value pop ();
extern Value peek (int distance);

extern void postVM ();
extern void bistVM ();
