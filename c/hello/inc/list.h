#pragma once

typedef struct lnode_s *list;
typedef const char *str;

extern void         list_test(void);

/** Return the data pointer from this node.
 *
 * Returns NULL if the passed L is the list header, else returns a
 * (const) pointer to the stored string.
 *
 * Fails if passed a NULL.
 */
extern str          list_data(list l);

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
extern list         list_next(list l);

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
extern list         list_prev(list l);

/** Create a new empty list.
 *
 * Returns a pointer to a node containing a
 * zero-length string.
 *
 * Fails if passed a NULL, or if out of memory.
 */
extern list         list_new();

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
extern list         list_insert(list l, str s);

/** Locate a string on the linked list.
 *
 * Searches the provided list for a node whose contents compares equal
 * to the given string. If passed the header node for the list,
 * searches the whole list; if passed a data node, starts after that
 * node, allowing the caller to find several nodes matching the given
 * string.
 *
 * If the string is not found, returns a pointer to a node L such
 * that list_data(L) is NULL (the HEAD node).
 */
extern list         list_find(list l, str s);

/** Delete the next matching data from the list.
 *
 * Starting after the given node (which is the start of
 * the list if the header node is given), find the next
 * node whose content matches the given string, remove
 * the node from the list, and release storage.
 *
 * If a node is removed, returns the pointer to the
 * node that was before it on the list. If no node was
 * removed, returns NULL.
 */

extern list         list_delete(list l, str s);

/** Free the whole list.
 *
 * Release all storage for the whole list that
 * contains the node L as either header or data.
 */
extern void         list_free(list l);
