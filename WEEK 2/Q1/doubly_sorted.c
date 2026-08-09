/* doubly_sorted.c
 *
 * Dictionary implemented as a doubly linked sorted list.
 *
 * This is the strongest row of the table: five of the seven operations are
 * O(1).  Sorted order gives Minimum and Maximum through the head and tail
 * pointers, and the back pointer turns Predecessor into a single hop.
 *
 * The two that stay O(n) are the ones that need to locate a key:
 * Search (no random access, so no binary search) and Insert (the sorted
 * position has to be walked to).
 */

#include <stdio.h>
#include <stdlib.h>
#include "dictionary.h"

void dsInit(DoublySorted *d)
{
    d->head = NULL;
    d->tail = NULL;
    d->n = 0;
}

void dsFree(DoublySorted *d)
{
    DNode *cur = d->head;

    while (cur != NULL)
    {
        DNode *next = cur->next;
        free(cur);
        cur = next;
    }
    dsInit(d);
}

/* Search - walk in order and give up once the keys pass the target.
 * Still O(n) - ordering without random access does not allow a jump. */
DNode *dsSearch(DoublySorted *d, int key)
{
    for (DNode *cur = d->head; cur != NULL; cur = cur->next)
    {
        steps++;
        if (cur->key == key)
            return cur;
        if (cur->key > key)
            return NULL;
    }
    return NULL;
}

/* Insert - walk to the sorted position and link in both directions.  O(n) */
void dsInsert(DoublySorted *d, int key)
{
    DNode *node = (DNode *)malloc(sizeof(DNode));
    DNode *cur = d->head;
    DNode *prev = NULL;

    node->key = key;

    while (cur != NULL && cur->key < key)
    {
        steps++;
        prev = cur;
        cur = cur->next;
    }

    node->prev = prev;
    node->next = cur;

    if (prev == NULL)
        d->head = node;
    else
        prev->next = node;

    if (cur == NULL)
        d->tail = node;
    else
        cur->prev = node;

    d->n++;
}

/* Delete - unlink through the node's own pointers.  O(1) */
void dsDelete(DoublySorted *d, DNode *x)
{
    steps++;

    if (x->prev == NULL)
        d->head = x->next;
    else
        x->prev->next = x->next;

    if (x->next == NULL)
        d->tail = x->prev;
    else
        x->next->prev = x->prev;

    free(x);
    d->n--;
}

/* Maximum - sorted, so the tail is the answer.  O(1) */
DNode *dsMaximum(DoublySorted *d)
{
    steps++;
    return d->tail;
}

/* Minimum - the head.  O(1) */
DNode *dsMinimum(DoublySorted *d)
{
    steps++;
    return d->head;
}

/* Predecessor - one hop backwards.  This is the operation the singly
 * linked sorted list had to pay O(n) for.  O(1) */
DNode *dsPredecessor(DoublySorted *d, DNode *x)
{
    steps++;
    if (d->n == 0 || x == NULL)
        return NULL;
    return x->prev;
}

/* Successor - one hop forwards.  O(1) */
DNode *dsSuccessor(DoublySorted *d, DNode *x)
{
    steps++;
    if (d->n == 0 || x == NULL)
        return NULL;
    return x->next;
}

void dsMeasure(int n, long long result[NOPS])
{
    DoublySorted d;

    dsInit(&d);
    for (int i = 1; i <= n; i++)
        dsInsert(&d, i);

    resetSteps();
    dsSearch(&d, n + 1);
    result[OP_SEARCH] = steps;

    resetSteps();
    dsMaximum(&d);
    result[OP_MAX] = steps;

    resetSteps();
    dsMinimum(&d);
    result[OP_MIN] = steps;

    resetSteps();
    dsPredecessor(&d, d.tail);
    result[OP_PRED] = steps;

    resetSteps();
    dsSuccessor(&d, d.head);
    result[OP_SUCC] = steps;

    /* Worst case for Insert is a key larger than everything already there. */
    resetSteps();
    dsInsert(&d, n + 1);
    result[OP_INSERT] = steps;

    resetSteps();
    dsDelete(&d, d.tail);
    result[OP_DELETE] = steps;

    dsFree(&d);
}
