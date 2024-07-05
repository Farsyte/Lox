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

void initTable (Table *table);
void freeTable (Table *table);

extern void postTable ();
extern void bistTable ();
