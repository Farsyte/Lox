#pragma once

#include "common.h"
#include "value.h"

/** Hash Table Entry
 */
struct Entry {
    ObjString *key;             ///< key (must be interned)
    Value value;                ///< value (any object)
};

/** Hash table
 */
struct Table {
    int count;                  ///< number of entries including tombstones
    int capacity;               ///< total current capacity of entries
    Entry *entries;             ///< array of entries
};

extern void initTable (Table *table);
extern void freeTable (Table *table);
extern bool tableGet (Table *table, ObjString *key, Value *value);
extern bool tableSet (Table *table, ObjString *key, Value value);
extern bool tableDelete (Table *table, ObjString *key);
extern void tableAddAll (Table *from, Table *to);
extern ObjString *tableFindString (Table *table, const char *chars, int length, uint32_t hash);
extern void tableRemoveWhite (Table *table);
extern void markTable (Table *table);

extern void postTable ();
extern void bistTable ();
