#include "table.h"

#include "object.h"

#include <stdio.h>

/** @file table_bist.c
 * @brief Built-In Self Test for the TABLE module.
 */

/** Run all BIST cases for Table.
 */
void
bistTable ()
{
    printf ("BIST: %s ...\n", "bistTable");
    Value val;
    Table table;

    initTable (&table);
    INVAR (0 == table.count, "initTable should set count to zero.");
    INVAR (0 == table.capacity, "initTable should set capacity to zero.");
    INVAR (NULL == table.entries, "initTable should set entries to NULL.");

    ObjString *k1 = copyString ("k1", 3);
    bool k1b = tableSet (&table, k1, NUMBER_VAL (1.0));

    INVAR (k1b, "expecing true when adding k1");

    ObjString *k2 = copyString ("k2", 3);
    bool k2b = tableSet (&table, k2, NIL_VAL);

    INVAR (k2b, "expecing true when adding k2");

    ObjString *k3 = copyString ("k3", 3);
    bool k3b = tableSet (&table, k3, BOOL_VAL (true));

    INVAR (k3b, "expecing true when adding k3");

    k2b = tableSet (&table, k2, BOOL_VAL (false));
    INVAR (!k2b, "expecing false when updating k3");

    k2b = tableGet (&table, k2, &val);
    INVAR (k2b, "expecing true when getting k2");
    INVAR (IS_BOOL (val), "expecting to see Boolean from k2");
    INVAR (false == AS_BOOL (val), "expecting to see false from k2");

    k2b = tableDelete (&table, k2);
    INVAR (k2b, "expecing true when removing k2");

    k2b = tableSet (&table, k2, BOOL_VAL (false));
    INVAR (k2b, "expecing true when re-adding  k3");

    Table table2;

    initTable (&table2);

    ObjString *k4 = copyString ("k4", 4);
    bool k4b = tableSet (&table2, k4, BOOL_VAL (true));

    INVAR (k4b, "expecing true when adding k4");

    tableAddAll (&table, &table2);

    k1b = tableGet (&table2, k1, &val);
    INVAR (k1b, "expecing true when getting k1");
    INVAR (IS_NUMBER (val), "expecting to see Number from k1");
    INVAR (1.0 == AS_NUMBER (val), "expecting to see 1.0 from k1");

    k2b = tableGet (&table2, k2, &val);
    INVAR (k2b, "expecing true when getting k2");
    INVAR (IS_BOOL (val), "expecting to see Boolean from k2");
    INVAR (false == AS_BOOL (val), "expecting to see false from k2");

    k3b = tableGet (&table2, k3, &val);
    INVAR (k3b, "expecing true when getting k3");
    INVAR (IS_BOOL (val), "expecting to see Boolean from k3");
    INVAR (true == AS_BOOL (val), "expecting to see true from k3");

    k4b = tableGet (&table2, k4, &val);
    INVAR (k4b, "expecing true when getting k4");
    INVAR (IS_BOOL (val), "expecting to see Boolean from k4");
    INVAR (true == AS_BOOL (val), "expecting to see true from k4");

    // printObject (OBJ_VAL ((Obj *) k1));

    freeTable (&table);
    INVAR (0 == table.count, "freeTable should set count to zero.");
    INVAR (0 == table.capacity, "freeTable should set capacity to zero.");
    INVAR (NULL == table.entries, "freeTable should set entries to NULL.");

    freeTable (&table2);
    INVAR (0 == table2.count, "freeTable should set count to zero.");
    INVAR (0 == table2.capacity, "freeTable should set capacity to zero.");
    INVAR (NULL == table2.entries, "freeTable should set entries to NULL.");

    printf ("BIST: %s ... done.\n", "bistTable");
}
