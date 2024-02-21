
#ifndef FMSH_COMMON_LIST_H
#define FMSH_COMMON_LIST_H

#ifdef __cplusplus
extern "C" {    // allow C++ to use these headers
#endif

// INLINE definition for the ARM C compiler
#define INLINE inline //__inline

/****h* include.list
 * NAME
 *  fmsh_list.h -- simple double linked list implementation
 * DESCRIPTION
 *  Some of the internal functions ("__fmsh_xxx") are useful when
 *  manipulating whole lists rather than single entries, as
 *  sometimes we already know the next/prev entries and we can
 *  generate better code by using them directly rather than
 *  using the generic single-entry routines.
 ***/

/****is* include.fmsh_list_head
 * DESCRIPTION
 *  This is the structure used for managing linked lists.
 * SOURCE
 */
typedef struct fmsh_list_head
{
    struct fmsh_list_head *next, *prev;
}FMSH_listHead;
/*****/

#define FMSH_LIST_HEAD_INIT(name) { &(name), &(name) }

#define FMSH_LIST_HEAD(name) \
    FMSH_listHead name = FMSH_LIST_HEAD_INIT(name)

#define FMSH_INIT_LIST_HEAD(ptr)                  \
do {                                            \
    (ptr)->next = (ptr); (ptr)->prev = (ptr);   \
} while (0)

/****if* include.list/__fmsh_list_add
 * DESCRIPTION
 *  Insert a new entry between two known consecutive entries. 
 *  This is only for internal list manipulation where we know
 *  the prev/next entries already!
 * ARGUMENTS
 *  new         element to insert
 *  prev        previous entry
 *  next        next entry
 * SOURCE
 */
static INLINE void __FMSH_listAdd(FMSH_listHead *new,
                                  FMSH_listHead *prev,
                                  FMSH_listHead *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}
/*****/

/****f* include.list/fmsh_list_add
 * DESCRIPTION
 *  Insert a new entry after the specified head.
 *  This is good for implementing stacks.
 * ARGUMENTS
 *  new         new entry to be added
 *  head        list head to add it after
 * SOURCE
 */
static INLINE void FMSH_listAdd(FMSH_listHead *new, FMSH_listHead *head)
{
    __FMSH_listAdd(new, head, head->next);
}
/*****/

/****f* include.list/fmsh_list_add_tail
 * DESCRIPTION
 *  Insert a new entry before the specified head.
 *  This is useful for implementing queues.
 * ARGUMENTS
 *  new         new entry to be added
 *  head        list head to add it before
 * SOURCE
 */
static INLINE void FMSH_listAddTail(FMSH_listHead *new, FMSH_listHead *head)
{
    __FMSH_listAdd(new, head->prev, head);
}
/*****/

/****if* include.list/__fmsh_list_del
 * DESCRIPTION
 *  Delete a list entry by making the prev/next entries point to each
 *  other.  This is only for internal list manipulation where we know
 *  the prev/next entries already!
 * ARGUMENTS
 *  prev        previous entry
 *  next        next entry
 * SOURCE
 */
static INLINE void __FMSH_listDel(FMSH_listHead *prev, FMSH_listHead *next)
{
    next->prev = prev;
    prev->next = next;
}
/*****/

/****f* include.list/fmsh_list_del
 * DESCRIPTION
 *  Deletes entry from list.
 * ARGUMENTS
 *  entry       the element to delete from the list
 * NOTES
 *  list_empty on entry does not return TRUE after this, the entry
 *  is in an undefined state.
 * SOURCE
 */
static INLINE void FMSH_listDel(FMSH_listHead *entry)
{
    __FMSH_listDel(entry->prev, entry->next);
}
/*****/

/****f* include.list/fmsh_list_del_init
 * DESCRIPTION
 *  Deletes entry from list and reinitializes it.
 * ARGUMENTS
 *  entry       the element to delete from the list
 * SOURCE
 */
static INLINE void FMSH_listDelInit(FMSH_listHead *entry)
{
    __FMSH_listDel(entry->prev, entry->next);
    FMSH_INIT_LIST_HEAD(entry); 
}
/*****/

/****f* include.list/fmsh_list_empty
 * DESCRIPTION
 *  Tests whether a list is empty.
 * ARGUMENTS
 *  head        the list to test
 * SOURCE
 */
static INLINE int FMSH_listEmpty(FMSH_listHead *head)
{
    return head->next == head;
}
/*****/

/****f* include.list/fmsh_list_splice
 * DESCRIPTION
 *  Join two lists.
 * ARGUMENTS
 *  list        the new list to add
 *  head        the place to add it in the first list
 * SOURCE
 */
static INLINE void FMSH_listSplice(FMSH_listHead *list, FMSH_listHead *head)
{
    FMSH_listHead *first = list->next;

    if (first != list) {
        FMSH_listHead *last = list->prev;
        FMSH_listHead *at = head->next;

        first->prev = head;
        head->next = first;

        last->next = at;
        at->prev = last;
    }
}
/*****/

/****d* include.list/fmsh_list_entry
 * DESCRIPTION
 *  Get the struct for this entry.
 * ARGUMENTS
 *  ptr         the &FMSH_listHead pointer
 *  type        the type of the struct this is embedded in
 *  member      the name of the list_struct within the struct
 * SOURCE
 */
#define FMSH_LIST_ENTRY(ptr, type, member) \
    ((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))
/*****/

/****d* include.list/fmsh_list_for_each
 * DESCRIPTION
 *  Iterate over a list.
 * ARGUMENTS
 *  pos         the &FMSH_listHead to use as a loop counter
 *  head        the head for your list
 * SOURCE
 */
#define FMSH_LIST_FOR_EACH(pos, head)                             \
    for(pos = (head)->next; pos != (head); pos = pos->next)
/*****/

/****d* include.list/fmsh_list_for_each_safe
 * SYNOPSIS
 *  list_for_each_safe(pos, head)
 * DESCRIPTION
 *  Iterate over a list safe against removal of list entry.
 * ARGUMENTS
 *  pos         the &FMSH_listHead to use as a loop counter
 *  n           another &FMSH_listHead to use as temporary storage
 *  head        the head for your list
 * SOURCE
 */
#define FMSH_LIST_FOR_EACH_SAFE(pos, n, head)                     \
    for(pos = (head)->next, n = pos->next; pos != (head);       \
        pos = n, n = pos->next)
/*****/

/****d* include.list/fmsh_list_for_each_prev
 * DESCRIPTION
 *  Iterate over a list in reverse order.
 * ARGUMENTS
 *  pos         the &FMSH_listHead to use as a loop counter
 *  head        the head for your list
 * SOURCE
 */
#define FMSH_LIST_FOR_EACH_PREV(pos, head)                        \
    for(pos = (head)->prev; pos != (head); pos = pos->prev)
/*****/

#ifdef __cplusplus
}
#endif

#endif      // FMSH_COMMON_LIST_H

