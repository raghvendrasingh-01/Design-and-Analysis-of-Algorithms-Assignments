/* doubly_unsorted.c
 *
 * Dictionary implemented as a doubly linked unsorted list.
 *
 * The back pointer is what makes Delete O(1): given the node, both of its
 * neighbours are reachable immediately, so nothing has to be searched for.
 * That is the only entry that improves over the singly linked unsorted
 * list - being unordered still costs O(n) everywhere else.
 */

#include <stdio.h>
#include <stdlib.h>
#include "dictionary.h"

void duInit(DoublyUnsorted *d)
{
    d->head = NULL;
    d->tail = NULL;
    d->n = 0;
}

void duFree(DoublyUnsorted *d)
{
    DNode *cur = d->head;

    while (cur != NULL)
    {
        DNode *next = cur->next;
        free(cur);
        cur = next;
    }
    duInit(d);
}

/* Search - walk the list.  O(n) */
DNode *duSearch(DoublyUnsorted *d, int key)
{
    for (DNode *cur = d->head; cur != NULL; cur = cur->next)
    {
        steps++;
        if (cur->key == key)
            return cur;
    }
    return NULL;
}

/* Insert - append at the tail.  O(1) */
void duInsert(DoublyUnsorted *d, int key)
{
    DNode *node = (DNode *)malloc(sizeof(DNode));

    steps++;
    node->key = key;
    node->next = NULL;
    node->prev = d->tail;

    if (d->tail == NULL)
        d->head = node;
    else
        d->tail->next = node;

    d->tail = node;
    d->n++;
}

/* Delete - unlink using the node's own prev and next.  No walking.  O(1) */
void duDelete(DoublyUnsorted *d, DNode *x)
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

/* Maximum - unordered, so scan.  O(n) */
DNode *duMaximum(DoublyUnsorted *d)
{
    DNode *best = d->head;

    for (DNode *cur = d->head; cur != NULL; cur = cur->next)
    {
        steps++;
        if (cur->key > best->key)
            best = cur;
    }
    return best;
}

/* Minimum - scan.  O(n) */
DNode *duMinimum(DoublyUnsorted *d)
{
    DNode *best = d->head;

    for (DNode *cur = d->head; cur != NULL; cur = cur->next)
    {
        steps++;
        if (cur->key < best->key)
            best = cur;
    }
    return best;
}

/* Predecessor - the list order is not the key order, so the back pointer
 * is no help.  Scan for the largest key below x->key.  O(n) */
DNode *duPredecessor(DoublyUnsorted *d, DNode *x)
{
    DNode *best = NULL;

    for (DNode *cur = d->head; cur != NULL; cur = cur->next)
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
DNode *duSuccessor(DoublyUnsorted *d, DNode *x)
{
    DNode *best = NULL;

    for (DNode *cur = d->head; cur != NULL; cur = cur->next)
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

void duMeasure(int n, long long result[NOPS])
{
    DoublyUnsorted d;

    duInit(&d);
    for (int i = 1; i <= n; i++)
        duInsert(&d, i);

    resetSteps();
    duSearch(&d, n + 1);
    result[OP_SEARCH] = steps;

    resetSteps();
    duMaximum(&d);
    result[OP_MAX] = steps;

    resetSteps();
    duMinimum(&d);
    result[OP_MIN] = steps;

    resetSteps();
    duPredecessor(&d, d.head);
    result[OP_PRED] = steps;

    resetSteps();
    duSuccessor(&d, d.head);
    result[OP_SUCC] = steps;

    resetSteps();
    duInsert(&d, n + 1);
    result[OP_INSERT] = steps;

    /* Any node costs the same here, so the tail is as good as any. */
    resetSteps();
    duDelete(&d, d.tail);
    result[OP_DELETE] = steps;

    duFree(&d);
}
