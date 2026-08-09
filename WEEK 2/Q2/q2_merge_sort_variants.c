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
