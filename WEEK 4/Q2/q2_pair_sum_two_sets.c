/* q2_pair_sum_two_sets.c
 *
 * DAA Q2 : is there an a in S1 and a b in S2 with a + b = x, in O(n log n).
 * Sorting is the whole trick.  Sort S2 once in O(n log n); the partner x - a of
 * every a in S1 is then found by binary search, so n searches of O(log n) each
 * keep the total at O(n log n).  Sorting S1 too lets a two-pointer walk decide
 * it in O(n) more steps - cheaper after the sort, but it pays a second sort.
 * Brute force over all n^2 pairs is the reference answer at the smaller sizes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NNS 6
#define BFMAX 4096      /* past this, n^2 pairs is too slow - the table shows "-" */

long long sortOps = 0;      /* one comparison between two keys, made by the sort */
long long searchOps = 0;    /* one three-way probe of x - a against a[mid] */
long long walkOps = 0;      /* one step of the two-pointer walk */

static int nValues[NNS] = {8, 64, 512, 4096, 32768, 262144};
static long long sortC1[NNS], sortC2[NNS], searchC[NNS], stepC[NNS];

/* qsort exposes no hook for counting, but its comparator is invoked exactly
 * once per comparison it makes, so bumping a global here counts the sort. */
static int cmpInt(const void *x, const void *y) {
    int a = *(const int *)x, b = *(const int *)y;
    sortOps++;
    return (a > b) - (a < b);
}

/* n distinct EVEN values in pseudo-random order: element i comes from the block
 * [off + 8i, off + 8i + 6], so two elements never collide and the shuffle leaves
 * the sort real work.  All even, so an odd x can never be hit - the "no" case. */
static void fill(int *a, int n, int off) {
    for (int i = 0; i < n; i++) a[i] = off + 8 * i + 2 * (rand() % 4);
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1), t = a[i]; a[i] = a[j]; a[j] = t;
    }
}

static int findIdx(const int *a, int n, int v) {
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        searchOps++;
        if (a[mid] == v) return mid;
        if (a[mid] < v) lo = mid + 1; else hi = mid - 1;
    }
    return -1;
}

/* ALGORITHM A : S2 sorted, then one binary search per element of S1.  a comes
 * from S1 and b from sorted S2, i.e. from different sets, so the single-array
 * two-sum worry about reusing one element cannot arise - even a == b is legal. */
static int algoA(const int *s1, const int *s2s, int n, int x, int *pa, int *pb) {
    for (int i = 0; i < n; i++) {
        int j = findIdx(s2s, n, x - s1[i]);
        if (j >= 0) { *pa = s1[i]; *pb = s2s[j]; return 1; }
    }
    return 0;
}

/* ALGORITHM B : both sorted, then walk in from the smallest of S1 and the
 * largest of S2.  A sum below x needs a larger a, one above x a smaller b, so
 * each step retires an element: at most 2n - 1 steps, checked in measure(). */
static int algoB(const int *s1s, const int *s2s, int n, int x, int *pa, int *pb) {
    int i = 0, j = n - 1;
    while (i < n && j >= 0) {
        int sum = s1s[i] + s2s[j];
        walkOps++;
        if (sum == x) { *pa = s1s[i]; *pb = s2s[j]; return 1; }
        if (sum < x) i++; else j--;
    }
    return 0;
}

static int brute(const int *s1, const int *s2, int n, int x) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (s1[i] + s2[j] == x) return 1;
    return 0;
}

/* The two algorithms must agree with each other, with the answer known by
 * construction, and with the brute force wherever it is affordable; any pair
 * either one returns is re-added and must come to x.  Until all of that holds
 * the counts mean nothing, so one failure stops the run. */
static void check(const int *s1, const int *s2, const int *s1s, const int *s2s,
                  int n, int x, int expect) {
    int a = 0, b = 0, c = 0, d = 0;
    int ra = algoA(s1, s2s, n, x, &a, &b);
    int rb = algoB(s1s, s2s, n, x, &c, &d);
    if (ra != rb || ra != expect || (ra && (a + b != x || c + d != x)) ||
        (n <= BFMAX && brute(s1, s2, n, x) != ra)) {
        printf("MISMATCH n=%d x=%d: A=%d (%d+%d) B=%d (%d+%d) want=%d\n",
               n, x, ra, a, b, rb, c, d, expect);
        exit(1);
    }
}

static void measure(int idx) {
    int n = nValues[idx];
    int *s1 = (int *)malloc(sizeof(int) * n), *s2 = (int *)malloc(sizeof(int) * n);
    int *s1s = (int *)malloc(sizeof(int) * n), *s2s = (int *)malloc(sizeof(int) * n);
    fill(s1, n, 0); fill(s2, n, 4);
    for (int i = 0; i < n; i++) { s1s[i] = s1[i]; s2s[i] = s2[i]; }
    sortOps = 0; qsort(s1s, n, sizeof(int), cmpInt); sortC1[idx] = sortOps;
    sortOps = 0; qsort(s2s, n, sizeof(int), cmpInt); sortC2[idx] = sortOps;
    /* Counts come from the unreachable target: nothing is found, so every one
     * of the n searches runs to full depth and the walk runs with no exit. */
    searchOps = walkOps = 0;
    check(s1, s2, s1s, s2s, n, s1[0] + s2[0] + 1, 0);   /* odd sum: a certain "no" */
    searchC[idx] = searchOps; stepC[idx] = walkOps;
    if (stepC[idx] > 2LL * n - 1) {                     /* B's claimed step bound */
        printf("MISMATCH n=%d: walk %lld steps > 2n-1\n", n, stepC[idx]); exit(1);
    }
    check(s1, s2, s1s, s2s, n, s1[n/3] + s2[2*n/3], 1); /* a real pair: "yes" */
    check(s1, s2, s1s, s2s, n, s1s[0] + s2s[0], 1);     /* sole pair, at index 0 */
    check(s1, s2, s1s, s2s, n, s1s[n-1] + s2s[n-1], 1); /* sole pair, at index n-1 */
    free(s1); free(s2); free(s1s); free(s2s);
}

int main(void) {
    for (int i = 0; i < NNS; i++) measure(i);

    printf("\n=====================================================\n");
    printf(" A PAIR FROM S1 AND S2 SUMMING TO x : O(n log n)\n");
    printf("=====================================================\n");
    printf("---------------------------------------------------------------"
           "------------\n");
    printf("%8s %10s %10s %11s %10s %12s %8s\n", "n", "sort S1", "sort S2",
           "search cmps", "walk steps", "brute n^2", "A/nlgn");
    printf("---------------------------------------------------------------"
           "------------\n");
    for (int i = 0; i < NNS; i++) {
        int n = nValues[i];
        char bf[24];
        double theory = (double)n * log2((double)n);
        if (n <= BFMAX) snprintf(bf, sizeof bf, "%lld", (long long)n * n);
        else snprintf(bf, sizeof bf, "-");
        printf("%8d %10lld %10lld %11lld %10lld %12s %8.2f\n", n, sortC1[i],
               sortC2[i], searchC[i], stepC[i], bf,
               (sortC2[i] + searchC[i]) / theory);
    }

    printf("\nThe A/nlgn column - sort S2 plus the searches - stays inside\n");
    printf("1.5-2.0 while n grows 32768x, so A is Theta(n log n), the bound\n");
    printf("asked for, and its two halves cost about the same.  B buys a second\n");
    printf("sort for a walk of 2n - 1 steps; brute force's n^2 dwarfs them all.\n\n");
    return 0;
}
