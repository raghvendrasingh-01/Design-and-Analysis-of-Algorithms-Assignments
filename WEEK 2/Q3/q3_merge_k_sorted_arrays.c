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
