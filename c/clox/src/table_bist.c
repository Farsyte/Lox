#include "table.h"

/** @file table_bist.c
 * @brief Built-In Self Test for the TABLE module.
 */

/** Run all BIST cases for Table.
 */
void
bistTable ()
{
    Table table;

    initTable (&table);
    INVAR (0 == table.count, "initTable should set count to zero.");
    INVAR (0 == table.capacity, "initTable should set capacity to zero.");
    INVAR (NULL == table.entries, "initTable should set entries to NULL.");

    freeTable (&table);
    INVAR (0 == table.count, "freeTable should set count to zero.");
    INVAR (0 == table.capacity, "freeTable should set capacity to zero.");
    INVAR (NULL == table.entries, "freeTable should set entries to NULL.");

}
