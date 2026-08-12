/* q6_selection_sort_invariant.c
 *
 * DAA Q6 : selection sort - repeatedly find the smallest of the unsorted
 * suffix and swap it into place.  The loop invariant is asserted at the top
 * of every iteration, so initialisation, maintenance and termination are
 * checked at run time rather than only argued on paper.  Comparisons and
 * swaps are counted over sorted, reverse and random inputs to show the
 * comparison count is Theta(n^2) regardless.
 */

#include <stdio.h>
#include <stdlib.h>

#define NNS 6

long long cmps = 0, swaps = 0;

static int nValues[NNS] = {8, 32, 128, 512, 1024, 2048};
static long long cSorted[NNS], cRev[NNS], cRand[NNS];
static long long sSorted[NNS], sRev[NNS], sRand[NNS];

/* The invariant, checked rather than assumed:  at the start of the pass
 * with index i, the subarray A[0..i) holds the i smallest elements of the
 * original array, in sorted order.  Two things are verified - that the
 * prefix is sorted, and that nothing in the suffix is smaller than the
 * last element placed. */
void assertInvariant(int *a, int n, int i, const char *when) {
    for (int k = 1; k < i; k++)
        if (a[k-1] > a[k]) {
            printf("INVARIANT BROKEN (%s, prefix unsorted) n=%d i=%d\n",
                   when, n, i);
            exit(1);
        }
    if (i > 0)
        for (int k = i; k < n; k++)
            if (a[k] < a[i-1]) {
                printf("INVARIANT BROKEN (%s, suffix has smaller) n=%d i=%d\n",
                       when, n, i);
                exit(1);
            }
}

/* SELECTION-SORT(A, n)
 *   for i = 0 to n-2
 *       min = i
 *       for j = i+1 to n-1
 *           if A[j] < A[min] then min = j
 *       exchange A[i] with A[min]
 *
 * The outer loop stops at n-2, not n-1: once the first n-1 elements are the
 * n-1 smallest in order, the single element left in A[n-1] has nothing it
 * could be exchanged with and nothing smaller can remain, so it is already
 * in place.  A final pass would compare nothing and swap A[n-1] with itself. */
void selectionSort(int *a, int n) {
    for (int i = 0; i < n - 1; i++) {
        int min = i, t;
        assertInvariant(a, n, i, "maintenance");    /* holds each pass */
        for (int j = i + 1; j < n; j++) {
            cmps++;
            if (a[j] < a[min]) min = j;
        }
        t = a[i]; a[i] = a[min]; a[min] = t;
        swaps++;
    }
    assertInvariant(a, n, n, "termination");        /* i = n: whole array */
}

/* kind: 0 already sorted, 1 reverse sorted, 2 random */
void fill(int *a, int n, int kind) {
    for (int i = 0; i < n; i++)
        a[i] = (kind == 0) ? i : (kind == 1) ? n - i : rand() % 10000;
}

void runOne(int n, int kind, long long *c, long long *s) {
    int *a = (int *)malloc(sizeof(int) * n);
    long long bc = cmps, bs = swaps;
    fill(a, n, kind);
    assertInvariant(a, n, 0, "initialisation");     /* empty prefix, trivial */
    selectionSort(a, n);
    for (int i = 1; i < n; i++)                     /* the output is sorted */
        if (a[i-1] > a[i]) { printf("NOT SORTED n=%d\n", n); exit(1); }
    *c = cmps - bc; *s = swaps - bs;
    free(a);
}

int main(void) {
    int i;
    for (i = 0; i < NNS; i++) {
        runOne(nValues[i], 0, &cSorted[i], &sSorted[i]);
        runOne(nValues[i], 1, &cRev[i],    &sRev[i]);
        runOne(nValues[i], 2, &cRand[i],   &sRand[i]);
    }

    printf("\n=====================================================\n");
    printf(" SELECTION SORT: COMPARISONS DO NOT DEPEND ON THE INPUT\n");
    printf("=====================================================\n");
    printf("-----------------------------------------------------------------"
           "---------\n");
    printf("%6s %11s %11s %11s %12s %8s %8s\n", "n", "sorted", "reverse",
           "random", "n(n-1)/2", "swaps", "C/n^2");
    printf("-----------------------------------------------------------------"
           "---------\n");
    for (i = 0; i < NNS; i++) {
        int n = nValues[i];
        long long exact = (long long)n * (n - 1) / 2;
        printf("%6d %11lld %11lld %11lld %12lld %8lld %8.3f\n",
               n, cSorted[i], cRev[i], cRand[i], exact, sRand[i],
               (double)cRand[i] / ((double)n * n));
    }

    printf("\nInvariant : A[0..i) holds the i smallest elements, sorted.\n");
    printf("  init    : i = 0, the prefix is empty - vacuously true.\n");
    printf("  maint   : pass i puts the smallest of A[i..n) at A[i], so the\n");
    printf("            prefix grows by one and stays correct.\n");
    printf("  term    : i = n-1, so A[0..n-1) is the n-1 smallest in order\n");
    printf("            and A[n-1] must be the largest - the array is sorted.\n");
    printf("\nWorst case Theta(n^2), best case Theta(n^2) - the same, because the\n");
    printf("inner loop always scans the whole suffix whatever it finds.  Only\n");
    printf("the swap count is fixed at n-1; no early exit is possible.\n\n");
    return 0;
}
