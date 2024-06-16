#include "list.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>

void list_test(void)
{
    list                l1 = list_new();
    assert(NULL != l1);

    list                l2 = list_new();
    assert(NULL != l2);

    assert(l1 != l2);

    list                i1 = list_insert(l1, "foo");
    assert(NULL != i1);
    assert(l1 != i1);
    assert(l2 != i1);

    list                i2 = list_insert(l2, "bar");
    assert(NULL != i2);
    assert(l1 != i2);
    assert(l2 != i2);

    list                i3 = list_insert(l1, "baz");
    assert(NULL != i3);
    assert(l1 != i3);
    assert(l2 != i3);

    list                f1 = list_find(l1, "foo");
    assert(NULL != f1);
    assert(0 == strcmp("foo", list_data(f1)));
    assert(i1 == f1);

    list                f1x = list_find(l2, "foo");
    assert(NULL == f1x);

    list                f2 = list_find(l2, "bar");
    assert(NULL != f2);
    assert(0 == strcmp("bar", list_data(f2)));
    assert(i2 == f2);

    list                f2x = list_find(l1, "bar");
    assert(NULL == f2x);

    list                f3 = list_find(l1, "baz");
    assert(NULL != f3);
    assert(0 == strcmp("baz", list_data(f3)));
    assert(i3 == f3);

    list                f3x = list_find(l2, "baz");
    assert(NULL == f3x);

    list                d3 = list_delete(l1, "baz");
    assert(i1 == d3);

    list                f3d = list_find(l1, "baz");
    assert(NULL == f3d);

    list                d3x = list_delete(l2, "baz");
    assert(NULL == d3x);

    list                f3dx = list_find(l1, "baz");
    assert(NULL == f3dx);


    list_free(l1);
    list_free(l2);

    printf("list_test complete.\n");
}
