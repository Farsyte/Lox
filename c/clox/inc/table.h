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
extern bool tableSet (Table *table, ObjString *key, Value value);

extern void postTable ();
extern void bistTable ();
