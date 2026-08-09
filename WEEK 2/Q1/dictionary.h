/* dictionary.h
 *
 * Common interface for the six dictionary representations used in Q1.
 *
 * A dictionary supports seven primary operations:
 *     Search, Insert, Delete, Maximum, Minimum, Predecessor, Successor
 *
 * Every representation below implements all seven with the same meaning,
 * so they can be compared fairly:
 *
 *     unsorted_array.c    -> UnsortedArray   (ua...)
 *     sorted_array.c      -> SortedArray     (sa...)
 *     singly_unsorted.c   -> SinglyUnsorted  (su...)
 *     singly_sorted.c     -> SinglySorted    (ss...)
 *     doubly_unsorted.c   -> DoublyUnsorted  (du...)
 *     doubly_sorted.c     -> DoublySorted    (ds...)
 *
 * Delete is given the position/pointer of the item, not its key, so it
 * measures only the cost of removing - not the cost of finding.
 */

#ifndef DICTIONARY_H
#define DICTIONARY_H

#define NOPS 7      /* Search, Insert, Delete, Max, Min, Pred, Succ */

/* Index of each operation inside a result[] array */
#define OP_SEARCH  0
#define OP_INSERT  1
#define OP_DELETE  2
#define OP_MAX     3
#define OP_MIN     4
#define OP_PRED    5
#define OP_SUCC    6

/* Counts one unit of work (a comparison or a pointer/index move).
 * Defined in q1_dictionary_operations.c and shared by all six files. */
extern long long steps;

void resetSteps(void);

/* ------------------------------------------------------------------
 * 1. Unsorted array
 * ------------------------------------------------------------------ */
typedef struct
{
    int *data;
    int n;
} UnsortedArray;

void uaInit(UnsortedArray *d, int capacity);
void uaFree(UnsortedArray *d);
int  uaSearch(UnsortedArray *d, int key);
void uaInsert(UnsortedArray *d, int key);
void uaDelete(UnsortedArray *d, int pos);
int  uaMaximum(UnsortedArray *d);
int  uaMinimum(UnsortedArray *d);
int  uaPredecessor(UnsortedArray *d, int pos);
int  uaSuccessor(UnsortedArray *d, int pos);
void uaMeasure(int n, long long result[NOPS]);

/* ------------------------------------------------------------------
 * 2. Sorted array
 * ------------------------------------------------------------------ */
typedef struct
{
    int *data;
    int n;
} SortedArray;

void saInit(SortedArray *d, int capacity);
void saFree(SortedArray *d);
int  saSearch(SortedArray *d, int key);
void saInsert(SortedArray *d, int key);
void saDelete(SortedArray *d, int pos);
int  saMaximum(SortedArray *d);
int  saMinimum(SortedArray *d);
int  saPredecessor(SortedArray *d, int pos);
int  saSuccessor(SortedArray *d, int pos);
void saMeasure(int n, long long result[NOPS]);

/* ------------------------------------------------------------------
 * Node types for the linked lists
 * ------------------------------------------------------------------ */
typedef struct SNode
{
    int key;
    struct SNode *next;
} SNode;

typedef struct DNode
{
    int key;
    struct DNode *prev;
    struct DNode *next;
} DNode;

/* ------------------------------------------------------------------
 * 3. Singly linked unsorted list
 * ------------------------------------------------------------------ */
typedef struct
{
    SNode *head;
    SNode *tail;
    int n;
} SinglyUnsorted;

void   suInit(SinglyUnsorted *d);
void   suFree(SinglyUnsorted *d);
SNode *suSearch(SinglyUnsorted *d, int key);
void   suInsert(SinglyUnsorted *d, int key);
void   suDelete(SinglyUnsorted *d, SNode *x);
SNode *suMaximum(SinglyUnsorted *d);
SNode *suMinimum(SinglyUnsorted *d);
SNode *suPredecessor(SinglyUnsorted *d, SNode *x);
SNode *suSuccessor(SinglyUnsorted *d, SNode *x);
void   suMeasure(int n, long long result[NOPS]);

/* ------------------------------------------------------------------
 * 4. Singly linked sorted list
 * ------------------------------------------------------------------ */
typedef struct
{
    SNode *head;
    SNode *tail;
    int n;
} SinglySorted;

void   ssInit(SinglySorted *d);
void   ssFree(SinglySorted *d);
SNode *ssSearch(SinglySorted *d, int key);
void   ssInsert(SinglySorted *d, int key);
void   ssDelete(SinglySorted *d, SNode *x);
SNode *ssMaximum(SinglySorted *d);
SNode *ssMinimum(SinglySorted *d);
SNode *ssPredecessor(SinglySorted *d, SNode *x);
SNode *ssSuccessor(SinglySorted *d, SNode *x);
void   ssMeasure(int n, long long result[NOPS]);

/* ------------------------------------------------------------------
 * 5. Doubly linked unsorted list
 * ------------------------------------------------------------------ */
typedef struct
{
    DNode *head;
    DNode *tail;
    int n;
} DoublyUnsorted;

void   duInit(DoublyUnsorted *d);
void   duFree(DoublyUnsorted *d);
DNode *duSearch(DoublyUnsorted *d, int key);
void   duInsert(DoublyUnsorted *d, int key);
void   duDelete(DoublyUnsorted *d, DNode *x);
DNode *duMaximum(DoublyUnsorted *d);
DNode *duMinimum(DoublyUnsorted *d);
DNode *duPredecessor(DoublyUnsorted *d, DNode *x);
DNode *duSuccessor(DoublyUnsorted *d, DNode *x);
void   duMeasure(int n, long long result[NOPS]);

/* ------------------------------------------------------------------
 * 6. Doubly linked sorted list
 * ------------------------------------------------------------------ */
typedef struct
{
    DNode *head;
    DNode *tail;
    int n;
} DoublySorted;

void   dsInit(DoublySorted *d);
void   dsFree(DoublySorted *d);
DNode *dsSearch(DoublySorted *d, int key);
void   dsInsert(DoublySorted *d, int key);
void   dsDelete(DoublySorted *d, DNode *x);
DNode *dsMaximum(DoublySorted *d);
DNode *dsMinimum(DoublySorted *d);
DNode *dsPredecessor(DoublySorted *d, DNode *x);
DNode *dsSuccessor(DoublySorted *d, DNode *x);
void   dsMeasure(int n, long long result[NOPS]);

#endif
