/* singly_sorted.c
 *
 * Dictionary implemented as a singly linked sorted list.
 *
 * The keys are in ascending order, which makes Minimum, Maximum and
 * Successor cheap.  Predecessor is the interesting one: it is O(n) even
 * though Successor is O(1), because a node can reach forward but not back.
 * Adding a prev pointer is exactly what removes that asymmetry - see
 * doubly_sorted.c.
 */

#include <stdio.h>
#include <stdlib.h>
#include "dictionary.h"

void ssInit(SinglySorted *d)
{
    d->head = NULL;
    d->tail = NULL;
    d->n = 0;
}

void ssFree(SinglySorted *d)
{
    SNode *cur = d->head;

    while (cur != NULL)
    {
        SNode *next = cur->next;
        free(cur);
        cur = next;
    }
    ssInit(d);
}

/* Search - walk, but stop as soon as the keys grow past the target.
 * Still O(n): a linked list has no random access, so the ordering does
 * not buy binary search here. */
SNode *ssSearch(SinglySorted *d, int key)
{
    for (SNode *cur = d->head; cur != NULL; cur = cur->next)
    {
        steps++;
        if (cur->key == key)
            return cur;
        if (cur->key > key)
            return NULL;
    }
    return NULL;
}

/* Insert - walk to the correct position, then link in.  O(n) */
void ssInsert(SinglySorted *d, int key)
{
    SNode *node = (SNode *)malloc(sizeof(SNode));
    SNode *prev = NULL;
    SNode *cur = d->head;

    node->key = key;
    node->next = NULL;

    while (cur != NULL && cur->key < key)
    {
        steps++;
        prev = cur;
        cur = cur->next;
    }

    node->next = cur;

    if (prev == NULL)
        d->head = node;
    else
        prev->next = node;

    if (cur == NULL)
        d->tail = node;

    d->n++;
}

/* Delete - same problem as the unsorted singly linked list: the node in
 * front of x has to be found by walking.  O(n) */
void ssDelete(SinglySorted *d, SNode *x)
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

/* Maximum - the tail pointer already holds it.  O(1) */
SNode *ssMaximum(SinglySorted *d)
{
    steps++;
    return d->tail;
}

/* Minimum - the head.  O(1) */
SNode *ssMinimum(SinglySorted *d)
{
    steps++;
    return d->head;
}

/* Predecessor - the node whose next is x.  Nothing points backwards, so
 * this is a walk from the head.  O(n) */
SNode *ssPredecessor(SinglySorted *d, SNode *x)
{
    SNode *prev = NULL;

    for (SNode *cur = d->head; cur != NULL; cur = cur->next)
    {
        steps++;
        if (cur == x)
            return prev;
        prev = cur;
    }
    return NULL;
}

/* Successor - one pointer hop.  O(1) */
SNode *ssSuccessor(SinglySorted *d, SNode *x)
{
    steps++;
    if (d->n == 0 || x == NULL)
        return NULL;
    return x->next;
}

void ssMeasure(int n, long long result[NOPS])
{
    SinglySorted d;

    /* Insert 1..n.  Each new key is the largest so far, so the build walks
     * the list every time - this is the O(n^2) part of the harness. */
    ssInit(&d);
    for (int i = 1; i <= n; i++)
        ssInsert(&d, i);

    /* A key above every element forces the walk to reach the end. */
    resetSteps();
    ssSearch(&d, n + 1);
    result[OP_SEARCH] = steps;

    resetSteps();
    ssMaximum(&d);
    result[OP_MAX] = steps;

    resetSteps();
    ssMinimum(&d);
    result[OP_MIN] = steps;

    /* Worst case for Predecessor is the last node. */
    resetSteps();
    ssPredecessor(&d, d.tail);
    result[OP_PRED] = steps;

    resetSteps();
    ssSuccessor(&d, d.head);
    result[OP_SUCC] = steps;

    /* Worst case for Insert is a key larger than everything. */
    resetSteps();
    ssInsert(&d, n + 1);
    result[OP_INSERT] = steps;

    resetSteps();
    ssDelete(&d, d.tail);
    result[OP_DELETE] = steps;

    ssFree(&d);
}
