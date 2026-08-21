/* q3_k_sum_subset.c
 *
 * DAA Q3 : do k of the n integers in S add up to T, in O(n^(k-1) log n).
 * Sort S once in O(n log n), then enumerate every strictly increasing tuple of
 * k-1 indices - C(n-1, k-1) = O(n^(k-1)) of them - and for each binary search
 * the suffix past the last chosen index for the residual value.  Each search is
 * O(log n), so the total is O(n^(k-1) log n) and the sort is dominated for
 * k >= 2.  Run for k = 2, 3, 4 on a reachable and an unreachable target, both
 * cross-checked by brute force over all C(n, k) subsets, plus a random fuzz. */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NKS 3
#define NNS 4
#define RULE "------------------------------------------------------------------------------"

long long probes = 0;       /* one probe of the sorted suffix, i.e. one midpoint test */
long long tuples = 0;       /* one (k-1)-tuple of indices enumerated */
long long bfSubsets = 0;    /* one complete k-subset examined by brute force */

static int kValues[NKS] = {2, 3, 4};
static int nValues[NKS][NNS] = {{500, 1000, 2000, 4000}, {50, 100, 200, 400},
                                {25, 40, 60, 90}};
static long long probeC[NKS][NNS], tupleC[NKS][NNS], bfC[NKS][NNS];

static int cmpInt(const void *x, const void *y) {
    int a = *(const int *)x, b = *(const int *)y; return (a > b) - (a < b);
}
/* n distinct multiples of 10, shuffled so the sort has real work to do. */
static void fill(int *a, int n) {
    a[0] = 10 * (1 + rand() % 20);
    for (int i = 1; i < n; i++) a[i] = a[i - 1] + 10 * (1 + rand() % 20);
    for (int i = n - 1; i > 0; i--) { int j = rand() % (i + 1), t = a[i]; a[i] = a[j]; a[j] = t; }
}

/* Binary search a[lo..hi] for v; returns its index or -1. */
static int bsearchRange(int *a, int lo, int hi, int v) {
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2; probes++;
        if (a[mid] == v) return mid;
        if (a[mid] < v) lo = mid + 1; else hi = mid - 1; }
    return -1;
}

/* Choose the index at position 'depth' from a[start..], then recurse; at depth
 * k-1 the k-1 indices are fixed and the last element is searched for instead of
 * looped over, so the recursion is only k-1 deep.  Why the SUFFIX
 * a[i_{k-1}+1 .. n-1] is the right range: once S is sorted every k-element
 * subset has exactly one form as sorted indices j1 < ... < jk, and it is
 * reported precisely when the enumerated tuple is (j1..j_{k-1}) and the search
 * lands on jk, which by sortedness lies after i_{k-1}.  So nothing is missed or
 * double counted, and the k indices are forced DISTINCT: the searched range
 * excludes the k-1 positions already chosen, which a search over the whole
 * array could not promise. */
static int search(int *a, int n, int k, int start, int depth, long long sum,
                  int T, int *idx) {
    if (depth == k - 1) {
        int j = (tuples++, bsearchRange(a, start, n - 1, (int)(T - sum)));
        if (j < 0) return 0;
        idx[k - 1] = j; return 1;
    }
    for (int i = start; i <= n - k + depth; i++) {  /* leave room for the rest */
        idx[depth] = i;
        if (search(a, n, k, i + 1, depth + 1, sum + a[i], T, idx)) return 1; }
    return 0;
}
/* Reference answer: all C(n, k) subsets, a depth-k recursion, O(n^k). */
static int brute(int *a, int n, int k, int start, int depth, long long sum, int T) {
    if (depth == k) { bfSubsets++; return sum == T; }
    for (int i = start; i <= n - k + depth; i++)
        if (brute(a, n, k, i + 1, depth + 1, sum + a[i], T)) return 1;
    return 0;
}

/* Accept a yes only if the k indices come back increasing (hence distinct) and
 * the k values really do add up to T. */
static void check(int *a, int k, int T, int *idx) {
    long long s = 0;
    for (int i = 0; i < k; i++) {
        if (i && idx[i] <= idx[i - 1]) {
            printf("MISMATCH k=%d: indices not distinct\n", k); exit(1); }
        s += a[idx[i]];
    }
    if (s != T) { printf("MISMATCH k=%d: sum %lld != T %d\n", k, s, T); exit(1); }
}

/* Randomised differential check: search vs brute on small instances (duplicates
 * and negatives allowed) over EVERY target in range.  The two crafted targets in
 * measure() cannot see a sweep off-by-one - their solutions sit at interior
 * indices, and the unreachable one stays unreachable even for an incomplete
 * search - so this is what actually pins completeness down. */
static void fuzz(void) {
    int a[14], idx[4], n, k, got;
    for (int t = 0; t < 3000; t++) {
        n = 2 + rand() % 12; k = 2 + rand() % 3;
        if (k > n) continue;
        for (int i = 0; i < n; i++) a[i] = rand() % 21 - 10;
        qsort(a, n, sizeof(int), cmpInt);
        for (int T = -30; T <= 30; T++) {
            got = search(a, n, k, 0, 0, 0, T, idx);
            if (got != brute(a, n, k, 0, 0, 0, T)) {
                printf("MISMATCH fuzz n=%d k=%d T=%d\n", n, k, T); exit(1); }
            if (got) check(a, k, T, idx);
        }
    }
}

static void measure(int ki, int ni) {
    int k = kValues[ki], n = nValues[ki][ni], idx[4], T;
    long long bp, bt, bb, sum = 0;
    int *a = (int *)malloc(sizeof(int) * n);
    fill(a, n);
    qsort(a, n, sizeof(int), cmpInt);               /* the single O(n log n) sort */
    for (int i = 0; i < k; i++) sum += a[(i + 1) * n / (k + 1)];
    T = (int)sum;                                  /* k chosen elements: reachable */
    if (!search(a, n, k, 0, 0, 0, T, idx) || !brute(a, n, k, 0, 0, 0, T)) {
        printf("MISMATCH k=%d n=%d: reachable T=%d not found\n", k, n, T); exit(1); }
    check(a, k, T, idx);
    /* Unreachable by divisibility: every element is a multiple of 10, so any k
     * of them sum to 0 mod 10 while T+5 is 5 mod 10.  Also the worst case -
     * nothing is found, so both full enumerations run - hence the table uses it. */
    T += 5;
    bp = probes; bt = tuples; bb = bfSubsets;
    if (search(a, n, k, 0, 0, 0, T, idx) || brute(a, n, k, 0, 0, 0, T)) {
        printf("MISMATCH k=%d n=%d: unreachable T=%d found\n", k, n, T); exit(1); }
    probeC[ki][ni] = probes - bp;
    tupleC[ki][ni] = tuples - bt;
    bfC[ki][ni] = bfSubsets - bb;
    free(a);
}

int main(void) {
    for (int ki = 0; ki < NKS; ki++)
        for (int ni = 0; ni < NNS; ni++) measure(ki, ni);
    fuzz();     /* after the sweep, so the table's rand() stream is untouched */

    printf("\n=====================================================\n"
           " k OF n INTEGERS SUMMING TO T : SORT ONCE, THEN SEARCH\n"
           "=====================================================\n%s\n", RULE);
    printf("%4s %6s %12s %12s %14s %8s %13s\n%s\n", "k", "n", "probes", "tuples",
           "n^(k-1)lg n", "ratio", "C(n,k) bf", RULE);
    for (int ki = 0; ki < NKS; ki++)
        for (int ni = 0; ni < NNS; ni++) {
            int k = kValues[ki], n = nValues[ki][ni];
            double theory = pow((double)n, k - 1) * log2((double)n);
            printf("%4d %6d %12lld %12lld %14.0f %8.3f %13lld\n", k, n,
                   probeC[ki][ni], tupleC[ki][ni], theory,
                   probeC[ki][ni] / theory, bfC[ki][ni]);
        }

    printf("\nFor each fixed k the ratio holds one constant scale as n grows, rising\n"
           "towards 1/(k-1)! from below - only increasing tuples are enumerated and the\n"
           "suffixes are shorter than n - never trending like n, so the probes are\n"
           "Theta(n^(k-1) log n).  Against the naive C(n,k) column one factor of n is\n"
           "traded for a log n: the k-th element is found by search, not a loop.\n\n");
    return 0;
}
