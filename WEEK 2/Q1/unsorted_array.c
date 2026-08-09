/* unsorted_array.c
 *
 * Dictionary implemented as an unsorted array.
 *
 * Nothing is kept in order, so every "find" style operation has to look at
 * all n elements.  In exchange, adding and removing are both O(1).
 */

#include <stdio.h>
#include <stdlib.h>
#include "dictionary.h"

void uaInit(UnsortedArray *d, int capacity)
{
    d->data = (int *)malloc(sizeof(int) * capacity);
    d->n = 0;
}

void uaFree(UnsortedArray *d)
{
    free(d->data);
    d->data = NULL;
    d->n = 0;
}

/* Search - check each element in turn.  O(n) */
int uaSearch(UnsortedArray *d, int key)
{
    for (int i = 0; i < d->n; i++)
    {
        steps++;
        if (d->data[i] == key)
            return i;
    }
    return -1;
}

/* Insert - write past the last element, no order to maintain.  O(1) */
void uaInsert(UnsortedArray *d, int key)
{
    steps++;
    d->data[d->n] = key;
    d->n++;
}

/* Delete - move the last element into the hole.  No shifting.  O(1) */
void uaDelete(UnsortedArray *d, int pos)
{
    steps++;
    d->data[pos] = d->data[d->n - 1];
    d->n--;
}

/* Maximum - no order to exploit, so scan everything.  O(n) */
int uaMaximum(UnsortedArray *d)
{
    int best = 0;

    for (int i = 0; i < d->n; i++)
    {
        steps++;
        if (d->data[i] > d->data[best])
            best = i;
    }
    return best;
}

/* Minimum - same scan as Maximum.  O(n) */
int uaMinimum(UnsortedArray *d)
{
    int best = 0;

    for (int i = 0; i < d->n; i++)
    {
        steps++;
        if (d->data[i] < d->data[best])
            best = i;
    }
    return best;
}

/* Predecessor - largest key smaller than data[pos].  O(n) */
int uaPredecessor(UnsortedArray *d, int pos)
{
    int best = -1;

    for (int i = 0; i < d->n; i++)
    {
        steps++;
        if (d->data[i] < d->data[pos])
        {
            if (best == -1 || d->data[i] > d->data[best])
                best = i;
        }
    }
    return best;
}

/* Successor - smallest key larger than data[pos].  O(n) */
int uaSuccessor(UnsortedArray *d, int pos)
{
    int best = -1;

    for (int i = 0; i < d->n; i++)
    {
        steps++;
        if (d->data[i] > d->data[pos])
        {
            if (best == -1 || d->data[i] < d->data[best])
                best = i;
        }
    }
    return best;
}

/* Build a dictionary of size n and count the steps of each operation.
 *
 * The keys are inserted as 1..n.  That order does not matter here: this
 * representation never assumes any ordering, and each worst-case operation
 * below scans the whole array anyway.
 */
void uaMeasure(int n, long long result[NOPS])
{
    UnsortedArray d;

    uaInit(&d, n + 2);
    for (int i = 1; i <= n; i++)
        uaInsert(&d, i);

    /* Worst case for Search is a key that is absent: no early exit. */
    resetSteps();
    uaSearch(&d, n + 1);
    result[OP_SEARCH] = steps;

    resetSteps();
    uaMaximum(&d);
    result[OP_MAX] = steps;

    resetSteps();
    uaMinimum(&d);
    result[OP_MIN] = steps;

    resetSteps();
    uaPredecessor(&d, 0);
    result[OP_PRED] = steps;

    resetSteps();
    uaSuccessor(&d, 0);
    result[OP_SUCC] = steps;

    resetSteps();
    uaInsert(&d, n + 1);
    result[OP_INSERT] = steps;

    resetSteps();
    uaDelete(&d, 0);
    result[OP_DELETE] = steps;

    uaFree(&d);
}
