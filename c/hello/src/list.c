#include "list.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

// string stored in heap memory.
typedef char       *hstr;

struct lnode_s {
    list                next;
    list                prev;
    hstr                that;
};

static list         list_ctor(str that);
static list         list_link(list n, list l);
static list         list_del(list l);

/** Return the data pointer from this node.
 *
 * Returns NULL if the passed L is the list header, else returns a
 * (const) pointer to the stored string.
 *
 * Fails if passed a NULL.
 */
str list_data(list l)
{
    assert(l);
    return l->that;
}

/** Step to the next node.
 *
 * If given the list header, retuns the node pointer for the first
 * item on the list. If given such a node pointer, returns the next
 * node. If given the LAST node, returns the header node.
 *
 * Note that list_data(l) returns NULL if L is the header node.
 *
 * Fails if passed a NULL.
 */
list list_next(list l)
{
    assert(NULL != l);
    return l->next;
}

/** Step to the previous node.
 *
 * If given the list header, retuns the node pointer for the final
 * item on the list. If given such a node pointer, returns the
 * previous node. If given the FIRST node, returns the header node.
 *
 * Note that list_data(l) returns NULL if L is the header node.
 *
 * Fails if passed a NULL.
 */
list list_prev(list l)
{
    assert(NULL != l);
    return l->prev;
}

/** Create a new empty list.
 *
 * Returns a pointer to a node containing a
 * zero-length string.
 *
 * Fails if passed a NULL, or if out of memory.
 */
list list_new()
{
    return list_ctor(0);
}

/** Insert data onto linked list
 *
 * Makes a copy of the data into new memory allocated from the heap,
 * and adds a new node to the linked list just before the L node.
 *
 * Consequently: If L is the list header, *APPENDS* a node containing
 * a heap-allocated copy of s onto the list; If L is a pointer to a
 * found node, *INSERT* a node before it containing a heap-allocated
 * copy of s onto the list.
 *
 * Fails if passed a NULL, or if out of memory.
 */
list list_insert(list l, str s)
{
    assert(NULL != l);
    assert(NULL != s);
    list                n = list_ctor(s);
    assert(NULL != n);
    return list_link(n, l);
}

/** Locate a string on the linked list.
 *
 * Searches the provided list for a node whose contents compares equal
 * to the given string. If passed the header node for the list,
 * searches the whole list; if passed a data node, starts after that
 * node, allowing the caller to find several nodes matching the given
 * string.
 *
 * If the string is not found, returns NULL.
 */
list list_find(list l, str s)
{
    assert(NULL != l);
    assert(NULL != s);

    for (l = l->next; l->that; l = l->next) {
	assert(NULL != l);
	if (0 == strcmp(s, l->that))
	    return l;
    }

    return NULL;
}

/** Delete the next matching data from the list.
 *
 * Starting after the given node (which is the start of
 * the list if the header node is given), find the next
 * node whose content matches the given string, remove
 * the node from the list, and release storage.
 *
 * If a node is removed, returns the pointer to the
 * node that was before it on the list. If no node was
 * removed, returns the pointer to the HEAD node.
 */

list list_delete(list l, str s)
{
    assert(NULL != l);
    assert(NULL != s);
    list                f = list_find(l, s);
    if (NULL == f)
	return NULL;
    return list_del(f);
}

/** Free the whole list.
 *
 * Release all storage for the whole list that
 * contains the node L as either header or data.
 */
void list_free(list l)
{
    while (l)
	l = list_del(l->next);
}

/** Create a new node.
 *
 * If the parameter is NULL, the node contains NULL;
 * otherwise, the node contains a duplicate of the
 * provided string stored on the heap.
 *
 *
 * The node and the duplicate string are allocated
 * separately.
 */
static list list_ctor(str that)
{
    list                n = (list) malloc(sizeof *n);
    assert(NULL != n);

    hstr                d = 0;
    if (that) {
	d = strdup(that);
	assert(NULL != d);
    }

    n->next = n;
    n->prev = n;
    n->that = d;
    return n;
}

/** Insert list node N before list node L.
 *
 * Note that if L is a list header (with null data),
 * then this is appenending the string to the end;
 * if L is the result of a find, this inserts the
 * new node before the found node.
 *
 * If either input is NULL the function returns NULL.
 */
static list list_link(list n, list l)
{
    assert(NULL != n);
    assert(NULL != l);
    list                p = l->prev;
    assert(NULL != p);
    n->next = l;
    n->prev = p;
    p->next = n;
    l->prev = n;
    return n;
}

/** Unlink and free a node.
 *
 * Returns NULL if the node was self-linked,
 * otherwise returns the node's PREV link.
 */
static list list_del(list l)
{
    assert(l);
    list                n = l->next;
    list                p = l->prev;

    n->prev = p;
    p->next = n;
    free(l->that);
    free(l);

    return (l == p) ? NULL : p;
}
