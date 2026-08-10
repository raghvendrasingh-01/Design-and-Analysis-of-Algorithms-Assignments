/* q1_dictionary_operations.c
 *
 * DAA Q1 : the seven dictionary operations under six representations.
 *
 * Rather than assert the table of worst-case running times, this program
 * implements all six representations, forces the worst case of every
 * operation, counts steps, and infers the growth class back from the
 * measurements.  All 42 cells are checked.
 *
 * Two families serve the six rows.  The two arrays differ only in whether
 * order is maintained; the four lists differ only in that and in whether a
 * back pointer may be read.  Two flags therefore generate all six rows -
 * not a trick of this file, but the shape of the claim table itself.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOPS     7      /* Search, Insert, Delete, Max, Min, Pred, Succ */
#define NSIZES   6
#define NSTRUCTS 6

#define OP_SEARCH  0
#define OP_INSERT  1
#define OP_DELETE  2
#define OP_MAX     3
#define OP_MIN     4
#define OP_PRED    5
#define OP_SUCC    6

/* One step is one key comparison or one element/pointer move.  Wall-clock
 * time would depend on the machine; step counts are exact and repeatable. */
long long steps = 0;

void resetSteps(void) { steps = 0; }

/* --- Array family: one struct, `sorted` picks the behaviour --- */

typedef struct { int *data; int n; int sorted; } Array;

void arrInit(Array *d, int capacity, int sorted) {
    d->data = (int *)malloc(sizeof(int) * capacity);
    d->n = 0;
    d->sorted = sorted;
}

void arrFree(Array *d) { free(d->data); d->data = NULL; d->n = 0; }

/* Search - binary when sorted (the only O(log n) cell in the table, and it
 * needs the random access only an array has), otherwise a full scan. */
int arrSearch(Array *d, int key) {
    if (d->sorted) {
        int low = 0, high = d->n - 1;
        while (low <= high) {
            int mid = (low + high) / 2;
            steps++;
            if (d->data[mid] == key) return mid;
            else if (d->data[mid] < key) low = mid + 1;
            else high = mid - 1;
        }
        return -1;
    }
    for (int i = 0; i < d->n; i++) {
        steps++;
        if (d->data[i] == key) return i;
    }
    return -1;
}

/* Insert - unsorted writes past the end, O(1).  Sorted binary-searches the
 * slot and shifts the tail right, and the shifting dominates: O(n). */
void arrInsert(Array *d, int key) {
    int low = 0, high = d->n - 1, pos;
    if (!d->sorted) { steps++; d->data[d->n] = key; d->n++; return; }

    while (low <= high) {
        int mid = (low + high) / 2;
        steps++;
        if (d->data[mid] < key) low = mid + 1;
        else high = mid - 1;
    }
    pos = low;
    for (int i = d->n; i > pos; i--) { steps++; d->data[i] = d->data[i - 1]; }
    d->data[pos] = key;
    d->n++;
}

/* Delete - unsorted drops the last element into the hole, O(1).  A sorted
 * array cannot: the order has to survive, so it shifts.  O(n) */
void arrDelete(Array *d, int pos) {
    if (!d->sorted) { steps++; d->data[pos] = d->data[d->n - 1]; d->n--; return; }
    for (int i = pos; i < d->n - 1; i++) { steps++; d->data[i] = d->data[i + 1]; }
    d->n--;
}

/* Maximum / Minimum - sorted reads an end slot, O(1); unsorted scans, O(n) */
int arrMaximum(Array *d) {
    int best = 0;
    if (d->sorted) { steps++; return d->n - 1; }
    for (int i = 0; i < d->n; i++) {
        steps++;
        if (d->data[i] > d->data[best]) best = i;
    }
    return best;
}

int arrMinimum(Array *d) {
    int best = 0;
    if (d->sorted) { steps++; return d->n == 0 ? -1 : 0; }
    for (int i = 0; i < d->n; i++) {
        steps++;
        if (d->data[i] < d->data[best]) best = i;
    }
    return best;
}

/* Predecessor / Successor - sorted reads the neighbouring slot, O(1);
 * unsorted must scan for the nearest key.  O(n) */
int arrPredecessor(Array *d, int pos) {
    int best = -1;
    if (d->sorted) { steps++; return (pos <= 0 || pos >= d->n) ? -1 : pos - 1; }
    for (int i = 0; i < d->n; i++) {
        steps++;
        if (d->data[i] < d->data[pos])
            if (best == -1 || d->data[i] > d->data[best]) best = i;
    }
    return best;
}

int arrSuccessor(Array *d, int pos) {
    int best = -1;
    if (d->sorted) { steps++; return (pos >= d->n - 1) ? -1 : pos + 1; }
    for (int i = 0; i < d->n; i++) {
        steps++;
        if (d->data[i] > d->data[pos])
            if (best == -1 || d->data[i] < d->data[best]) best = i;
    }
    return best;
}

/* Build at size n, then force and measure each operation's worst case.  The
 * two variants need different worst-case arguments, so each picks its own
 * below - sharing one would understate a row. */
void arrMeasure(int n, int sorted, long long result[NOPS]) {
    Array d;
    arrInit(&d, n + 2, sorted);
    for (int i = 1; i <= n; i++) arrInsert(&d, i);

    /* An absent key: no early exit, and binary search runs its full depth. */
    resetSteps(); arrSearch(&d, n + 1);  result[OP_SEARCH] = steps;
    resetSteps(); arrMaximum(&d);        result[OP_MAX]    = steps;
    resetSteps(); arrMinimum(&d);        result[OP_MIN]    = steps;

    /* Sorted: the far end, so the O(1) hop is not flattered by position 0. */
    resetSteps(); arrPredecessor(&d, sorted ? d.n - 1 : 0); result[OP_PRED] = steps;
    resetSteps(); arrSuccessor(&d, 0);                      result[OP_SUCC] = steps;

    /* Sorted: a key below everything, so every element shifts right, and
     * position 0 for Delete shifts the whole tail left.  For an unsorted
     * array both cost the same wherever they land. */
    resetSteps(); arrInsert(&d, sorted ? 0 : n + 1); result[OP_INSERT] = steps;
    resetSteps(); arrDelete(&d, 0);                  result[OP_DELETE] = steps;

    arrFree(&d);
}

/* --- List family: one node, `sorted` and `dbl` pick the behaviour --- */

/* The node carries `prev` for all four list rows, but the two singly linked
 * rows are *defined by never reading it* - which is exactly what makes their
 * Delete and Predecessor O(n).  Step counts are unaffected either way, since
 * steps are counted in traversals and comparisons, not in link writes. */
typedef struct Node { int key; struct Node *prev, *next; } Node;
typedef struct { Node *head, *tail; int n, sorted, dbl; } List;

void listInit(List *d, int sorted, int dbl) {
    d->head = d->tail = NULL;
    d->n = 0;
    d->sorted = sorted;
    d->dbl = dbl;
}

void listFree(List *d) {
    Node *cur = d->head;
    while (cur != NULL) { Node *next = cur->next; free(cur); cur = next; }
    listInit(d, d->sorted, d->dbl);
}

/* Search - a walk either way.  Sorted stops early once the keys pass the
 * target, but ordering without random access still cannot jump.  O(n) */
Node *listSearch(List *d, int key) {
    for (Node *cur = d->head; cur != NULL; cur = cur->next) {
        steps++;
        if (cur->key == key) return cur;
        if (d->sorted && cur->key > key) return NULL;
    }
    return NULL;
}

/* Insert - unsorted appends at the tail, O(1).  Sorted walks to the right
 * position first, O(n).  Both keep the tail pointer up to date. */
void listInsert(List *d, int key) {
    Node *node = (Node *)malloc(sizeof(Node));
    Node *cur = d->head, *prev = NULL;
    node->key = key;
    node->prev = node->next = NULL;
    if (!d->sorted) {
        steps++;
        node->prev = d->tail;
        if (d->tail == NULL) d->head = node;
        else d->tail->next = node;
        d->tail = node;
        d->n++;
        return;
    }
    while (cur != NULL && cur->key < key) { steps++; prev = cur; cur = cur->next; }

    node->prev = prev;
    node->next = cur;
    if (prev == NULL) d->head = node;
    else prev->next = node;
    if (cur == NULL) d->tail = node;
    else cur->prev = node;
    d->n++;
}

/* Delete - with a back pointer both neighbours are already reachable, so
 * unlinking is O(1).  Without one, the node in front has to be found by
 * walking from the head, which is O(n) even though the node is given. */
void listDelete(List *d, Node *x) {
    Node *prev = NULL, *cur;
    if (d->dbl) {
        steps++;
        if (x->prev == NULL) d->head = x->next;
        else x->prev->next = x->next;
        if (x->next == NULL) d->tail = x->prev;
        else x->next->prev = x->prev;
        free(x);
        d->n--;
        return;
    }
    cur = d->head;
    while (cur != NULL && cur != x) { steps++; prev = cur; cur = cur->next; }
    if (cur == NULL) return;
    if (prev == NULL) d->head = x->next;
    else prev->next = x->next;
    if (d->tail == x) d->tail = prev;
    free(x);
    d->n--;
}

/* Maximum / Minimum - sorted keeps them at tail and head, O(1); unsorted
 * must scan every node.  O(n) */
Node *listMaximum(List *d) {
    Node *best = d->head;
    if (d->sorted) { steps++; return d->tail; }
    for (Node *cur = d->head; cur != NULL; cur = cur->next) {
        steps++;
        if (cur->key > best->key) best = cur;
    }
    return best;
}

Node *listMinimum(List *d) {
    Node *best = d->head;
    if (d->sorted) { steps++; return d->head; }
    for (Node *cur = d->head; cur != NULL; cur = cur->next) {
        steps++;
        if (cur->key < best->key) best = cur;
    }
    return best;
}

/* Predecessor - three cases, and the middle one is the point of the table.
 *
 *   sorted + doubly : one hop backwards.                              O(1)
 *   sorted + singly : the keys are in order, so the predecessor is the node
 *                     whose next is x - but nothing points backwards, so it
 *                     has to be found by walking from the head.       O(n)
 *   unsorted        : list order is not key order, so a back pointer would
 *                     not help; scan for the largest key below x.     O(n)
 */
Node *listPredecessor(List *d, Node *x) {
    Node *best = NULL, *prev = NULL;
    if (d->sorted && d->dbl) {
        steps++;
        return (d->n == 0 || x == NULL) ? NULL : x->prev;
    }
    if (d->sorted) {
        for (Node *cur = d->head; cur != NULL; cur = cur->next) {
            steps++;
            if (cur == x) return prev;
            prev = cur;
        }
        return NULL;
    }
    for (Node *cur = d->head; cur != NULL; cur = cur->next) {
        steps++;
        if (cur->key < x->key)
            if (best == NULL || cur->key > best->key) best = cur;
    }
    return best;
}

/* Successor - sorted hops forward with or without a back pointer, O(1);
 * unsorted scans for the smallest key above x.  O(n) */
Node *listSuccessor(List *d, Node *x) {
    Node *best = NULL;
    if (d->sorted) {
        steps++;
        return (d->n == 0 || x == NULL) ? NULL : x->next;
    }
    for (Node *cur = d->head; cur != NULL; cur = cur->next) {
        steps++;
        if (cur->key > x->key)
            if (best == NULL || cur->key < best->key) best = cur;
    }
    return best;
}

void listMeasure(int n, int sorted, int dbl, long long result[NOPS]) {
    List d;
    /* Keys go in as 1..n.  For a sorted list each is the largest so far, so
     * the build walks the whole list every time: the O(n^2) part of the
     * harness. */
    listInit(&d, sorted, dbl);
    for (int i = 1; i <= n; i++) listInsert(&d, i);

    resetSteps(); listSearch(&d, n + 1); result[OP_SEARCH] = steps;
    resetSteps(); listMaximum(&d);       result[OP_MAX]    = steps;
    resetSteps(); listMinimum(&d);       result[OP_MIN]    = steps;

    /* Sorted: the last node, so the walk (or hop) runs full length.
     * Unsorted: the head, whose predecessor scan covers everything. */
    resetSteps(); listPredecessor(&d, sorted ? d.tail : d.head); result[OP_PRED] = steps;
    resetSteps(); listSuccessor(&d, d.head);                     result[OP_SUCC] = steps;

    /* A key above every element makes the sorted walk run full length; the
     * last node is the worst case when Delete must find the node in front
     * of it, and costs the same as any other when a back pointer exists. */
    resetSteps(); listInsert(&d, n + 1);  result[OP_INSERT] = steps;
    resetSteps(); listDelete(&d, d.tail); result[OP_DELETE] = steps;

    listFree(&d);
}

/* --- The claim, the measurements, and the comparison between them --- */

static int sizes[NSIZES] = {100, 200, 400, 800, 1600, 3200};

static char *structName[NSTRUCTS] = {
    "Unsorted array", "Sorted array", "Singly unsorted",
    "Singly sorted", "Doubly unsorted", "Doubly sorted"
};

static char *opName[NOPS] = {
    "Search", "Insert", "Delete", "Maximum", "Minimum", "Predecessor", "Successor"
};

/* Shorter labels so the wide claim table fits in 80 columns */
static char *opShort[NOPS] = {
    "Search", "Insert", "Delete", "Max", "Min", "Pred", "Succ"
};

/* The claimed worst-case running times */
static char *claim[NSTRUCTS][NOPS] = {
    /*              Search      Insert  Delete  Max     Min     Pred    Succ   */
    /* U array  */ {"O(n)",     "O(1)", "O(1)", "O(n)", "O(n)", "O(n)", "O(n)"},
    /* S array  */ {"O(log n)", "O(n)", "O(n)", "O(1)", "O(1)", "O(1)", "O(1)"},
    /* S unsort */ {"O(n)",     "O(1)", "O(n)", "O(n)", "O(n)", "O(n)", "O(n)"},
    /* S sorted */ {"O(n)",     "O(n)", "O(n)", "O(1)", "O(1)", "O(n)", "O(1)"},
    /* D unsort */ {"O(n)",     "O(1)", "O(1)", "O(n)", "O(n)", "O(n)", "O(n)"},
    /* D sorted */ {"O(n)",     "O(n)", "O(1)", "O(1)", "O(1)", "O(1)", "O(1)"}
};

/* measured[structure][operation][size] */
static long long measured[NSTRUCTS][NOPS][NSIZES];

/* Which family and which flags produce each row of the table above.
 * family 0 = array, 1 = list; then sorted, then dbl (unused by arrays). */
static int variant[NSTRUCTS][3] = {
    {0, 0, 0}, {0, 1, 0}, {1, 0, 0}, {1, 1, 0}, {1, 0, 1}, {1, 1, 1}
};

void measureOne(int s, int n, long long result[NOPS]) {
    if (variant[s][0] == 0) arrMeasure(n, variant[s][1], result);
    else listMeasure(n, variant[s][1], variant[s][2], result);
}

void printLine(char c, int width) {
    for (int i = 0; i < width; i++) printf("%c", c);
    printf("\n");
}

/* Every section opens with the same rule-title-rule block. */
void banner(char *nl, char *title) {
    printf("%s", nl); printLine('=', 69); printf(" %s\n", title); printLine('=', 69);
}

void printClaimTable(void) {
    banner("\n", "ASYMPTOTIC WORST-CASE RUNNING TIMES (the claim)");
    printf("%-17s", "Structure");
    for (int j = 0; j < NOPS; j++) printf("%-9s", opShort[j]);
    printf("\n");
    printLine('-', 80);

    for (int i = 0; i < NSTRUCTS; i++) {
        printf("%-17s", structName[i]);
        for (int j = 0; j < NOPS; j++) printf("%-9s", claim[i][j]);
        printf("\n");
    }
}

void printMeasurements(void) {
    banner("\n", "MEASURED WORST-CASE STEP COUNTS");
    for (int i = 0; i < NSTRUCTS; i++) {
        printf("\n%s\n", structName[i]);
        printLine('-', 62);

        printf("%-14s", "n =");
        for (int k = 0; k < NSIZES; k++) printf("%8d", sizes[k]);
        printf("\n");

        for (int j = 0; j < NOPS; j++) {
            printf("%-14s", opName[j]);
            for (int k = 0; k < NSIZES; k++) printf("%8lld", measured[i][j][k]);
            printf("\n");
        }
    }
}

/* Growth class from the last doubling of n: a ratio near 1 means the cost
 * ignores n (O(1)), near 2 means it doubles with n (O(n)), and in between
 * means doubling n only added a constant (O(log n)). */
char *inferClass(long long small, long long big) {
    double ratio;
    if (small <= 0) return "O(1)";
    ratio = (double)big / (double)small;
    if (ratio < 1.02) return "O(1)";
    else if (ratio < 1.5) return "O(log n)";
    else return "O(n)";
}

int printValidation(void) {
    int matches = 0;
    banner("\n", "VALIDATION : CLAIMED CLASS vs CLASS INFERRED FROM MEASUREMENT");
    printf("%-17s %-13s %7s  %-9s %-9s %s\n",
           "Structure", "Operation", "Ratio", "Claimed", "Observed", "Verdict");
    printLine('-', 72);
    for (int i = 0; i < NSTRUCTS; i++) {
        for (int j = 0; j < NOPS; j++) {
            long long small = measured[i][j][NSIZES - 2];
            long long big = measured[i][j][NSIZES - 1];
            double ratio = (small > 0) ? (double)big / (double)small : 1.0;
            char *observed = inferClass(small, big);
            int ok = (strcmp(claim[i][j], observed) == 0);

            if (ok) matches++;
            printf("%-17s %-13s %7.3f  %-9s %-9s %s\n",
                   structName[i], opName[j], ratio,
                   claim[i][j], observed, ok ? "MATCH" : "DIFFER");
        }
    }

    printLine('-', 72);
    printf("Agreement: %d of %d claims reproduced by measurement.\n",
           matches, NSTRUCTS * NOPS);
    return matches;
}

int main(void) {
    banner("", "DAA Q1 : DICTIONARY OPERATIONS ACROSS SIX REPRESENTATIONS");
    printClaimTable();

    printf("\nMeasuring ");
    for (int i = 0; i < NSTRUCTS; i++) {
        for (int k = 0; k < NSIZES; k++) {
            long long result[NOPS];

            measureOne(i, sizes[k], result);
            for (int j = 0; j < NOPS; j++) measured[i][j][k] = result[j];
        }
        printf(".");
        fflush(stdout);
    }
    printf(" done\n");

    printMeasurements();
    printValidation();

    printf("\nTime Complexity  : see the claim table above, per structure\n");
    printf("                   and per operation\n");
    printf("Space Complexity : O(n) for every representation.  A singly\n");
    printf("                   linked list adds one pointer per node, a\n");
    printf("                   doubly linked list adds two.\n");

    return 0;
}
