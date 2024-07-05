#include "table.h"

#include "memory.h"
#include "object.h"
#include "value.h"

#include <stdlib.h>
#include <string.h>

/** @file table.h
 * @brief hash table implementation
 */

void
initTable (Table *table)
{
    table->count = 0;
    table->capacity = 0;
    table->entries = NULL;
}

void
freeTable (Table *table)
{
    FREE_ARRAY (Entry, table->entries, table->capacity);

    initTable (table);
}
