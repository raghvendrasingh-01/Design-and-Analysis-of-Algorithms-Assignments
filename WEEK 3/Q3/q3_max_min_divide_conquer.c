/* q3_max_min_divide_conquer.c
 *
 * DAA Q3 : maximum and minimum of an array in at most 3n/2 - 2 comparisons.
 * Divide the array in half, solve each half, and combine with exactly two
 * comparisons.  Counted against the naive 2n - 2 scan and against the
 * pairwise method, with both answers checked before the counts are believed.
 */

#include <stdio.h>
#include <stdlib.h>

#define NNS 7

typedef struct { int mn, mx; } Pair;

long long cmps = 0;     /* one comparison between two array elements */

static int nValues[NNS] = {2, 4, 16, 64, 256, 1024, 4096};
static long long dcC[NNS], naiveC[NNS], pairC[NNS];

/* Divide and conquer.  T(n) = 2T(n/2) + 2, T(2) = 1, T(1) = 0,
 * which solves to 3n/2 - 2 for n a power of two. */
Pair maxMin(int *a, int lo, int hi) {
    Pair p, l, r;
    if (lo == hi) { p.mn = p.mx = a[lo]; return p; }        /* 0 comparisons */
    if (hi - lo == 1) {                                     /* 1 comparison  */
        cmps++;
        if (a[lo] < a[hi]) { p.mn = a[lo]; p.mx = a[hi]; }
        else               { p.mn = a[hi]; p.mx = a[lo]; }
        return p;
    }
    int mid = lo + (hi - lo) / 2;
    l = maxMin(a, lo, mid);
    r = maxMin(a, mid + 1, hi);
    cmps++; p.mn = (l.mn < r.mn) ? l.mn : r.mn;             /* combine: 2 */
    cmps++; p.mx = (l.mx > r.mx) ? l.mx : r.mx;
    return p;
}

/* The obvious scan: every element is compared against both running
 * extremes, so 2n - 2 comparisons. */
Pair naive(int *a, int n) {
    Pair p; p.mn = p.mx = a[0];
    for (int i = 1; i < n; i++) {
        cmps++; if (a[i] < p.mn) p.mn = a[i];
        cmps++; if (a[i] > p.mx) p.mx = a[i];
    }
    return p;
}

/* Iterative twin of the recursion: pair the elements up, and only the
 * smaller of each pair can be the minimum.  Also 3n/2 - 2, no stack. */
Pair pairwise(int *a, int n) {
    Pair p;
    int i = 0;
    if (n % 2) { p.mn = p.mx = a[0]; i = 1; }
    else {
        cmps++;
        if (a[0] < a[1]) { p.mn = a[0]; p.mx = a[1]; }
        else             { p.mn = a[1]; p.mx = a[0]; }
        i = 2;
    }
    for (; i + 1 < n; i += 2) {
        int lo, hi;
        cmps++;                                     /* 3 comparisons per 2 */
        if (a[i] < a[i + 1]) { lo = a[i]; hi = a[i + 1]; }
        else                 { lo = a[i + 1]; hi = a[i]; }
        cmps++; if (lo < p.mn) p.mn = lo;           /* only the loser can win */
        cmps++; if (hi > p.mx) p.mx = hi;
    }
    return p;
}

void measure(int idx) {
    int n = nValues[idx], i;
    long long before;
    Pair d, s, q;
    int *a = (int *)malloc(sizeof(int) * n);
    for (i = 0; i < n; i++) a[i] = rand() % 100000;

    before = cmps; d = maxMin(a, 0, n - 1);  dcC[idx]    = cmps - before;
    before = cmps; s = naive(a, n);          naiveC[idx] = cmps - before;
    before = cmps; q = pairwise(a, n);       pairC[idx]  = cmps - before;

    /* All three must agree, else the counts mean nothing. */
    if (d.mn != s.mn || d.mx != s.mx || q.mn != s.mn || q.mx != s.mx) {
        printf("MISMATCH at n=%d\n", n);
        exit(1);
    }
    free(a);
}

int main(void) {
    int i;
    for (i = 0; i < NNS; i++) measure(i);

    printf("\n=====================================================\n");
    printf(" COMPARISONS TO FIND BOTH MAX AND MIN\n");
    printf("=====================================================\n");
    printf("---------------------------------------------------------------"
           "-------\n");
    printf("%8s %10s %10s %10s %12s %10s\n", "n", "D&C", "pairwise", "naive",
           "3n/2 - 2", "saved");
    printf("---------------------------------------------------------------"
           "-------\n");
    for (i = 0; i < NNS; i++) {
        int n = nValues[i];
        long long bound = 3 * n / 2 - 2;
        printf("%8d %10lld %10lld %10lld %12lld %9.1f%%\n",
               n, dcC[i], pairC[i], naiveC[i], bound,
               100.0 * (naiveC[i] - dcC[i]) / naiveC[i]);
    }

    printf("\nT(n) = 2T(n/2) + 2, T(2) = 1  ->  T(n) = 3n/2 - 2.\n");
    printf("The D&C column equals the 3n/2 - 2 column at every size, so the\n");
    printf("bound is met exactly, and it saves 25%% against the 2n - 2 scan.\n\n");
    return 0;
}
