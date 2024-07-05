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
    Entry *tombstone = NULL;

    for (;;) {
        Entry *entry = &entries[index];

        if (entry->key == NULL) {
            if (IS_NIL (entry->value)) {
                // Empty entry.
                return tombstone != NULL ? tombstone : entry;
            } else {
                // We found a tombstone.
                if (tombstone == NULL)
                    tombstone = entry;
            }
        } else if (entry->key == key) {
            // Put a pin in the "entry->key == key" thing for later.
            // We found the key.
            return entry;
        }
        index = (index + 1) % capacity;
    }
}

/** Get the value associated with this key in this table
 *
 * @param table the hash table of interest
 * @param key the string to use as the key
 * @param value where to return the value for that key
 * @return true if the key was in the table
 */
bool
tableGet (Table *table, ObjString *key, Value *value)
{
    if (table->count == 0)
        return false;

    Entry *entry = findEntry (table->entries, table->capacity, key);

    if (entry->key == NULL)
        return false;

    *value = entry->value;
    return true;
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
    Entry *entries = ALLOCATE (Entry, capacity);

    for (int i = 0; i < capacity; i++) {
        entries[i].key = NULL;
        entries[i].value = NIL_VAL;
    }

    table->count = 0;
    for (int i = 0; i < table->capacity; i++) {
        Entry *entry = &table->entries[i];

        if (entry->key == NULL)
            continue;
        Entry *dest = findEntry (entries, capacity, entry->key);

        dest->key = entry->key;
        dest->value = entry->value;
        table->count++;
    }

    FREE_ARRAY (Entry, table->entries, table->capacity);

    table->entries = entries;
    table->capacity = capacity;
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

    if (isNewKey && IS_NIL (entry->value))
        table->count++;
    entry->key = key;
    entry->value = value;
    return isNewKey;
}

/** Remove the entry for this key from this table.
 *
 * Replaces the entry with a tombstone.
 *
 * @param table the table of interest
 * @param key the key to remove
 * @return true if found, false if not.
 */
bool
tableDelete (Table *table, ObjString *key)
{
    if (table->count == 0)
        return false;

    // find the entry.
    Entry *entry = findEntry (table->entries, table->capacity, key);

    if (entry->key == NULL)
        return false;

    // Place a tombstone.
    entry->key = NULL;
    entry->value = BOOL_VAL (true);
    return true;
}

/** Add all entries from one table to another.
 *
 * @param from table to read
 * @param to table to update
 */
void
tableAddAll (Table *from, Table *to)
{
    for (int i = 0; i < from->capacity; ++i) {
        Entry *entry = &from->entries[i];

        if (entry->key != NULL) {
            tableSet (to, entry->key, entry->value);
        }
    }
}

/** Search the table for a key equal to this string.
 *
 * This function looks for the given string as a key in the table,
 * using a string compare as part of the equality condition (this is
 * used for interning strings).
 *
 * @param table the table of interest
 * @param chars the string to add to the table
 * @param length number of bytes in the string
 * @param hash precomputed hash value
 */
ObjString *
tableFindString (Table *table, const char *chars, int length, uint32_t hash)
{
    if (table->count == 0)
        return NULL;
    uint32_t index = hash % table->capacity;

    for (;;) {
        Entry *entry = &table->entries[index];

        if (entry->key == NULL) {
            // Stop if we find an empty non-tombstone entry.
            if (IS_NIL (entry->value))
                return NULL;
        } else if (entry->key->length == length && entry->key->hash == hash && 0 == memcmp (entry->key->chars, chars, length)) {
            // Found a key that matches.
            return entry->key;
        }
        index = (index + 1) % table->capacity;
    }
}
