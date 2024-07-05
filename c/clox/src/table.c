#include "table.h"

#include "memory.h"
#include "object.h"
#include "value.h"

#include <stdlib.h>
#include <string.h>

/** Load Factor for Tables.
 *
 * Tune based on benchmarking if there is evidence that
 * we are getting too many collisions.
 */
#define TABLE_MAX_LOAD 0.75

/** @file table.h
 * @brief hash table implementation
 */

/** Initialize the hash table.
 *
 * This leaves the table containing no elements,
 * with no capacity, and owning no other memory.
 *
 * @param table the hash table of interest
 */
void
initTable (Table *table)
{
    table->count = 0;
    table->capacity = 0;
    table->entries = NULL;
}

/** Free the hash table.
 *
 * This releases any memory owned by the table,
 * and restores it to its initial state.
 *
 * @param table the hash table of interest
 */
void
freeTable (Table *table)
{
    FREE_ARRAY (Entry, table->entries, table->capacity);

    initTable (table);
}

/** Find a key in the hash table.
 *
 * If the key is in the table, return the pointer to
 * its bucket. If it is not, return a pointer to the
 * empty bucket to use.
 *
 * @param entries the entry array
 * @param capacity the size of the array in entries
 * @param key the string to use for the key
 * @return the hash table bucket to use
 */
static Entry *
findEntry (Entry *entries, int capacity, ObjString *key)
{
    uint32_t index = key->hash % capacity;

    for (;;) {
        Entry *entry = &entries[index];

        // Put a pin in the "entry->key == key" thing for later.
        if (entry->key == key || entry->key == NULL) {
            return entry;
        }
        index = (index + 1) % capacity;
    }
}

/** Adjust the capacity of the table.
 *
 * The entry count is nearing the load factor times the
 * capacity, expand the table. Note that the entries in
 * the old table must be "rehashed" into the larger table.
 *
 * @param table the table of interest
 * @param capacity the desired new capacity
 */
static void
adjustCapacity (Table *table, int capacity)
{
    (void) table;
    (void) capacity;
    STUB (0);
}

/** Add a key/value pair to the table.
 *
 * @param table the hash table of interest
 * @param key the string to use as the key
 * @param value the value to store for that key
 * @return true if this is a new key
 */
bool
tableSet (Table *table, ObjString *key, Value value)
{
    if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
        int capacity = GROW_CAPACITY (table->capacity);

        adjustCapacity (table, capacity);
    }
    Entry *entry = findEntry (table->entries, table->capacity, key);
    bool isNewKey = entry->key == NULL;

    if (isNewKey)
        table->count++;
    entry->key = key;
    entry->value = value;
    return isNewKey;
}
