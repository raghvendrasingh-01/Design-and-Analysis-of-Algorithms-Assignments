/* singly_unsorted.c
 *
 * Dictionary implemented as a singly linked unsorted list.
 *
 * A tail pointer is kept, so appending is O(1).  But each node only knows
 * its successor, so removing a known node still costs a walk from the head
 * to find the node in front of it.
 */

#include <stdio.h>
#include <stdlib.h>
#include "dictionary.h"

void suInit(SinglyUnsorted *d)
{
    d->head = NULL;
    d->tail = NULL;
    d->n = 0;
}

void suFree(SinglyUnsorted *d)
{
    SNode *cur = d->head;

    while (cur != NULL)
    {
        SNode *next = cur->next;
        free(cur);
        cur = next;
    }
    suInit(d);
}

/* Search - walk the list.  O(n) */
SNode *suSearch(SinglyUnsorted *d, int key)
{
    for (SNode *cur = d->head; cur != NULL; cur = cur->next)
    {
        steps++;
        if (cur->key == key)
            return cur;
    }
    return NULL;
}

/* Insert - append at the tail, no order to respect.  O(1) */
void suInsert(SinglyUnsorted *d, int key)
{
    SNode *node = (SNode *)malloc(sizeof(SNode));

    steps++;
    node->key = key;
    node->next = NULL;

    if (d->tail == NULL)
        d->head = node;
    else
        d->tail->next = node;

    d->tail = node;
    d->n++;
}

/* Delete - we are handed the node, but not the one before it.  Finding
 * the previous node needs a walk from the head.  O(n) */
void suDelete(SinglyUnsorted *d, SNode *x)
{
    SNode *prev = NULL;
    SNode *cur = d->head;

    while (cur != NULL && cur != x)
    {
        steps++;
        prev = cur;
        cur = cur->next;
    }

    if (cur == NULL)
        return;

    if (prev == NULL)
        d->head = x->next;
    else
        prev->next = x->next;

    if (d->tail == x)
        d->tail = prev;

    free(x);
    d->n--;
}

/* Maximum - unordered, so look at every node.  O(n) */
SNode *suMaximum(SinglyUnsorted *d)
{
    SNode *best = d->head;

    for (SNode *cur = d->head; cur != NULL; cur = cur->next)
    {
        steps++;
        if (cur->key > best->key)
            best = cur;
    }
    return best;
}

/* Minimum - same walk.  O(n) */
SNode *suMinimum(SinglyUnsorted *d)
{
    SNode *best = d->head;

    for (SNode *cur = d->head; cur != NULL; cur = cur->next)
    {
        steps++;
        if (cur->key < best->key)
            best = cur;
    }
    return best;
}

/* Predecessor - largest key below x->key.  O(n) */
SNode *suPredecessor(SinglyUnsorted *d, SNode *x)
{
    SNode *best = NULL;

    for (SNode *cur = d->head; cur != NULL; cur = cur->next)
    {
        steps++;
        if (cur->key < x->key)
        {
            if (best == NULL || cur->key > best->key)
                best = cur;
        }
    }
    return best;
}

/* Successor - smallest key above x->key.  O(n) */
SNode *suSuccessor(SinglyUnsorted *d, SNode *x)
{
    SNode *best = NULL;

    for (SNode *cur = d->head; cur != NULL; cur = cur->next)
    {
        steps++;
        if (cur->key > x->key)
        {
            if (best == NULL || cur->key < best->key)
                best = cur;
        }
    }
    return best;
}

void suMeasure(int n, long long result[NOPS])
{
    SinglyUnsorted d;

    suInit(&d);
    for (int i = 1; i <= n; i++)
        suInsert(&d, i);

    /* Absent key, so the walk cannot stop early. */
    resetSteps();
    suSearch(&d, n + 1);
    result[OP_SEARCH] = steps;

    resetSteps();
    suMaximum(&d);
    result[OP_MAX] = steps;

    resetSteps();
    suMinimum(&d);
    result[OP_MIN] = steps;

    resetSteps();
    suPredecessor(&d, d.head);
    result[OP_PRED] = steps;

    resetSteps();
    suSuccessor(&d, d.head);
    result[OP_SUCC] = steps;

    resetSteps();
    suInsert(&d, n + 1);
    result[OP_INSERT] = steps;

    /* Worst case for Delete is the last node: the walk to find its
     * predecessor covers the whole list. */
    resetSteps();
    suDelete(&d, d.tail);
    result[OP_DELETE] = steps;

    suFree(&d);
}
