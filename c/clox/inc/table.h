#pragma once

#include "common.h"
#include "value.h"

/** Hash Table Entry */
struct Entry {
    ObjString *key;
    Value value;
};

/** Hash table
 */
struct Table {
    int count;
    int capacity;
    Entry *entries;
};

extern void initTable (Table *table);
extern void freeTable (Table *table);
extern bool tableGet (Table *table, ObjString *key, Value *value);
extern bool tableSet (Table *table, ObjString *key, Value value);
extern bool tableDelete (Table *table, ObjString *key);
extern void tableAddAll (Table *from, Table *to);
extern ObjString *tableFindString (Table *table, const char *chars, int length, uint32_t hash);
extern void postTable ();
extern void bistTable ();
