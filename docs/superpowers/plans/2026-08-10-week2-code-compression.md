# Week 2 Code Compression Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Compress each Week 2 question into a single C file within budget (Q1 ≤500, Q2 ≤226, Q3 ≤240 lines), then update every document that describes the old layout and delete three plots the code cannot produce.

**Architecture:** Q1's eight files collapse to one by replacing six near-duplicate representations with two flag-parameterised families — an `Array` with a `sorted` flag and a `List` with `sorted` and `dbl` flags — which is the actual shape of the claim table, not a trick of the refactor. Q2 and Q3 keep every algorithm verbatim and save lines through dense style plus folding near-identical printers into one table-driven routine each. All three sources are already written and verified; this plan installs them.

**Tech Stack:** C99, GCC with `-Wall -Wextra` (`-lm` for Q2/Q3), GNU coreutils for verification, git.

---

## Critical Constraints

Read these before starting any task. They come from the approved spec at
`docs/superpowers/specs/2026-08-10-week2-code-compression-design.md`.

1. **Output stays byte-identical.** Q1 prints 143 lines, Q2 132, Q3 91. Every task
   that touches a source diffs its output against a captured baseline. **Any
   difference fails the task** and is investigated, not accepted.
2. **No measurement is removed.** Q1 keeps all 42 measured cells. Q2 keeps 3 input
   classes × 6 sizes × 4 counters plus both tables. Q3 keeps both sweeps, all 6 `k`
   values, all 5 `n` values, every normalisation column.
3. **Nothing is compiled or run inside the repository.** All building and running
   happens in the scratchpad. Week 2 is stdout-only, but the discipline holds.
4. **Line savings come from structure and style, never from deleting evidence.**
5. **Week 1 is out of scope entirely.** Do not edit any file under `WEEK 1/`.

### Paths used throughout

| Name | Path |
|---|---|
| Repo | `/home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments` |
| Scratchpad | `/tmp/claude-1000/-home-raghvendra-singh-Design-and-Analysis-of-Algorithms-Assignments/11840e73-9cf9-0b67-8136-53d0bb954b3c/scratchpad` |
| Verified sources | `<scratchpad>/build/q1.c`, `q2.c`, `q3.c` |
| Baselines | `<scratchpad>/baseline/q1.out` (143 lines), `q2.out` (132), `q3.out` (91) |

**If the scratchpad has been cleared:** the three sources are embedded verbatim in
Tasks 1–3 below, so they can be recreated from this document alone. The baselines
must then be recaptured *from the committed sources at `HEAD` before any source is
modified* — never after. Recapture with:

```bash
SCRATCH=/tmp/claude-1000/-home-raghvendra-singh-Design-and-Analysis-of-Algorithms-Assignments/11840e73-9cf9-0b67-8136-53d0bb954b3c/scratchpad
mkdir -p "$SCRATCH/baseline" && cd "$SCRATCH/baseline"
cp /home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments/"WEEK 2"/Q1/*.c /home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments/"WEEK 2"/Q1/*.h .
gcc -Wall -Wextra *.c -o q1 && ./q1 > q1.out
gcc -Wall -Wextra /home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments/"WEEK 2"/Q2/q2_merge_sort_variants.c -o q2 -lm && ./q2 > q2.out
gcc -Wall -Wextra /home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments/"WEEK 2"/Q3/q3_merge_k_sorted_arrays.c -o q3 -lm && ./q3 > q3.out
wc -l q1.out q2.out q3.out    # expect 143, 132, 91
```

### Verification protocol

Every source task runs this, in the scratchpad, before committing:

1. Compile with `-Wall -Wextra`. **Zero warnings is a requirement, not a preference.**
2. Run; require exit status 0.
3. Run a second time and `diff` the two outputs — confirms determinism survived.
4. `diff` against the captured baseline. Any difference fails the change.
5. `wc -l` the source and record it against its budget.
6. `diff` the verified output against the output section already committed in
   `sample.txt`. **Expect no change to the file.** A dirty diff means the
   compression broke something — investigate it, do not overwrite `sample.txt`.

---

## File Structure

| File | Disposition |
|---|---|
| `WEEK 2/Q1/q1_dictionary_operations.c` | **Rewritten** — 217 → 499 lines, absorbs all six representations and the header. Keeps its name, so no README link to it breaks. |
| `WEEK 2/Q1/dictionary.h` | **Deleted** |
| `WEEK 2/Q1/unsorted_array.c` | **Deleted** |
| `WEEK 2/Q1/sorted_array.c` | **Deleted** |
| `WEEK 2/Q1/singly_unsorted.c` | **Deleted** |
| `WEEK 2/Q1/singly_sorted.c` | **Deleted** |
| `WEEK 2/Q1/doubly_unsorted.c` | **Deleted** |
| `WEEK 2/Q1/doubly_sorted.c` | **Deleted** |
| `WEEK 2/Q2/q2_merge_sort_variants.c` | **Rewritten** — 382 → 226 lines |
| `WEEK 2/Q3/q3_merge_k_sorted_arrays.c` | **Rewritten** — 401 → 240 lines |
| `WEEK 2/Q2/plots/4_input_classes.png` | **Deleted** — plots 4 input classes incl. "Worst"; code defines 3 |
| `WEEK 2/Q2/plots/6_k_way_constant.png` | **Deleted** — plots a tournament merge; code implements 2-way and 3-way only |
| `WEEK 2/Q3/plots/4_input_classes.png` | **Deleted** — plots *comparisons* over Interleaved/Random/Blocked; Q3 counts only moves, builds random arrays only |
| `WEEK 2/Q1/README.md` | Modified — header table, File Layout, Approach, Space Complexity, Committed Artefacts (new), Build and Run, Files |
| `WEEK 2/Q2/README.md` | Modified — Approach, Committed Artefacts (new) |
| `WEEK 2/Q3/README.md` | Modified — Committed Artefacts (new) |
| `README.md` (root) | Modified — tree, question table, Features, Requirements, UTF-8 note, blockquote, build blocks, Conventions |
| `WEEK 2/Q*/sample.txt` | **Verified unchanged.** Not edited. |

### Section naming decision

The spec calls the new README section "Plots". **Use `## Committed Artefacts`
instead.** That is the established repo convention: all four Week 1 READMEs use it,
it sits in the same slot (between `## Sample Explanation` and `## Build and Run`),
and the root README documents it at lines 180–183 as "a committed-artefacts section
wherever a question ships generated data". Introducing a second name for the same
thing would contradict that sentence. Content is exactly what the spec asked for.

---

## Task 1: Compress Q1 to a single file

**Files:**
- Create: `WEEK 2/Q1/q1_dictionary_operations.c` (overwrite the existing 217-line driver)
- Delete: `WEEK 2/Q1/dictionary.h`, `unsorted_array.c`, `sorted_array.c`, `singly_unsorted.c`, `singly_sorted.c`, `doubly_unsorted.c`, `doubly_sorted.c`
- Verify against: `<scratchpad>/baseline/q1.out`

**Design notes.** Two structs serve six rows. The `variant[6][3]` table maps each
row of the claim table to `(family, sorted, dbl)`. Every row still gets its own
instance, its own worst-case construction, its own seven measurements and its own
printed block. The merged `Node` carries `prev` even for the two singly linked
rows — those rows are *defined by never reading it*, which is exactly what makes
their Delete and Predecessor O(n). Step counts are unaffected, because steps are
counted in traversals and comparisons, not link writes. A comment at the `Node`
definition says so, and Task 5 adds the matching note to the README.

- [ ] **Step 1: Confirm the baseline exists and reads 143 lines**

```bash
SCRATCH=/tmp/claude-1000/-home-raghvendra-singh-Design-and-Analysis-of-Algorithms-Assignments/11840e73-9cf9-0b67-8136-53d0bb954b3c/scratchpad
wc -l "$SCRATCH/baseline/q1.out"
```

Expected: `143 .../baseline/q1.out`. If the file is missing, recapture per
"If the scratchpad has been cleared" above **before** touching any source.

- [ ] **Step 2: Write the new single-file source**

Write the following 499 lines to `WEEK 2/Q1/q1_dictionary_operations.c`, replacing
its entire contents.

```c
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
```

- [ ] **Step 3: Check the line count**

```bash
wc -l "/home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments/WEEK 2/Q1/q1_dictionary_operations.c"
```

Expected: `499`. Budget is 500. If it reads anything else, the file was not
written exactly — rewrite it, do not adjust the budget.

- [ ] **Step 4: Delete the seven now-absorbed files**

```bash
cd "/home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments/WEEK 2/Q1"
git rm dictionary.h unsorted_array.c sorted_array.c singly_unsorted.c \
       singly_sorted.c doubly_unsorted.c doubly_sorted.c
ls
```

Expected `ls` output: `README.md  plots  q1_dictionary_operations.c  sample.txt`

- [ ] **Step 5: Compile in the scratchpad, requiring zero warnings**

```bash
SCRATCH=/tmp/claude-1000/-home-raghvendra-singh-Design-and-Analysis-of-Algorithms-Assignments/11840e73-9cf9-0b67-8136-53d0bb954b3c/scratchpad
mkdir -p "$SCRATCH/verify" && cd "$SCRATCH/verify"
gcc -Wall -Wextra "/home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments/WEEK 2/Q1/q1_dictionary_operations.c" -o q1 2>&1 | tee q1.warn
wc -l < q1.warn
```

Expected: gcc prints nothing, `q1.warn` is 0 lines. Any warning fails this task.

- [ ] **Step 6: Run it, check exit status, determinism, and the baseline**

```bash
cd "$SCRATCH/verify"
./q1 > q1.out; echo "exit=$?"
./q1 > q1b.out
diff q1.out q1b.out && echo "DETERMINISTIC"
diff q1.out "$SCRATCH/baseline/q1.out" && echo "BASELINE CLEAN"
wc -l < q1.out
grep -c 'MATCH' q1.out
grep 'Agreement:' q1.out
```

Expected, in order: `exit=0`, `DETERMINISTIC`, `BASELINE CLEAN`, `143`, `42`,
`Agreement: 42 of 42 claims reproduced by measurement.`

**If the baseline diff is dirty, stop.** The compression changed behaviour.
Investigate the diff; do not proceed and do not edit the baseline.

- [ ] **Step 7: Verify `sample.txt` needs no edit**

`sample.txt` is 160 lines: a 17-line wrapper plus the 143 lines of output. Confirm
the committed output section still matches what the new binary prints.

```bash
cd "$SCRATCH/verify"
tail -n 143 "/home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments/WEEK 2/Q1/sample.txt" > sample_body.txt
diff sample_body.txt q1.out && echo "SAMPLE CLEAN — no edit needed"
```

Expected: `SAMPLE CLEAN — no edit needed`. **Do not overwrite `sample.txt`** — a
dirty diff here is a bug in the compression, not a file to regenerate.

- [ ] **Step 8: Commit**

```bash
cd /home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments
git add "WEEK 2/Q1/q1_dictionary_operations.c"
git commit -m "refactor(week2-q1): compress eight files into one 499-line source

Replace six near-identical representations with two flag-parameterised
families: an Array with a sorted flag, and a List with sorted and dbl
flags. All 42 measured cells are preserved and the output is byte-identical
to the previous build.

Co-Authored-By: Claude <noreply@anthropic.com>"
```

---

## Task 2: Compress Q2 to 226 lines

**Files:**
- Modify: `WEEK 2/Q2/q2_merge_sort_variants.c` (382 → 226 lines, full rewrite)
- Verify against: `<scratchpad>/baseline/q2.out`

**Design notes.** Nothing about what the program computes changes. Three folds pay
for the 156 lines: the four result arrays `comp2`/`comp3`/`move2`/`move3` become
`comp[2][...]` and `mov[2][...]` indexed by variant; a `SortFn` function-pointer
table lets `printDemo` and `measure` each run one loop over both variants instead
of two straight-line blocks; and `printTable(cls, normalise)` — previously called
six times from `main` — becomes `printTables(normalise)`, called twice, looping
over classes internally with a `cell[4]` array per row. `printRecurrences` states
both recurrences from one `for (int b = 2; b <= 3; b++)` loop. The `n == 2` base
case in `mergeSort3` is preserved exactly: it is what guarantees `third >= 1` and
stops the recursion looping forever.

**226 is the agreed ceiling**, above the 200 target, because Q2's measurement
coverage is not negotiable. The file lands exactly on it.

- [ ] **Step 1: Confirm the baseline reads 132 lines**

```bash
SCRATCH=/tmp/claude-1000/-home-raghvendra-singh-Design-and-Analysis-of-Algorithms-Assignments/11840e73-9cf9-0b67-8136-53d0bb954b3c/scratchpad
wc -l "$SCRATCH/baseline/q2.out"
```

Expected: `132`.

- [ ] **Step 2: Write the new source**

Write the following 226 lines to `WEEK 2/Q2/q2_merge_sort_variants.c`, replacing
its entire contents.

```c
/* q2_merge_sort_variants.c
 *
 * DAA Q2 : ordinary merge sort against a three-way merge sort.  Both are
 * instrumented over three input classes and six sizes, and printed raw and
 * divided by n log2(n) - the normalisation that stays flat exactly when
 * the growth really is O(n log n).
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NSIZES   6
#define NCLASSES 3

/* Counters, reset before every sort */
long long comparisons = 0, moves = 0;

void resetCounters(void) { comparisons = 0; moves = 0; }
static int sizes[NSIZES] = {1000, 2000, 4000, 8000, 16000, 32000};
static char *className[NCLASSES] = {"Random", "Already sorted", "Reverse sorted"};

/* Results.  [variant][class][size], variant 0 = two-way. */
static long long comp[2][NCLASSES][NSIZES], mov[2][NCLASSES][NSIZES];

/* --- Ordinary merge sort: split in half ---
 * Merge the sorted halves [lo,mid) and [mid,hi) into temp, then copy back */
void merge2(int a[], int temp[], int lo, int mid, int hi) {
    int i = lo, j = mid, t = lo;
    while (i < mid && j < hi) {
        comparisons++;
        if (a[i] <= a[j]) temp[t++] = a[i++];
        else temp[t++] = a[j++];
        moves++;
    }
    while (i < mid) { temp[t++] = a[i++]; moves++; }   /* tail of either run: */
    while (j < hi)  { temp[t++] = a[j++]; moves++; }   /* moves, no comparisons */
    for (int k = lo; k < hi; k++) a[k] = temp[k];
}

void mergeSort2(int a[], int temp[], int lo, int hi) {
    int n = hi - lo, mid = lo + (hi - lo) / 2;
    if (n < 2) return;
    mergeSort2(a, temp, lo, mid);
    mergeSort2(a, temp, mid, hi);
    merge2(a, temp, lo, mid, hi);
}

/* --- Modified merge sort: split into thirds ---
 * Merge the three sorted runs [lo,m1), [m1,m2) and [m2,hi) into temp.
 * pos[r] is the head of run r, end[r] where it stops.  Each round keeps the
 * smallest head: the first live run is free, every live run after it costs
 * one comparison.  So a round costs 2 with all three alive, 1 with two and
 * none with one - the exact extra cost of splitting three ways not two. */
void merge3(int a[], int temp[], int lo, int m1, int m2, int hi) {
    int pos[3] = {lo, m1, m2}, end[3] = {m1, m2, hi}, t = lo;
    while (t < hi) {
        int pick = -1;
        for (int r = 0; r < 3; r++) {
            if (pos[r] >= end[r]) continue;
            if (pick < 0) pick = r;
            else { comparisons++; if (a[pos[r]] < a[pos[pick]]) pick = r; }
        }
        temp[t++] = a[pos[pick]++];
        moves++;
    }
    for (int x = lo; x < hi; x++) a[x] = temp[x];
}

void mergeSort3(int a[], int temp[], int lo, int hi) {
    int n = hi - lo, third = n / 3, m1 = lo + third, m2 = lo + 2 * third;
    if (n < 2) return;
    /* Two elements cannot be split into three non-empty parts, so handle
     * that directly.  This also guarantees third >= 1 below, which is what
     * stops the recursion from looping forever. */
    if (n == 2) {
        comparisons++;
        if (a[lo] > a[lo + 1]) { int t = a[lo]; a[lo] = a[lo+1]; a[lo+1] = t; moves += 2; }
        return;
    }
    mergeSort3(a, temp, lo, m1);
    mergeSort3(a, temp, m1, m2);
    mergeSort3(a, temp, m2, hi);
    merge3(a, temp, lo, m1, m2, hi);
}

/* The two variants share a signature, so one table drives both wherever the
 * program runs the same block once per variant. */
typedef void (*SortFn)(int[], int[], int, int);
static SortFn sortFn[2] = {mergeSort2, mergeSort3};
static char *vName[2] = {"two-way", "three-way"};

/* --- Helpers ---
 * class 0 random (fixed seed, so runs repeat), 1 already sorted, 2 reversed */
void buildInput(int a[], int n, int cls) {
    if (cls == 0) srand(42);
    for (int i = 0; i < n; i++)
        a[i] = (cls == 0) ? rand() % (n * 10) : (cls == 1) ? i : n - i;
}

int isSorted(int a[], int n) {
    for (int i = 1; i < n; i++) if (a[i - 1] > a[i]) return 0;
    return 1; }
void printLine(int w) { while (w-- > 0) printf("-"); printf("\n"); }
void printHeading(char *nl, char *title) {
    printf("%s=====================================================\n %s\n"
           "=====================================================\n", nl, title);
}

void printArray(int a[], int n) { for (int i = 0; i < n; i++) printf("%d ", a[i]); printf("\n"); }

/* --- Output sections ---
 * Sort one small array both ways so the reader can see they agree. */
void printDemo(void) {
    int demo[12] = {38, 27, 43, 3, 9, 82, 10, 1, 55, 20, 7, 64};
    int a[2][12], temp[12];
    char *label[2] = {"Two-way  ", "Three-way"};
    printHeading("\n", "DEMONSTRATION ON A SMALL ARRAY");
    printf("Input          : "); printArray(demo, 12);
    for (int v = 0; v < 2; v++) {
        for (int i = 0; i < 12; i++) a[v][i] = demo[i];
        resetCounters();
        sortFn[v](a[v], temp, 0, 12);
        printf("%s sort : ", label[v]); printArray(a[v], 12);
        printf("                 %lld comparisons, %lld moves\n", comparisons, moves);
    }
    printf("\nBoth outputs sorted and identical : %s\n",
           (isSorted(a[0], 12) && isSorted(a[1], 12)) ? "YES" : "NO");
}

/* Both recurrences are the same shape: only the branching factor and the
 * depth differ, so one loop states both. */
void printRecurrences(void) {
    printHeading("\n", "THE TWO RECURRENCES");
    for (int b = 2; b <= 3; b++)
        printf("%s merge sort\n  T(n) = %d T(n/%d) + O(n)\n  Master theorem : a = %d,"
               " b = %d, so n^(log_b a) = n^1 = n.\n  The merge cost O(n) matches, so"
               " T(n) = O(n log n).\n  Depth of recursion = %s.\n\n",
               b == 2 ? "Ordinary" : "Three-way", b, b, b, b,
               b == 2 ? "log2 n" : "log3 n = log2 n / log2 3");
    printf("ANSWER: the modified merge sort is still O(n log n) in the\nworst case."
           "  Splitting into thirds only changes the base of\nthe logarithm, and a"
           " change of base is a constant factor\n(1 / log2 3 = %.4f), which"
           " O-notation absorbs.\n", 1.0 / (log(3.0) / log(2.0)));
}

/* One table of counts per input class.  With `normalise` set, every count is
 * divided by n log2 n: flat if the growth really is O(n log n), rising if not. */
void printTables(int normalise) {
    for (int cls = 0; cls < NCLASSES; cls++) {
        printf("\n%s input%s\n", className[cls],
               normalise ? " - counts divided by n log2(n)" : "");
        printLine(70);
        printf("%10s %14s %14s %14s %14s\n",
               "n", "2-way comps", "3-way comps", "2-way moves", "3-way moves");
        printLine(70);
        for (int k = 0; k < NSIZES; k++) {
            double n = (double)sizes[k], s = n * (log(n) / log(2.0));
            long long cell[4] = {comp[0][cls][k], comp[1][cls][k],
                                 mov[0][cls][k],  mov[1][cls][k]};
            printf("%10d", sizes[k]);
            for (int c = 0; c < 4; c++)
                if (normalise) printf(" %14.3f", cell[c] / s);
                else printf(" %14lld", cell[c]);
            printf("\n");
        }
    }
}

void printConclusion(void) {
    int last = NSIZES - 1;
    double n = (double)sizes[last], scale = n * (log(n) / log(2.0));
    printHeading("\n", "CONCLUSION");
    printf("At n = %d on random input:\n\n", sizes[last]);
    printf("  comparisons / (n log2 n)   2-way %.3f   3-way %.3f\n",
           comp[0][0][last] / scale, comp[1][0][last] / scale);
    printf("  moves       / (n log2 n)   2-way %.3f   3-way %.3f\n\n",
           mov[0][0][last] / scale, mov[1][0][last] / scale);
    printf("Both ratios are flat as n grows, which is what O(n log n)\n"
           "looks like when measured.  Neither variant escapes the class.\n\n"
           "The three-way version makes fewer moves, because it has\n"
           "log3 n levels instead of log2 n and every level copies n\n"
           "elements.  It makes more comparisons, because choosing the\n"
           "smallest of three heads costs 2 comparisons instead of 1.\n"
           "The two effects nearly cancel, and neither changes the\nasymptotic answer.\n"
           "\nTime Complexity  : O(n log n) for both variants\n"
           "Space Complexity : O(n) for both - one temporary array of n\n"
           "                   integers, plus O(log n) recursion stack\n");
}

/* Both variants at one size, four counts.  Input is rebuilt per variant, so
 * neither ever sees the other's leftovers. */
void measure(int cls, int k) {
    int n = sizes[k];
    int *a = (int *)malloc(sizeof(int) * n);
    int *temp = (int *)malloc(sizeof(int) * n);
    for (int v = 0; v < 2; v++) {
        buildInput(a, n, cls);
        resetCounters();
        sortFn[v](a, temp, 0, n);
        comp[v][cls][k] = comparisons; mov[v][cls][k] = moves;
        if (!isSorted(a, n))
            printf("\nERROR: %s sort failed at n = %d\n", vName[v], n);
    }
    free(a); free(temp);
}

int main(void) {
    printHeading("", "DAA Q2 : MERGE SORT vs THREE-WAY MERGE SORT");
    printDemo();
    printRecurrences();

    printf("\nMeasuring ");
    for (int c = 0; c < NCLASSES; c++) {
        for (int k = 0; k < NSIZES; k++) measure(c, k);
        printf("."); fflush(stdout);
    }
    printf(" done\n");

    printHeading("\n", "MEASURED COUNTS");
    printTables(0);
    printHeading("\n", "NORMALISED BY n log2(n)");
    printTables(1);
    printConclusion();
    return 0;
}
```

- [ ] **Step 3: Check the line count**

```bash
wc -l "/home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments/WEEK 2/Q2/q2_merge_sort_variants.c"
```

Expected: `226`, exactly at the ceiling.

- [ ] **Step 4: Compile with `-lm`, requiring zero warnings**

```bash
SCRATCH=/tmp/claude-1000/-home-raghvendra-singh-Design-and-Analysis-of-Algorithms-Assignments/11840e73-9cf9-0b67-8136-53d0bb954b3c/scratchpad
cd "$SCRATCH/verify"
gcc -Wall -Wextra "/home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments/WEEK 2/Q2/q2_merge_sort_variants.c" -o q2 -lm 2>&1 | tee q2.warn
wc -l < q2.warn
```

Expected: no gcc output, `q2.warn` is 0 lines. `-lm` is required for `log()`.

- [ ] **Step 5: Run it, check exit status, determinism, and the baseline**

```bash
cd "$SCRATCH/verify"
./q2 > q2.out; echo "exit=$?"
./q2 > q2b.out
diff q2.out q2b.out && echo "DETERMINISTIC"
diff q2.out "$SCRATCH/baseline/q2.out" && echo "BASELINE CLEAN"
wc -l < q2.out
grep 'Both outputs sorted and identical' q2.out
```

Expected: `exit=0`, `DETERMINISTIC`, `BASELINE CLEAN`, `132`, and
`Both outputs sorted and identical : YES`.

Determinism here rests on `srand(42)` in `buildInput`. If the second run differs,
the seed was lost in the rewrite.

- [ ] **Step 6: Verify `sample.txt` needs no edit**

```bash
cd "$SCRATCH/verify"
tail -n 132 "/home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments/WEEK 2/Q2/sample.txt" > sample_body2.txt
diff sample_body2.txt q2.out && echo "SAMPLE CLEAN — no edit needed"
```

Expected: `SAMPLE CLEAN — no edit needed`. Do not overwrite `sample.txt`.

- [ ] **Step 7: Commit**

```bash
cd /home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments
git add "WEEK 2/Q2/q2_merge_sort_variants.c"
git commit -m "refactor(week2-q2): compress to 226 lines

Fold the four result arrays into two indexed by variant, dispatch both
sorts through a function-pointer table, and collapse the six printTable
calls into one loop. Both sorts, all three input classes, all six sizes
and both tables are unchanged; output is byte-identical.

Co-Authored-By: Claude <noreply@anthropic.com>"
```

---

## Task 3: Compress Q3 to 240 lines

**Files:**
- Modify: `WEEK 2/Q3/q3_merge_k_sorted_arrays.c` (401 → 240 lines, full rewrite)
- Verify against: `<scratchpad>/baseline/q3.out`

**Design notes.** The single largest fold is `printSweepK` and `printSweepN`
becoming one `printSweep(int varyK)`. Both print the same table — axis column, two
move counts, speedup — and differ only in which axis moves and what the trailing
columns mean: two normalisations when `k` varies, the constant prediction when `n`
varies. Preserved exactly: `mergeInto`, method 1's buffer swap (which is what keeps
the count honest about merging alone rather than copying back), method 2's
odd-one-out carry, `srand(42)`, the k=4/n=3 demo, and the `isSorted` check after
every single run.

**240 is the agreed ceiling**, above the 200 target, for the same reason as Q2.

- [ ] **Step 1: Confirm the baseline reads 91 lines**

```bash
SCRATCH=/tmp/claude-1000/-home-raghvendra-singh-Design-and-Analysis-of-Algorithms-Assignments/11840e73-9cf9-0b67-8136-53d0bb954b3c/scratchpad
wc -l "$SCRATCH/baseline/q3.out"
```

Expected: `91`.

- [ ] **Step 2: Write the new source**

Write the following 240 lines to `WEEK 2/Q3/q3_merge_k_sorted_arrays.c`, replacing
its entire contents.

```c
/* q3_merge_k_sorted_arrays.c
 *
 * DAA Q3 : merging k sorted arrays of n elements, two ways.  Method 1 folds
 * them in one at a time; method 2 merges in pairs, round by round.  Both are
 * counted in element moves and swept over k and over n, and every run's
 * output is checked sorted before its counts are believed.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NKS 6
#define NNS 5
#define FIXED_N 200     /* n held constant while k varies */
#define FIXED_K 16      /* k held constant while n varies */

long long moves = 0;    /* one element written into an output array */
void resetMoves(void) { moves = 0; }

static int kValues[NKS] = {2, 4, 8, 16, 32, 64};
static int nValues[NNS] = {100, 200, 400, 800, 1600};

/* Results of the two sweeps */
static long long sweepK1[NKS], sweepK2[NKS];
static long long sweepN1[NNS], sweepN2[NNS];

/* Merge sorted x[0..xn) and sorted y[0..yn) into out[0..xn+yn) - the one
 * subroutine both methods are built out of. */
void mergeInto(int *x, int xn, int *y, int yn, int *out) {
    int i = 0, j = 0, t = 0;
    while (i < xn && j < yn) {
        if (x[i] <= y[j]) out[t++] = x[i++];
        else out[t++] = y[j++];
        moves++;
    }
    while (i < xn) { out[t++] = x[i++]; moves++; }   /* whichever run is left */
    while (j < yn) { out[t++] = y[j++]; moves++; }   /* still costs its moves */
}

/* --- Method 1: merge them in one at a time --- */
void method1(int **arrays, int k, int n, int *out) {
    int total = k * n, len = n;
    int *acc = (int *)malloc(sizeof(int) * total);
    int *tmp = (int *)malloc(sizeof(int) * total);
    /* Start from a copy of the first array.  These copies are setup, not
     * merging work, so they are not counted. */
    for (int i = 0; i < n; i++) acc[i] = arrays[0][i];
    for (int a = 1; a < k; a++) {
        int *swap;
        mergeInto(acc, len, arrays[a], n, tmp);
        len += n;
        /* The result is in tmp - swap the buffers rather than copy it back,
         * so the count stays honest about merging alone. */
        swap = acc; acc = tmp; tmp = swap;
    }
    for (int i = 0; i < total; i++) out[i] = acc[i];
    free(acc); free(tmp);
}

/* --- Method 2: merge in pairs, round by round --- */
void method2(int **arrays, int k, int n, int *out) {
    int **cur = (int **)malloc(sizeof(int *) * k);
    int *len = (int *)malloc(sizeof(int) * k);
    int count = k;
    /* Working copies, so the caller's arrays stay untouched. */
    for (int i = 0; i < k; i++) {
        cur[i] = (int *)malloc(sizeof(int) * n);
        for (int j = 0; j < n; j++) cur[i][j] = arrays[i][j];
        len[i] = n;
    }
    while (count > 1) {
        int newCount = 0;
        for (int i = 0; i < count; i += 2) {
            if (i + 1 < count) {
                int m = len[i] + len[i + 1];
                int *merged = (int *)malloc(sizeof(int) * m);
                mergeInto(cur[i], len[i], cur[i + 1], len[i + 1], merged);
                free(cur[i]); free(cur[i + 1]);
                cur[newCount] = merged;
                len[newCount] = m;
            } else {    /* odd one out - it waits for the next round */
                cur[newCount] = cur[i];
                len[newCount] = len[i];
            }
            newCount++;
        }
        count = newCount;
    }
    for (int i = 0; i < len[0]; i++) out[i] = cur[0][i];
    free(cur[0]); free(cur); free(len);
}

/* --- Helpers --- */
int compareInts(const void *p, const void *q) {
    int a = *(const int *)p, b = *(const int *)q;
    return (a > b) - (a < b); }

int **buildArrays(int k, int n) {    /* k sorted arrays of n elements each */
    int **arrays = (int **)malloc(sizeof(int *) * k);
    srand(42);                              /* fixed seed, so runs repeat */
    for (int i = 0; i < k; i++) {
        arrays[i] = (int *)malloc(sizeof(int) * n);
        for (int j = 0; j < n; j++) arrays[i][j] = rand() % 100000;
        qsort(arrays[i], n, sizeof(int), compareInts);
    }
    return arrays;
}

void freeArrays(int **arrays, int k) {
    for (int i = 0; i < k; i++) free(arrays[i]);
    free(arrays); }

int isSorted(int *a, int total) {
    for (int i = 1; i < total; i++) if (a[i - 1] > a[i]) return 0;
    return 1; }
void printLine(int w) { while (w-- > 0) printf("-"); printf("\n"); }
void printHeading(char *nl, char *title) {
    printf("%s=====================================================\n %s\n"
           "=====================================================\n", nl, title);
}
void printArray(int *a, int total, char *fmt) {
    for (int i = 0; i < total; i++) printf(fmt, a[i]);
    printf("\n"); }

/* --- Output sections --- */
void printDemo(void) {
    int a0[3] = {1, 8, 15}, a1[3] = {2, 9, 20};
    int a2[3] = {5, 6, 30}, a3[3] = {3, 11, 12};
    int *demo[4] = {a0, a1, a2, a3};
    int out1[12], out2[12];
    long long m1, m2;
    printHeading("\n", "DEMONSTRATION : k = 4 arrays of n = 3 elements");
    for (int i = 0; i < 4; i++) { printf("Array %d : ", i + 1); printArray(demo[i], 3, "%3d "); }
    resetMoves(); method1(demo, 4, 3, out1); m1 = moves;
    resetMoves(); method2(demo, 4, 3, out2); m2 = moves;
    printf("\nMethod 1 output : "); printArray(out1, 12, "%d ");
    printf("                  %lld element moves\n", m1);
    printf("Method 2 output : "); printArray(out2, 12, "%d ");
    printf("                  %lld element moves\n", m2);
    printf("\nBoth sorted : %s\n",
           (isSorted(out1, 12) && isSorted(out2, 12)) ? "YES" : "NO");
    printf("Method 1 needed %lld moves, method 2 needed %lld.\n"
           "Even at k = 4 the repeated merging is already doing more work.\n", m1, m2);
}

void printTheory(void) {
    printHeading("\n", "THE TWO RUNNING TIMES");
    printf("Method 1 - merge one array in at a time\n"
           "  Round i merges a block of size i*n with an array of size n.\n"
           "  Total = 2n + 3n + ... + kn = n(k(k+1)/2 - 1) = O(n k^2)\n"
           "  The first array's elements get copied k-1 times.\n\n"
           "Method 2 - merge in pairs, halving the count each round\n"
           "  Every round moves all kn elements exactly once.\n"
           "  The number of rounds is log2(k).\n"
           "  Total = kn * log2(k) = O(n k log k)\n\n"
           "Method 2 is better by a factor of Theta(k / log k).\n"
           "Putting the leading constants back in, method 1 costs about\n"
           "n k^2 / 2 and method 2 about n k log2(k), so the speedup to\n"
           "expect in practice is k / (2 log2 k).\n"
           "At k = 64 that predicts about %.1f times.\n",
           64.0 / (2.0 * (log(64.0) / log(2.0))));
}

/* Both sweeps print the same table - axis column, two move counts, speedup -
 * and differ only in which axis moves and what the trailing columns mean: two
 * normalisations when k varies, the constant prediction when n varies.  So one
 * routine parameterised by the axis prints both. */
void printSweep(int varyK) {
    int count = varyK ? NKS : NNS, width = varyK ? 78 : 64;
    long long *m1 = varyK ? sweepK1 : sweepN1, *m2 = varyK ? sweepK2 : sweepN2;
    printf("\n=====================================================\n");
    printf(varyK ? " VARYING k, WITH n FIXED AT %d\n" : " VARYING n, WITH k FIXED AT %d\n",
           varyK ? FIXED_N : FIXED_K);
    printf("=====================================================\n");
    printLine(width);
    if (varyK) printf("%6s %14s %14s %10s %12s %12s\n", "k", "M1 moves", "M2 moves",
                      "speedup", "M1/(n k^2)", "M2/(nk log k)");
    else printf("%8s %14s %14s %10s %12s\n", "n", "M1 moves", "M2 moves",
                "speedup", "predicted");
    printLine(width);
    for (int i = 0; i < count; i++) {
        double k = varyK ? (double)kValues[i] : (double)FIXED_K;
        double n = varyK ? (double)FIXED_N : (double)nValues[i];
        double logk = log(k) / log(2.0);
        double speedup = (double)m1[i] / (double)m2[i];
        printf(varyK ? "%6d" : "%8d", varyK ? kValues[i] : nValues[i]);
        printf(" %14lld %14lld %10.2f", m1[i], m2[i], speedup);
        if (varyK) printf(" %12.4f %12.4f\n", m1[i] / (n * k * k),
                          (logk > 0) ? m2[i] / (n * k * logk) : 0.0);
        else printf(" %12.2f\n", k / (2.0 * logk));
    }
    if (varyK) printf("\nThe M1/(n k^2) column settles near 0.5, which is the n k^2 / 2\n"
                      "predicted above.  The M2/(n k log k) column settles near 1.0.\n"
                      "Both normalisations are flat, so both formulas are right.\n");
    else printf("\nBoth methods are linear in n, so doubling n doubles both\n"
                "counts and the speedup column does not move.  The gap between\n"
                "the methods is controlled by k alone, not by n.\n");
}

/* Fresh input, both methods run on it, their counts handed back. */
void measure(int k, int n, long long *m1, long long *m2) {
    int **arrays = buildArrays(k, n);
    int *out = (int *)malloc(sizeof(int) * k * n);
    resetMoves(); method1(arrays, k, n, out); *m1 = moves;
    if (!isSorted(out, k * n)) printf("\nERROR: method 1 output not sorted at k = %d\n", k);
    resetMoves(); method2(arrays, k, n, out); *m2 = moves;
    if (!isSorted(out, k * n)) printf("\nERROR: method 2 output not sorted at k = %d\n", k);
    free(out); freeArrays(arrays, k);
    printf("."); fflush(stdout);
}

void runSweeps(void) {
    printf("\nMeasuring ");
    for (int i = 0; i < NKS; i++) measure(kValues[i], FIXED_N, &sweepK1[i], &sweepK2[i]);
    for (int i = 0; i < NNS; i++) measure(FIXED_K, nValues[i], &sweepN1[i], &sweepN2[i]);
    printf(" done\n");
}

int main(void) {
    printHeading("", "DAA Q3 : MERGING k SORTED ARRAYS OF n ELEMENTS");
    printDemo();
    printTheory();
    runSweeps();
    printSweep(1);      /* k varies, n fixed */
    printSweep(0);      /* n varies, k fixed */
    printHeading("\n", "CONCLUSION");
    printf("Method 1 : O(n k^2)     - repeated merging into a growing\n"
           "                          block, so early elements are\n"
           "                          copied over and over\n"
           "Method 2 : O(n k log k) - a balanced merge tree, so every\n"
           "                          element is copied once per round\n"
           "                          and there are only log2(k) rounds\n\n"
           "Both were checked at every size: the output is sorted and\n"
           "holds all kn elements.\n"
           "\nTime Complexity  : method 1 O(n k^2), method 2 O(n k log k)\n"
           "Space Complexity : O(n k) for both - the merged output plus\n"
           "                   one working buffer of the same size\n");
    return 0;
}
```

- [ ] **Step 3: Check the line count**

```bash
wc -l "/home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments/WEEK 2/Q3/q3_merge_k_sorted_arrays.c"
```

Expected: `240`, exactly at the ceiling.

- [ ] **Step 4: Compile with `-lm`, requiring zero warnings**

```bash
SCRATCH=/tmp/claude-1000/-home-raghvendra-singh-Design-and-Analysis-of-Algorithms-Assignments/11840e73-9cf9-0b67-8136-53d0bb954b3c/scratchpad
cd "$SCRATCH/verify"
gcc -Wall -Wextra "/home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments/WEEK 2/Q3/q3_merge_k_sorted_arrays.c" -o q3 -lm 2>&1 | tee q3.warn
wc -l < q3.warn
```

Expected: no gcc output, `q3.warn` is 0 lines.

- [ ] **Step 5: Run it, check exit status, determinism, and the baseline**

```bash
cd "$SCRATCH/verify"
./q3 > q3.out; echo "exit=$?"
./q3 > q3b.out
diff q3.out q3b.out && echo "DETERMINISTIC"
diff q3.out "$SCRATCH/baseline/q3.out" && echo "BASELINE CLEAN"
wc -l < q3.out
grep 'Both sorted' q3.out
grep -c 'ERROR' q3.out || echo "no errors"
```

Expected: `exit=0`, `DETERMINISTIC`, `BASELINE CLEAN`, `91`,
`Both sorted : YES`, and `no errors`.

The `ERROR` grep matters: `measure()` prints an error line if either method's
output fails `isSorted`. Zero matches means every one of the 11 measured runs
produced a correctly sorted result.

- [ ] **Step 6: Verify `sample.txt` needs no edit**

```bash
cd "$SCRATCH/verify"
tail -n 91 "/home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments/WEEK 2/Q3/sample.txt" > sample_body3.txt
diff sample_body3.txt q3.out && echo "SAMPLE CLEAN — no edit needed"
```

Expected: `SAMPLE CLEAN — no edit needed`. Do not overwrite `sample.txt`.

- [ ] **Step 7: Commit**

```bash
cd /home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments
git add "WEEK 2/Q3/q3_merge_k_sorted_arrays.c"
git commit -m "refactor(week2-q3): compress to 240 lines

Fold printSweepK and printSweepN into one routine parameterised by the
axis being swept. Both methods, both sweeps, all six k values, all five n
values and every normalisation column are unchanged; output is
byte-identical.

Co-Authored-By: Claude <noreply@anthropic.com>"
```

---

## Task 4: Update WEEK 2/Q1/README.md

Nine edits to `WEEK 2/Q1/README.md`, then a proofread and a commit. Every anchor
is verbatim; use the exact replace targets below. The new plots section uses the
repo's own `## Committed Artefacts` convention (see the naming decision above).

**On line numbers.** They describe the file as committed at `HEAD`. Step 2 deletes
23 lines, so every anchor after it shifts up by 23. **Match on the quoted text,
not the line number** — the text is unique in the file in every case.

- [ ] **Step 1: Header table (lines 8–9, 12)**

Replace:

```
| **Header** | [`dictionary.h`](dictionary.h) |
| **Sources** | six implementation files + [`q1_dictionary_operations.c`](q1_dictionary_operations.c) |
```

with:

```
| **Source** | [`q1_dictionary_operations.c`](q1_dictionary_operations.c) |
| **Lines** | 499 — one file for all six representations and the measurement |
```

Then replace line 12:

```
| **Build** | `gcc -Wall -Wextra *.c -o q1` |
```

with:

```
| **Build** | `gcc -Wall -Wextra q1_dictionary_operations.c -o q1` |
```

- [ ] **Step 2: `## File Layout` — delete the heading, prose and table (lines 74–96)**

**Delete lines 74–96 inclusive:** the blank at 74, the heading at 75, the prose,
the 8-row table, the closing paragraph, the blank at 95, and the trailing `---` at
96. Line 73 already carries a `---` separating this section from the claim table
above, so removing the *trailing* rule rather than the leading one leaves exactly
one separator and no double rule. The result reads:

```
  (O(n), no random access) and `Insert` (O(n), must find the sorted position).

---

## Approach
```

The content removed is lines 75–94:

```
## File Layout

The six representations are separated into one file each, all sharing a single
header. Each file is self-contained: it owns its seven operations and its own
measurement routine, so a representation can be read and understood on its own.

| File | Contents | Prefix |
|------|----------|--------|
| [`dictionary.h`](dictionary.h) | Struct definitions and prototypes for all six | — |
| [`unsorted_array.c`](unsorted_array.c) | Unsorted array | `ua` |
| [`sorted_array.c`](sorted_array.c) | Sorted array | `sa` |
| [`singly_unsorted.c`](singly_unsorted.c) | Singly linked unsorted list | `su` |
| [`singly_sorted.c`](singly_sorted.c) | Singly linked sorted list | `ss` |
| [`doubly_unsorted.c`](doubly_unsorted.c) | Doubly linked unsorted list | `du` |
| [`doubly_sorted.c`](doubly_sorted.c) | Doubly linked sorted list | `ds` |
| [`q1_dictionary_operations.c`](q1_dictionary_operations.c) | The claim table, the measurement loop and the validation | — |

Every representation exposes the same seven operations with the same meaning, so
they are directly comparable. The driver calls `xxMeasure(n, result)` on each and
never needs to know how any of them work.
```

- [ ] **Step 3: Approach — replace the step-counter paragraph**

Replace:

```
**A step counter as the cost model.** One global counter, declared in
`dictionary.h` and defined in the driver, is incremented on every key comparison
and every pointer or index move. Wall-clock time is avoided deliberately — it
depends on the compiler and the machine, and at these sizes it is mostly noise.
Step counts are exact and reproducible.
```

with:

```
**A step counter as the cost model.** One global counter, defined in the file and
reset before every measurement, is incremented on every key comparison and every
pointer or index move. Wall-clock time is avoided deliberately — it depends on the
compiler and the machine, and at these sizes it is mostly noise. Step counts are
exact and reproducible.
```

- [ ] **Step 4: Approach — add the family-merge paragraph**

Insert this paragraph immediately after the worst-case table (after the
`Predecessor` / `Successor` row and before `**Class inference from the doubling
ratio.**`):

```
**Two families generate all six rows.** The two arrays differ only in whether
order is maintained; the four lists differ only in that and in whether a back
pointer may be read. One `Array` struct with a `sorted` flag and one `List` struct
with `sorted` and `dbl` flags therefore serve all six representations — not a
trick of the compression, but the shape of the claim table itself. The singly
linked nodes still carry a `prev` field; those rows are *defined by never reading
it*, which is exactly what makes their Delete and Predecessor O(n).
```

- [ ] **Step 5: Approach — replace the "asks for a plot" paragraph (lines 135–140)**

Replace:

```
**How the order of growth is shown.** The question asks for a plot. What the
program prints instead is the step count for every operation at every size,
followed by the doubling ratio and the class it implies. The table carries the
same information a curve would, and more precisely: a reader can check that
`Search` on a sorted array reads 7, 8, 9, 10, 11, 12 and see the logarithm
directly, which is not something you can read off a drawn axis.
```

with:

```
**How the order of growth is shown.** The question asks for a plot. What the
program prints instead is the step count for every operation at every size,
followed by the doubling ratio and the class it implies. The table carries the
same information a curve would, and more precisely: a reader can check that
`Search` on a sorted array reads 7, 8, 9, 10, 11, 12 and see the logarithm
directly, which is not something you can read off a drawn axis. The plots below
were produced separately from the program's tabulated output and committed
alongside the source.
```

- [ ] **Step 6: Space Complexity (lines 154–158) — drop the header/family reference**

Replace:

```
**O(n)** — only one structure is alive at a time and it is freed before the next
is built. Arrays hold `n` keys; a singly linked list adds one pointer per node, a
doubly linked list two.
```

with:

```
**O(n)** — only one structure is alive at a time and it is freed before the next
is built. The array family holds `n` keys; the list family adds one pointer per
node for the singly linked rows and two for the doubly linked rows.
```

- [ ] **Step 7: Insert `## Committed Artefacts` before `## Build and Run`**

Insert, between the end of `## Sample Explanation` (the paragraph ending
"which is the next step beyond this exercise." at line 253) and `## Build and Run`
(line 257):

```
## Committed Artefacts

The program itself writes nothing to disk. The plots below were produced
separately from its tabulated output and committed alongside the source.

| File | Description |
|------|-------------|
| [`plots/1_all_operations.png`](plots/1_all_operations.png) | All 42 measured step counts across the six structures |
| [`plots/2_search.png`](plots/2_search.png) | The seven `Search` rows — sorted array logarithmic, everything else linear |
| [`plots/3_per_structure.png`](plots/3_per_structure.png) | Each structure's seven operations, per structure |
| [`plots/4_complexity_table.png`](plots/4_complexity_table.png) | The claim table rendered as heatmap |
| [`plots/5_cost_at_max_n.png`](plots/5_cost_at_max_n.png) | All 42 cells at the largest size, `n = 3200` |
| [`plots/6_doubling_ratio.png`](plots/6_doubling_ratio.png) | The doubling ratio behind the class inference |
```

- [ ] **Step 8: Build and Run (lines 257–266) — single-file build**

Replace (outer fences are four backticks here because the content itself is a
fenced block):

````
## Build and Run

```bash
gcc -Wall -Wextra *.c -o q1
./q1
```

All seven `.c` files must be compiled together — the six implementations and the
driver. The driver defines the `steps` counter that the other six declare as
`extern` through the header.
````

with:

````
## Build and Run

```bash
gcc -Wall -Wextra q1_dictionary_operations.c -o q1
./q1
```

The whole question lives in one file — the six representations, the claim table,
the measurement loop and the validation.
````

- [ ] **Step 9: Files table (lines 268–280) — nine rows down to two**

Replace:

```
| File | Description |
|------|-------------|
| [`dictionary.h`](dictionary.h) | Shared interface — structs and prototypes for all six representations |
| [`unsorted_array.c`](unsorted_array.c) | Unsorted array, seven operations plus its measurement routine |
| [`sorted_array.c`](sorted_array.c) | Sorted array, with binary search |
| [`singly_unsorted.c`](singly_unsorted.c) | Singly linked unsorted list |
| [`singly_sorted.c`](singly_sorted.c) | Singly linked sorted list |
| [`doubly_unsorted.c`](doubly_unsorted.c) | Doubly linked unsorted list |
| [`doubly_sorted.c`](doubly_sorted.c) | Doubly linked sorted list |
| [`q1_dictionary_operations.c`](q1_dictionary_operations.c) | Driver — claim table, measurement loop, validation |
| [`sample.txt`](sample.txt) | Sample input and output |
```

with:

```
| File | Description |
|------|-------------|
| [`q1_dictionary_operations.c`](q1_dictionary_operations.c) | Everything — all six representations, the claim table, the measurement and the validation |
| [`sample.txt`](sample.txt) | Sample output |
```

- [ ] **Step 10: Proofread the result**

```bash
grep -n 'dictionary.h\|unsorted_array\|sorted_array\|singly_\|doubly_' \
  "/home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments/WEEK 2/Q1/README.md"
grep -n 'gcc -Wall' "/home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments/WEEK 2/Q1/README.md"
grep -n '^## ' "/home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments/WEEK 2/Q1/README.md"
```

Expected: the first grep finds nothing but the `[`q1_dictionary_operations.c`]`
rows in the header table, Files table and the new build block — in particular no
link to `dictionary.h` and no `*.c` anywhere. Second grep shows exactly one
`gcc -Wall -Wextra q1_dictionary_operations.c -o q1`. Third grep shows the heading
sequence with `## Committed Artefacts` sitting between `## Sample Explanation` and
`## Build and Run`.

- [ ] **Step 11: Commit**

```bash
cd /home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments
git add "WEEK 2/Q1/README.md"
git commit -m "docs(week2-q1): describe the single-file layout

Header table, file layout, approach, space complexity, build instructions
and files table now describe one source file; add a Committed Artefacts
section for the six committed plots.

Co-Authored-By: Claude <noreply@anthropic.com>"
```

---

## Task 5: Delete the three unfaithful plots

**Files:**
- Delete: `WEEK 2/Q2/plots/4_input_classes.png`
- Delete: `WEEK 2/Q2/plots/6_k_way_constant.png`
- Delete: `WEEK 2/Q3/plots/4_input_classes.png`

**Why these three.** 15 of the 18 committed PNGs are faithful — most run a wider
range of `n` or `k` than the C does, but compute the same quantities, and the
README entries added in Tasks 4/6/7 say so where it applies. These three measure
things the committed code cannot produce, so no honest caption could be written
for them. They are deleted, not redrawn: the script that produced them is not in
the repository and reconstructing it is out of scope.

| File | Why it goes |
|---|---|
| `WEEK 2/Q2/plots/4_input_classes.png` | Plots 4 input classes including a "Worst" class; `buildInput` defines 3 (random, sorted, reverse) |
| `WEEK 2/Q2/plots/6_k_way_constant.png` | Plots a tournament merge; Q2 implements 2-way and 3-way only |
| `WEEK 2/Q3/plots/4_input_classes.png` | Plots *comparisons* over Interleaved/Random/Blocked input; Q3 counts only `moves` and builds random arrays exclusively |

- [ ] **Step 1: Confirm what is there before deleting**

```bash
cd "/home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments/WEEK 2"
ls Q1/plots Q2/plots Q3/plots
```

Expected: Q1 has 6 files, Q2 has 6, Q3 has 6 — 18 total.

- [ ] **Step 2: Delete the three**

```bash
cd "/home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments/WEEK 2"
git rm Q2/plots/4_input_classes.png Q2/plots/6_k_way_constant.png Q3/plots/4_input_classes.png
```

- [ ] **Step 3: Confirm the surviving 15**

```bash
cd "/home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments/WEEK 2"
ls Q1/plots Q2/plots Q3/plots
ls Q1/plots Q2/plots Q3/plots | grep -c '\.png$'
```

Expected: 15 PNGs — Q1 keeps all 6; Q2 keeps `1_growth_comparisons`,
`2_normalised_constants`, `3_base_of_logarithm`, `5_tradeoff`; Q3 keeps
`1_growth_moves`, `2_normalised_constants`, `3_speedup`, `5_sweep_n`,
`6_merge_tree_shape`.

- [ ] **Step 4: Commit**

```bash
cd /home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments
git commit -m "chore(week2): remove three plots the code cannot produce

Q2's 4_input_classes plots a fourth 'Worst' class and 6_k_way_constant a
tournament merge; the program defines three input classes and implements
two variants. Q3's 4_input_classes plots comparisons over three input
shapes; Q3 counts moves and builds random arrays only.

Co-Authored-By: Claude <noreply@anthropic.com>"
```

---

## Task 6: Update WEEK 2/Q2/README.md

Two edits. Filenames and the build command do not change, so the header table and
build block stay as they are.

- [ ] **Step 1: Verify the line count claim at line 128**

```bash
grep -n 'the full run prints' "/home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments/WEEK 2/Q2/README.md"
```

Expected: line 128, `Abridged — the full run prints 132 lines. See [`sample.txt`](sample.txt).`

That number must still be 132 — Task 2's baseline diff already proved it is. **If
it reads anything else, the compression failed; fix the code, not the number.**
No edit to this line.

- [ ] **Step 2: Extend the "asks for a plot" paragraph (lines 96–101)**

Replace:

```
**How the order of growth is shown.** The question asks for a plot. The program
prints the raw counts across six doubling sizes and then divides each one by
`n log₂ n`. That second table is the stronger evidence: a curve drawn on a linear
axis makes `n log n` and `n²` look much alike, whereas a normalised column that
stays at 1.001 across a 32× range of `n` can only mean `n log n`, and one that
climbed would falsify the claim outright.
```

with:

```
**How the order of growth is shown.** The question asks for a plot. The program
prints the raw counts across six doubling sizes and then divides each one by
`n log₂ n`. That second table is the stronger evidence: a curve drawn on a linear
axis makes `n log n` and `n²` look much alike, whereas a normalised column that
stays at 1.001 across a 32× range of `n` can only mean `n log n`, and one that
climbed would falsify the claim outright. Plots of the same measurements are
committed under [`plots/`](plots) and listed below.
```

- [ ] **Step 3: Insert `## Committed Artefacts` before `## Build and Run`**

Insert between the end of `## Sample Explanation` (the paragraph ending "Only the
comparison counts shift, and only by a constant." at line 222) and
`## Build and Run` (line 224):

```
## Committed Artefacts

The program itself writes nothing to disk. The plots below were produced
separately from its tabulated output and committed alongside the source. Their
`n` range runs wider than the program's own sweep of 1000 … 32000; the quantities
plotted are the ones the program counts.

| File | Description |
|------|-------------|
| [`plots/1_growth_comparisons.png`](plots/1_growth_comparisons.png) | Raw comparison counts for both variants as `n` grows |
| [`plots/2_normalised_constants.png`](plots/2_normalised_constants.png) | Each count divided by `n log₂ n` — the flat columns that carry the claim |
| [`plots/3_base_of_logarithm.png`](plots/3_base_of_logarithm.png) | The measured three-way move ratio against `1/log₂3 = 0.6309` |
| [`plots/5_tradeoff.png`](plots/5_tradeoff.png) | Fewer moves against more comparisons — the two effects that nearly cancel |
```

- [ ] **Step 4: Proofread**

```bash
grep -n '^## ' "/home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments/WEEK 2/Q2/README.md"
grep -n '4_input_classes\|6_k_way_constant' "/home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments/WEEK 2/Q2/README.md"
```

Expected: `## Committed Artefacts` sits between `## Sample Explanation` and
`## Build and Run`. The second grep finds **nothing** — the two deleted plots must
not be referenced.

- [ ] **Step 5: Commit**

```bash
cd /home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments
git add "WEEK 2/Q2/README.md"
git commit -m "docs(week2-q2): document the committed plots

Add a Committed Artefacts section for the four surviving plots and note
that their n range runs wider than the program's own sweep.

Co-Authored-By: Claude <noreply@anthropic.com>"
```

---

## Task 7: Update WEEK 2/Q3/README.md

Two edits. Filenames and the build command do not change. Note Q3 has **no
`## Space Complexity` heading** — it folds space into a table under
`## Time Complexity`. Do not add one.

- [ ] **Step 1: Verify the line count claim at line 142**

```bash
grep -n 'the full run prints' "/home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments/WEEK 2/Q3/README.md"
```

Expected: line 142, `Abridged — the full run prints 91 lines. See [`sample.txt`](sample.txt).`

Must still read 91 — Task 3's baseline diff proved it. No edit.

- [ ] **Step 2: Extend the growth paragraph (lines 113–118)**

Replace:

```
**How the order of growth is shown.** Each sweep prints its move counts beside a
normalised column — `M1/(n k²)` and `M2/(nk log k)`. Those two columns are the
evidence: one settles at 0.5 and the other sits at exactly 1.0000, which pins each
method to its claimed formula far more tightly than two rising curves on a shared
axis ever could.
```

with:

```
**How the order of growth is shown.** Each sweep prints its move counts beside a
normalised column — `M1/(n k²)` and `M2/(nk log k)`. Those two columns are the
evidence: one settles at 0.5 and the other sits at exactly 1.0000, which pins each
method to its claimed formula far more tightly than two rising curves on a shared
axis ever could. Plots of the same measurements are committed under
[`plots/`](plots) and listed below.
```

- [ ] **Step 3: Insert `## Committed Artefacts` before `## Build and Run`**

Insert between the end of `## Sample Explanation` (the paragraph ending "That is
the entire lesson of the question." at line 230) and `## Build and Run`
(line 232):

```
## Committed Artefacts

The program itself writes nothing to disk. The plots below were produced
separately from its tabulated output and committed alongside the source. Their
`k` and `n` ranges run wider than the program's own sweeps (`k` = 2 … 64 at
`n` = 200, and `n` = 100 … 1600 at `k` = 16); the quantities plotted are the ones
the program counts.

| File | Description |
|------|-------------|
| [`plots/1_growth_moves.png`](plots/1_growth_moves.png) | Element moves for both methods as `k` grows — `nk²` against `nk log k` |
| [`plots/2_normalised_constants.png`](plots/2_normalised_constants.png) | `M1/(n k²)` settling at 0.5 and `M2/(nk log k)` at 1.0 |
| [`plots/3_speedup.png`](plots/3_speedup.png) | Measured speedup against the predicted `k / (2 log₂ k)` |
| [`plots/5_sweep_n.png`](plots/5_sweep_n.png) | Both methods linear in `n`, so the speedup does not move |
| [`plots/6_merge_tree_shape.png`](plots/6_merge_tree_shape.png) | Why the schedule is the whole difference — a skewed chain against a balanced tree |
```

- [ ] **Step 4: Proofread**

```bash
grep -n '^## ' "/home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments/WEEK 2/Q3/README.md"
grep -n '4_input_classes' "/home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments/WEEK 2/Q3/README.md"
```

Expected: `## Committed Artefacts` between `## Sample Explanation` and
`## Build and Run`, and **no** reference to the deleted `4_input_classes.png`.

- [ ] **Step 5: Commit**

```bash
cd /home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments
git add "WEEK 2/Q3/README.md"
git commit -m "docs(week2-q3): document the committed plots

Add a Committed Artefacts section for the five surviving plots and note
that their k and n ranges run wider than the program's own sweeps.

Co-Authored-By: Claude <noreply@anthropic.com>"
```

---

## Task 8: Update the root README.md

Nine edits. Three come from the compression itself; six correct statements that
become false — or were already false — once Week 2's plots are documented. The
spec listed only the Requirements paragraph; the other five were found while
gathering anchors and are included here because leaving them would contradict the
new Committed Artefacts sections.

| # | Lines | Kind |
|---|---|---|
| 1 | 105–125 | Compression — Week 2 tree |
| 2 | 158 | Compression — question table Q1 links |
| 3 | 263–270 | Compression — Q1 build note and block |
| 4 | 28–30 | Pre-existing — "Week 1 … commits a plot beside the source; Week 2 prints tables" |
| 5 | 168–172 | Pre-existing — same claim in Features |
| 6 | 194–196 | Pre-existing — same claim in Requirements (the one the spec lists) |
| 7 | 260–261 | Pre-existing — "neither does any Week 2 question" |
| 8 | 343–346 | Pre-existing — "leaving nothing behind on disk" |
| 9 | 11 | Pre-existing — the Charts badge says "SVG + measured tables" |

Edits 4–8 all restate one wrong idea: that only Week 1 ships images. Week 2 has
committed 18 PNGs all along. Each correction keeps the true half of the sentence —
Week 2 *does* print normalised tables, and its programs *do* write nothing to disk
— and drops only the false implication that it ships no images.

- [ ] **Step 1: Week 2 directory tree (lines 105–125)**

Replace:

```
    ├── Q1/                                     # Dictionary Operations
    │   ├── README.md
    │   ├── dictionary.h                        # shared interface — all six structures
    │   ├── unsorted_array.c                    # one file per representation
    │   ├── sorted_array.c
    │   ├── singly_unsorted.c
    │   ├── singly_sorted.c
    │   ├── doubly_unsorted.c
    │   ├── doubly_sorted.c
    │   ├── q1_dictionary_operations.c          # driver — claim, measurement, validation
    │   └── sample.txt
    │
    ├── Q2/                                     # Merge Sort vs Three-Way Merge Sort
    │   ├── README.md
    │   ├── q2_merge_sort_variants.c
    │   └── sample.txt
    │
    └── Q3/                                     # Merging k Sorted Arrays
        ├── README.md
        ├── q3_merge_k_sorted_arrays.c
        └── sample.txt
```

with:

```
    ├── Q1/                                     # Dictionary Operations
    │   ├── README.md
    │   ├── q1_dictionary_operations.c          # all six structures, measurement, validation
    │   ├── sample.txt
    │   └── plots/                              # 6 plots of the 42 measured cells
    │       ├── 1_all_operations.png
    │       ├── 2_search.png
    │       ├── 3_per_structure.png
    │       ├── 4_complexity_table.png
    │       ├── 5_cost_at_max_n.png
    │       └── 6_doubling_ratio.png
    │
    ├── Q2/                                     # Merge Sort vs Three-Way Merge Sort
    │   ├── README.md
    │   ├── q2_merge_sort_variants.c
    │   ├── sample.txt
    │   └── plots/                              # 4 plots of the two variants
    │       ├── 1_growth_comparisons.png
    │       ├── 2_normalised_constants.png
    │       ├── 3_base_of_logarithm.png
    │       └── 5_tradeoff.png
    │
    └── Q3/                                     # Merging k Sorted Arrays
        ├── README.md
        ├── q3_merge_k_sorted_arrays.c
        ├── sample.txt
        └── plots/                              # 5 plots of the two methods
            ├── 1_growth_moves.png
            ├── 2_normalised_constants.png
            ├── 3_speedup.png
            ├── 5_sweep_n.png
            └── 6_merge_tree_shape.png
```

Note the plot filenames are not renumbered after the three deletions — Q2 keeps
`5_tradeoff` and Q3 keeps `5_sweep_n` and `6_merge_tree_shape`. Renaming the
committed files is out of scope, so the gaps in the numbering are expected.

- [ ] **Step 2: Question table, Q1 row (line 158)**

Replace the trailing links cell:

```
| [README](WEEK%202/Q1/README.md) · [header](WEEK%202/Q1/dictionary.h) · [driver](WEEK%202/Q1/q1_dictionary_operations.c) |
```

with:

```
| [README](WEEK%202/Q1/README.md) · [code](WEEK%202/Q1/q1_dictionary_operations.c) |
```

This makes the Q1 row match Q2's and Q3's `[README] · [code]` shape. Keep the
` · ` separator and the `%20` path encoding exactly.

- [ ] **Step 3: Q1 build note and block (lines 263–270)**

Replace:

````
**Week 2 Q1 is split across several files** and must be compiled together: the six
representations plus the driver, which is where the shared step counter lives.

```bash
cd "WEEK 2/Q1"
gcc -Wall -Wextra *.c -o q1
./q1
```
````

with:

````
**Week 2 Q1 is one file** — all six representations, the claim table, the
measurement loop and the validation.

```bash
cd "WEEK 2/Q1"
gcc -Wall -Wextra q1_dictionary_operations.c -o q1
./q1
```
````

- [ ] **Step 4: Overview paragraph (lines 28–30)**

Replace:

```
rather than asserted. Week 1 exports its measurements to CSV and commits a plot
beside the source; Week 2 prints its measurements as tables, each one normalised
by the growth function it is supposed to follow, so a flat column is the proof.
```

with:

```
rather than asserted. Week 1 exports its measurements to CSV and commits a plot
beside the source; Week 2 commits its plots too, and additionally prints its
measurements as tables, each one normalised by the growth function it is supposed
to follow, so a flat column is the proof.
```

- [ ] **Step 5: Features bullet (lines 168–172)**

Replace:

```
- **Empirical alongside asymptotic.** Programs that study growth measure it
  rather than assume it. Week 1 writes its measurements to CSV and commits the
  plot as SVG beside the source; Week 2 prints the counts as a table and divides
  each one by the growth function it should follow, so the claim stands or falls
  on whether that column stays flat.
```

with:

```
- **Empirical alongside asymptotic.** Programs that study growth measure it
  rather than assume it. Week 1 writes its measurements to CSV and commits the
  plot as SVG beside the source; Week 2 commits its plots as PNG and also prints
  the counts as a table, dividing each one by the growth function it should
  follow, so the claim stands or falls on whether that column stays flat.
```

- [ ] **Step 6: Requirements paragraph (lines 194–196)**

Replace:

```
That is the whole list. The programs have no external dependencies — Week 1's plots
are committed as images and Week 2 prints its measurements as tables, so reading the
repository requires nothing but a browser and running it requires nothing but GCC.
```

with:

```
That is the whole list. The programs have no external dependencies — both weeks
commit their plots as images, and Week 2 additionally prints its measurements as
tables, so reading the repository requires nothing but a browser and running it
requires nothing but GCC.
```

- [ ] **Step 7: The scratch-directory blockquote (lines 260–261)**

Replace:

```
> Week 1 Q1, Q5 and Q6 write nothing to disk, and neither does any Week 2
> question — all three print their tables straight to the terminal.
```

with:

```
> Week 1 Q1, Q5 and Q6 write nothing to disk, and neither does any Week 2
> question — all three print their tables straight to the terminal. The Week 2
> plots were produced separately and committed; no program regenerates them.
```

This sentence was accurate about disk writes and stays accurate. The addition
stops a reader inferring that Week 2 ships no images.

- [ ] **Step 8: Repository Conventions (lines 343–346)**

Replace:

```
- Growth data is shown, never asserted. Week 1 writes a CSV and commits a
  hand-written `.svg` of the same name; Week 2 prints the measured counts as a
  table alongside a column normalised by the growth function under test, leaving
  nothing behind on disk.
```

with:

```
- Growth data is shown, never asserted. Week 1 writes a CSV and commits a
  hand-written `.svg` of the same name; Week 2 commits its plots under `plots/`
  and prints the measured counts as a table alongside a column normalised by the
  growth function under test, leaving nothing behind on disk when it runs.
```

- [ ] **Step 9: Charts badge (line 11)**

Replace:

```
  <img src="https://img.shields.io/badge/Charts-SVG%20%2B%20measured%20tables-1D9E5E?style=flat-square" alt="Charts" />
```

with:

```
  <img src="https://img.shields.io/badge/Charts-SVG%20%2B%20PNG%20%2B%20measured%20tables-1D9E5E?style=flat-square" alt="Charts" />
```

The repository commits 4 SVGs (Week 1) and 15 PNGs (Week 2); the badge named only
the first. Keep the `%20` and `%2B` encoding — shields.io needs it.

- [ ] **Step 10: Proofread the whole file**

```bash
cd /home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments
grep -n 'dictionary.h\|unsorted_array\|sorted_array\|singly_\|doubly_\|\*\.c' README.md
grep -n 'split across several files' README.md
grep -n '4_input_classes\|6_k_way_constant' README.md
grep -n 'Week 2 prints its measurements as tables' README.md
```

Expected: **all four greps find nothing.** The first confirms no stale Q1 filename
or wildcard build survives; the second that the split-files note is gone; the third
that no deleted plot is referenced; the fourth that the "only Week 1 has images"
phrasing is gone.

- [ ] **Step 11: Check every link the edits touched actually resolves**

```bash
cd /home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments
test -f "WEEK 2/Q1/q1_dictionary_operations.c" && echo "Q1 source OK"
for f in $(grep -o 'WEEK 2/Q[123]/plots/[0-9a-z_]*\.png' "WEEK 2/Q1/README.md" "WEEK 2/Q2/README.md" "WEEK 2/Q3/README.md" 2>/dev/null); do :; done
for q in 1 2 3; do
  while read -r p; do
    [ -f "WEEK 2/Q$q/$p" ] || echo "BROKEN: WEEK 2/Q$q/$p"
  done < <(grep -o '(plots/[0-9a-z_]*\.png)' "WEEK 2/Q$q/README.md" | tr -d '()')
done
echo "link check done"
```

Expected: `Q1 source OK`, no `BROKEN:` lines, then `link check done`.

- [ ] **Step 12: Commit**

```bash
cd /home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments
git add README.md
git commit -m "docs: update root README for the Week 2 single-file layout

Week 2 tree now shows one source per question plus the committed plots,
and Q1's build command drops the wildcard. Also corrects five statements
that implied only Week 1 commits images — Week 2 has shipped plots all
along.

Co-Authored-By: Claude <noreply@anthropic.com>"
```

---

## Task 9: Final verification sweep

Nothing new is written here. This task re-proves the whole change end to end, from
a clean checkout state, and produces the summary the spec's Definition of Done
asks for.

- [ ] **Step 1: Confirm Week 1 was never touched**

```bash
cd /home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments
git diff --stat 1c8ef25..HEAD -- "WEEK 1"
```

Expected: **empty output.** `1c8ef25` is the last commit before this work
("WEEK 2"). Any file listed here is a scope violation and must be reverted.

- [ ] **Step 2: Confirm the file inventory**

```bash
cd "/home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments/WEEK 2"
find . -name '*.c' -o -name '*.h' | sort
find . -name '*.png' | wc -l
```

Expected: exactly three sources — `./Q1/q1_dictionary_operations.c`,
`./Q2/q2_merge_sort_variants.c`, `./Q3/q3_merge_k_sorted_arrays.c` — no `.h` file
at all, and 15 PNGs.

- [ ] **Step 3: Rebuild all three from the repo copy and re-diff**

```bash
SCRATCH=/tmp/claude-1000/-home-raghvendra-singh-Design-and-Analysis-of-Algorithms-Assignments/11840e73-9cf9-0b67-8136-53d0bb954b3c/scratchpad
REPO="/home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments/WEEK 2"
mkdir -p "$SCRATCH/final" && cd "$SCRATCH/final"

gcc -Wall -Wextra "$REPO/Q1/q1_dictionary_operations.c" -o q1     2>&1 | tee w1
gcc -Wall -Wextra "$REPO/Q2/q2_merge_sort_variants.c"   -o q2 -lm 2>&1 | tee w2
gcc -Wall -Wextra "$REPO/Q3/q3_merge_k_sorted_arrays.c" -o q3 -lm 2>&1 | tee w3
echo "warning lines: $(cat w1 w2 w3 | wc -l)"

for n in 1 2 3; do
  ./q$n > f$n.out; echo "q$n exit=$?"
  diff f$n.out "$SCRATCH/baseline/q$n.out" > /dev/null && echo "q$n BASELINE CLEAN" || echo "q$n BASELINE DIRTY"
done
wc -l f1.out f2.out f3.out
```

Expected: `warning lines: 0`; `q1 exit=0`, `q2 exit=0`, `q3 exit=0`; three
`BASELINE CLEAN` lines; and counts of 143, 132, 91.

- [ ] **Step 4: Confirm all three `sample.txt` files are untouched**

```bash
cd /home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments
git diff 1c8ef25..HEAD --stat -- "WEEK 2/Q1/sample.txt" "WEEK 2/Q2/sample.txt" "WEEK 2/Q3/sample.txt"
```

Expected: **empty output.** The samples were verified, never regenerated. If this
shows a change, a task overwrote one — revert it and investigate why the output
moved.

- [ ] **Step 5: Record the landed line counts**

```bash
cd "/home/raghvendra-singh/Design-and-Analysis-of-Algorithms-Assignments/WEEK 2"
wc -l Q1/q1_dictionary_operations.c Q2/q2_merge_sort_variants.c Q3/q3_merge_k_sorted_arrays.c
```

Expected: 499, 226, 240.

- [ ] **Step 6: Report the summary**

Fill this table from the outputs above and report it:

| Question | Landed | Budget | Warnings | Exit | Deterministic | Baseline | Output lines |
|---|---|---|---|---|---|---|---|
| Q1 | 499 | 500 | 0 | 0 | yes | clean | 143 |
| Q2 | 226 | 226 | 0 | 0 | yes | clean | 132 |
| Q3 | 240 | 240 | 0 | 0 | yes | clean | 91 |

Also state: Q1 went from 8 files (1476 lines) to 1 file (499); 3 plots deleted,
15 kept; 4 READMEs updated; 3 `sample.txt` files verified unchanged.

- [ ] **Step 7: Confirm the Definition of Done**

Check each item from the spec:

1. Three single-file sources, each within budget, each warning-free — Step 3, 5
2. Three baseline diffs clean — Step 3
3. All 42 Q1 cells measured; Q2/Q3 coverage untouched — Task 1 Step 6 (`42`),
   baseline diffs
4. Seven files deleted from Q1, three PNGs deleted — Step 2
5. Every document describing the old layout updated, root README inaccuracy
   corrected — Tasks 4, 6, 7, 8
6. A Committed Artefacts section in each of the three question READMEs — Tasks 4,
   6, 7
7. Landed line counts reported per question — Step 6

---

## Notes for the executor

**On the embedded sources.** The three C files in Tasks 1–3 are not drafts. They
were written and verified before this plan: each compiles warning-free, runs to
exit 0, is deterministic across two runs, and reproduces its baseline byte for
byte. Write them exactly as given. If a line count comes out wrong, the file was
transcribed incorrectly — do not "fix" it by editing the code.

**On failure.** Every verification step in this plan is a gate, not a formality.
A dirty baseline diff, a compiler warning, or a changed `sample.txt` means
something broke. Stop, report what differs, and investigate. The one thing never
to do is adjust a baseline, a line-count claim, or a `sample.txt` to match a
broken build — the whole point of the exercise is that the numbers are evidence.

**On task order.** Tasks 1–3 are independent of each other and of 4–8. Task 5
(plot deletion) should run before Tasks 4, 6 and 7, because those add README
sections that reference the surviving plots. Task 9 runs last.
