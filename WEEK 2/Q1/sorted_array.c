/* sorted_array.c
 *
 * Dictionary implemented as a sorted array.
 *
 * This is the only representation in the whole table that can do Search in
 * O(log n), because binary search needs random access and an array is the
 * only structure here that provides it.  The price is Insert and Delete,
 * which must shift elements to keep the order intact.
 */

#include <stdio.h>
#include <stdlib.h>
#include "dictionary.h"

void saInit(SortedArray *d, int capacity)
{
    d->data = (int *)malloc(sizeof(int) * capacity);
    d->n = 0;
}

void saFree(SortedArray *d)
{
    free(d->data);
    d->data = NULL;
    d->n = 0;
}

/* Search - binary search.  O(log n) */
int saSearch(SortedArray *d, int key)
{
    int low = 0, high = d->n - 1;

    while (low <= high)
    {
        int mid = (low + high) / 2;
        steps++;

        if (d->data[mid] == key)
            return mid;
        else if (d->data[mid] < key)
            low = mid + 1;
        else
            high = mid - 1;
    }
    return -1;
}

/* Insert - find the slot by binary search, then shift the tail right.
 * The shifting dominates, so O(n). */
void saInsert(SortedArray *d, int key)
{
    int low = 0, high = d->n - 1;
    int pos;

    while (low <= high)
    {
        int mid = (low + high) / 2;
        steps++;

        if (d->data[mid] < key)
            low = mid + 1;
        else
            high = mid - 1;
    }
    pos = low;

    for (int i = d->n; i > pos; i--)
    {
        steps++;
        d->data[i] = d->data[i - 1];
    }

    d->data[pos] = key;
    d->n++;
}

/* Delete - shift the tail left to close the hole.  Order must survive,
 * so the unsorted-array trick of overwriting with the last element is
 * not allowed here.  O(n) */
void saDelete(SortedArray *d, int pos)
{
    for (int i = pos; i < d->n - 1; i++)
    {
        steps++;
        d->data[i] = d->data[i + 1];
    }
    d->n--;
}

/* Maximum - the order is already known, so just read the last slot.  O(1) */
int saMaximum(SortedArray *d)
{
    steps++;
    return d->n - 1;
}

/* Minimum - the first slot.  O(1) */
int saMinimum(SortedArray *d)
{
    steps++;
    if (d->n == 0)
        return -1;
    return 0;
}

/* Predecessor - the neighbour on the left.  O(1) */
int saPredecessor(SortedArray *d, int pos)
{
    steps++;
    if (pos <= 0 || pos >= d->n)
        return -1;
    return pos - 1;
}

/* Successor - the neighbour on the right.  O(1) */
int saSuccessor(SortedArray *d, int pos)
{
    steps++;
    if (pos >= d->n - 1)
        return -1;
    return pos + 1;
}

/* Build a sorted array of size n and count the steps of each operation. */
void saMeasure(int n, long long result[NOPS])
{
    SortedArray d;

    /* Insert 1..n.  Each key is already the largest so far, so building
     * costs no shifting - the expensive insert is measured separately. */
    saInit(&d, n + 2);
    for (int i = 1; i <= n; i++)
        saInsert(&d, i);

    /* Worst case for Search is an absent key: binary search runs the full
     * depth of the array instead of stopping early on a hit. */
    resetSteps();
    saSearch(&d, n + 1);
    result[OP_SEARCH] = steps;

    resetSteps();
    saMaximum(&d);
    result[OP_MAX] = steps;

    resetSteps();
    saMinimum(&d);
    result[OP_MIN] = steps;

    resetSteps();
    saPredecessor(&d, d.n - 1);
    result[OP_PRED] = steps;

    resetSteps();
    saSuccessor(&d, 0);
    result[OP_SUCC] = steps;

    /* Worst case for Insert is a key smaller than everything: every
     * element has to move one slot right. */
    resetSteps();
    saInsert(&d, 0);
    result[OP_INSERT] = steps;

    /* Worst case for Delete is position 0: the whole tail shifts left. */
    resetSteps();
    saDelete(&d, 0);
    result[OP_DELETE] = steps;

    saFree(&d);
}
