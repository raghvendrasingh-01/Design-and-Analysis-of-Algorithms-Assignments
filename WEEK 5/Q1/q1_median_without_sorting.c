/* q1_median_without_sorting.c
 *
 * DAA Q1 : find the median of a list of n numbers without sorting the list.
 *
 * The median is the element of rank n/2, so the problem is selection, not
 * sorting.  Quickselect partitions the list around a random pivot and then
 * descends into the one side that can still hold the wanted rank, discarding
 * the other outright: expected 2(1 + ln 2)n ~ 3.386n three-way comparisons,
 * worst case Theta(n^2).  The median-of-medians pivot (BFPRT, groups of five)
 * pays a larger constant for a Theta(n) worst case.  Both are measured against
 * the n log2 n a sort would have cost.
 *
 * For even n the two middle order statistics are needed.  Only the upper one is
 * selected; the lower one is then the maximum of a[0..n/2-1], which the
 * partition invariant has already isolated -- one linear scan, not a second
 * selection.
 *
 * The measured input is a shuffled permutation of 1..n, so the answer is known
 * in closed form -- the median of any permutation of 1..n is (n+1)/2 -- and the
 * count of positions still away from their sorted index is available for free.
 * Nothing is sorted to check the answer.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NNS 6
#define DUPN 10000      /* size of the duplicate-key demonstration */
#define SMALLMAX 40     /* small-n cross-check runs at every n up to this */
#define SMALLREP 40     /* random instances per small n */

long long selCmps = 0;      /* three-way comparisons, quickselect */
long long momCmps = 0;      /* three-way comparisons, median of medians */
long long sortCmps = 0;     /* comparisons made by the reference sort */
long long lomCmps = 0;      /* two-way Lomuto partition, duplicates demo */

static int nValues[NNS] = {11, 101, 1001, 10001, 100001, 1000001};
static int runsOf[NNS]  = {4000, 4000, 2000, 800, 200, 50};
static double selMean[NNS], momMean[NNS], sortMean[NNS], movedPct[NNS];
static double selSd[NNS];           /* sd of the mean of sel/n, per row */
static double evenSel = 0.0, evenScan = 0.0;   /* the two halves at an even n */

static void swap(int *a, int i, int j) { int t = a[i]; a[i] = a[j]; a[j] = t; }

/* Three-way (fat pivot) partition of a[lo..hi] into  < p | == p | > p.
 * The window i..gt shrinks by exactly one per iteration, so the loop makes
 * exactly hi-lo+1 three-way comparisons -- and on an all-equal window it ends
 * with lt == lo, gt == hi, which is what keeps duplicate keys linear. */
static void partition3(int *a, int lo, int hi, int p, int *ltOut, int *gtOut,
                       long long *cmps) {
    int lt = lo, i = lo, gt = hi;
    while (i <= gt) {
        (*cmps)++;
        if (a[i] < p) swap(a, lt++, i++);
        else if (a[i] > p) swap(a, i, gt--);
        else i++;
    }
    *ltOut = lt; *gtOut = gt;
}

/* k is a 0-based rank.  Written as a loop, not a recursion, so the stack does
 * not grow even on the worst-case sequence of pivots. */
static int quickSelect(int *a, int n, int k) {
    int lo = 0, hi = n - 1;
    while (lo < hi) {
        int p = a[lo + rand() % (hi - lo + 1)], lt, gt;
        partition3(a, lo, hi, p, &lt, &gt, &selCmps);
        if (k < lt) hi = lt - 1;            /* the wanted rank is on the left */
        else if (k > gt) lo = gt + 1;       /* ... or on the right */
        else return p;                      /* ... or inside the equal block */
    }
    return a[lo];
}

static void insertSort(int *a, int n, long long *cmps) {
    for (int i = 1; i < n; i++) {
        int v = a[i], j = i - 1;
        while (j >= 0) {
            (*cmps)++;
            if (a[j] <= v) break;
            a[j + 1] = a[j]; j--;
        }
        a[j + 1] = v;
    }
}

/* Median of medians: sort each group of five, select the median of the n/5
 * group medians, and partition around that.  At least 3n/10 elements fall on
 * each side of it, so the surviving window is at most 7n/10 and
 * T(n) <= T(n/5) + T(7n/10) + cn, which is Theta(n) because 1/5 + 7/10 < 1.
 * The 7n/10 branch is the loop; only the n/5 branch recurses, so the stack
 * depth is log_5 n. */
static int momSelect(int *a, int lo, int hi, int k) {
    while (hi - lo + 1 > 5) {
        int n = hi - lo + 1, m = 0, lt, gt, p;
        int *med = malloc(sizeof(int) * (size_t)((n + 4) / 5));
        if (!med) { printf("out of memory\n"); exit(1); }
        for (int i = lo; i <= hi; i += 5) {
            int r = (i + 4 <= hi) ? i + 4 : hi;
            insertSort(a + i, r - i + 1, &momCmps);
            med[m++] = a[i + (r - i) / 2];
        }
        p = momSelect(med, 0, m - 1, (m - 1) / 2);
        free(med);
        partition3(a, lo, hi, p, &lt, &gt, &momCmps);
        if (k < lt) hi = lt - 1;
        else if (k > gt) lo = gt + 1;
        else return p;
    }
    insertSort(a + lo, hi - lo + 1, &momCmps);
    return a[k];
}

/* The upper middle by selection; for even n the lower middle is the largest of
 * the n/2 elements the partition invariant has left in a[0..n/2-1]. */
static double medianQuickselect(int *a, int n) {
    int k = n / 2, up = quickSelect(a, n, k), down;
    if (n % 2) return (double)up;
    down = a[0];
    for (int i = 1; i < k; i++) { selCmps++; if (a[i] > down) down = a[i]; }
    return (down + (double)up) / 2.0;
}

static double medianMom(int *a, int n) {
    int k = n / 2, up = momSelect(a, 0, n - 1, k), down;
    if (n % 2) return (double)up;
    down = a[0];
    for (int i = 1; i < k; i++) { momCmps++; if (a[i] > down) down = a[i]; }
    return (down + (double)up) / 2.0;
}

static int cmpInt(const void *x, const void *y) {
    int p = *(const int *)x, q = *(const int *)y;
    sortCmps++;
    return (p < q) ? -1 : (p > q);
}

/* The method being avoided, kept only as a yardstick and a referee. */
static double medianBySorting(int *a, int n) {
    qsort(a, n, sizeof(int), cmpInt);
    return (n % 2) ? (double)a[n / 2] : (a[n / 2 - 1] + (double)a[n / 2]) / 2.0;
}

/* Two-way Lomuto partition with a strict <, the textbook version.  Kept to be
 * shown failing on duplicate keys: an all-equal window puts the pivot at lo and
 * hands back a window only one element shorter. */
static int lomutoSelect(int *a, int n, int k) {
    int lo = 0, hi = n - 1;
    while (lo < hi) {
        int r = lo + rand() % (hi - lo + 1), p, i = lo;
        swap(a, r, hi); p = a[hi];
        for (int j = lo; j < hi; j++) {
            lomCmps++;
            if (a[j] < p) swap(a, i++, j);
        }
        swap(a, i, hi);
        if (k < i) hi = i - 1; else if (k > i) lo = i + 1; else return a[i];
    }
    return a[lo];
}

static void shuffled(int *a, int n) {           /* a permutation of 1..n */
    for (int i = 0; i < n; i++) a[i] = i + 1;
    for (int i = n - 1; i > 0; i--) swap(a, i, rand() % (i + 1));
}

static void fail(const char *what, int n, double got, double want) {
    printf("MISMATCH %s: n=%d got %.1f want %.1f\n", what, n, got, want);
    exit(1);
}

/* Two hand-checked lists, one of each parity, with the array printed after the
 * selection to show that no sorted order was produced along the way. */
static void example(void) {
    int odd[9]  = {7, 12, 3, 9, 21, 5, 18, 1, 14};
    int even[6] = {4, 8, 15, 16, 23, 42};
    int work[9];
    double m;

    for (int i = 0; i < 9; i++) work[i] = odd[i];
    m = medianQuickselect(work, 9);
    printf("\nworked example, odd n = 9\n  input   :");
    for (int i = 0; i < 9; i++) printf(" %d", odd[i]);
    printf("\n  sorted  : 1 3 5 7 9 12 14 18 21   (for reference only)\n");
    printf("  median  : %.1f\n  after   :", m);
    for (int i = 0; i < 9; i++) printf(" %d", work[i]);
    printf("\n            only rank 4 is in place; the rest is still unsorted\n");
    if (m != 9.0) fail("odd example", 9, m, 9.0);

    for (int i = 0; i < 6; i++) work[i] = even[i];
    m = medianQuickselect(work, 6);
    printf("\nworked example, even n = 6\n  input   :");
    for (int i = 0; i < 6; i++) printf(" %d", even[i]);
    printf("\n  median  : %.1f   = (15 + 16) / 2, both middles from one pass\n", m);
    if (m != 15.5) fail("even example", 6, m, 15.5);
}

/* Every n up to SMALLMAX, both parities, duplicate-heavy keys, three methods
 * required to agree.  This is where correctness is established; the table below
 * only measures cost. */
static void smallCheck(void) {
    long long ss = sortCmps, sq = selCmps, sm = momCmps;
    int a[SMALLMAX], b[SMALLMAX], c[SMALLMAX], checks = 0;
    for (int n = 1; n <= SMALLMAX; n++)
        for (int t = 0; t < SMALLREP; t++) {
            for (int i = 0; i < n; i++)
                a[i] = b[i] = c[i] = rand() % (n / 2 + 2);   /* many ties */
            double q = medianQuickselect(a, n);
            double m = medianMom(b, n);
            double s = medianBySorting(c, n);
            if (q != s) fail("quickselect vs sort", n, q, s);
            if (m != s) fail("medians vs sort", n, m, s);
            checks++;
        }
    sortCmps = ss; selCmps = sq; momCmps = sm;      /* not part of the table */
    printf("\ncross-check: %d duplicate-heavy instances at n = 1..%d, "
           "all three agree\n", checks, SMALLMAX);
}

/* The two-way partition against the three-way one, on distinct keys and then on
 * keys that are all equal. */
static void dupDemo(void) {
    int *a = malloc(sizeof(int) * DUPN), *b = malloc(sizeof(int) * DUPN);
    long long l0, s0;
    int k = DUPN / 2;

    printf("\nduplicate keys, n = %d, selecting rank %d\n", DUPN, k);
    printf("  keys           two-way cmps   three-way cmps      ratio\n");

    shuffled(a, DUPN);
    for (int i = 0; i < DUPN; i++) b[i] = a[i];
    l0 = lomCmps; s0 = selCmps;
    if (lomutoSelect(a, DUPN, k) != quickSelect(b, DUPN, k))
        fail("distinct: two-way vs three-way", DUPN, 0, 0);
    printf("  all distinct   %12lld   %14lld   %8.2f\n", lomCmps - l0,
           selCmps - s0, (double)(lomCmps - l0) / (double)(selCmps - s0));

    for (int i = 0; i < DUPN; i++) a[i] = b[i] = 42;
    l0 = lomCmps; s0 = selCmps;
    if (lomutoSelect(a, DUPN, k) != 42 || quickSelect(b, DUPN, k) != 42)
        fail("all equal: wrong value", DUPN, 0, 42);
    printf("  all equal      %12lld   %14lld   %8.2f\n", lomCmps - l0,
           selCmps - s0, (double)(lomCmps - l0) / (double)(selCmps - s0));
    free(a); free(b);
}

/* Mean cost over runsOf[idx] independent shuffles.  The expected median of a
 * permutation of 1..n is (n+1)/2 exactly, so every run is verified without a
 * sort; the reference sort runs once, purely as the yardstick. */
static void measure(int idx) {
    int n = nValues[idx], runs = runsOf[idx];
    long long s0, m0, t0, moved = 0;
    double sum = 0.0, sumsq = 0.0;
    int *a = malloc(sizeof(int) * (size_t)n);
    int *b = malloc(sizeof(int) * (size_t)n);
    double want = (n + 1) / 2.0;
    if (!a || !b) { printf("out of memory\n"); exit(1); }

    s0 = selCmps; m0 = momCmps;
    for (int t = 0; t < runs; t++) {
        long long before = selCmps;
        double x;
        shuffled(a, n);
        for (int i = 0; i < n; i++) b[i] = a[i];
        double q = medianQuickselect(a, n);
        double m = medianMom(b, n);
        if (q != want) fail("quickselect", n, q, want);
        if (m != want) fail("medians", n, m, want);
        for (int i = 0; i < n; i++) if (a[i] != i + 1) moved++;
        x = (double)(selCmps - before) / n;         /* this run's sel/n */
        sum += x; sumsq += x * x;
    }
    /* the spread of sel/n over the runs, so each row reports its own precision:
     * the pivots are random, and one run of quickselect has sd of about 1.05n */
    selSd[idx] = sqrt(sumsq / runs - (sum / runs) * (sum / runs)) / sqrt((double)runs);
    selMean[idx] = (double)(selCmps - s0) / runs;
    momMean[idx] = (double)(momCmps - m0) / runs;
    movedPct[idx] = 100.0 * (double)moved / ((double)n * runs);

    shuffled(a, n);                       /* the yardstick, measured once */
    t0 = sortCmps;
    if (medianBySorting(a, n) != want) fail("reference sort", n, 0, want);
    sortMean[idx] = (double)(sortCmps - t0);
    free(a); free(b);
}

/* The same measurement at an even n, where the median is the mean of two order
 * statistics: the upper one by selection, the lower one as the maximum of the
 * n/2 elements the partition has already isolated in a[0..n/2-1]. */
static void measureEven(int n, int runs) {
    int *a = malloc(sizeof(int) * (size_t)n);
    double want = (n + 1) / 2.0;
    long long selPart = 0, scanPart = 0;
    if (!a) { printf("out of memory\n"); exit(1); }
    for (int t = 0; t < runs; t++) {
        long long b;
        int up, down;
        shuffled(a, n);
        b = selCmps;
        up = quickSelect(a, n, n / 2);              /* the upper middle */
        selPart += selCmps - b;
        b = selCmps;
        down = a[0];                                /* ... and the lower one */
        for (int i = 1; i < n / 2; i++) { selCmps++; if (a[i] > down) down = a[i]; }
        scanPart += selCmps - b;
        if ((down + (double)up) / 2.0 != want)
            fail("even n", n, (down + (double)up) / 2.0, want);
    }
    evenSel = (double)selPart / ((double)n * runs);
    evenScan = (double)scanPart / ((double)n * runs);
    free(a);
}

int main(void) {
    example();
    smallCheck();
    dupDemo();
    for (int i = 0; i < NNS; i++) measure(i);
    measureEven(100000, 200);

    printf("\n=========================================================\n");
    printf(" THE MEDIAN WITHOUT SORTING: SELECTION IN LINEAR TIME\n");
    printf("=========================================================\n");
    printf("--------------------------------------------------------------"
           "------------\n");
    printf("%8s %5s %10s %6s %6s %6s %10s %8s %7s\n", "n", "runs",
           "selCmps", "sel/n", "+-", "mom/n", "sortCmps", "sort/sel",
           "moved%");
    printf("--------------------------------------------------------------"
           "------------\n");
    for (int i = 0; i < NNS; i++) {
        int n = nValues[i];
        printf("%8d %5d %10.0f %6.3f %6.3f %6.3f %10.0f %8.2f %7.3f\n",
               n, runsOf[i], selMean[i], selMean[i] / n, selSd[i],
               momMean[i] / n, sortMean[i], sortMean[i] / selMean[i],
               movedPct[i]);
    }
    printf("+- is the standard deviation of the mean of sel/n over the runs;\n");
    printf("one run has a spread of about 1.05n, so it shrinks as 1/sqrt(runs).\n");

    printf("\neven n = 100000, mean of 200 runs: the upper middle costs\n");
    printf("     %.3fn and the lower one a further n/2 - 1 = %.5fn exactly,\n",
           evenSel, evenScan);
    printf("     the maximum of the block the partition has already isolated\n");
    printf("     below it -- %.3fn in all, and still only one selection.\n",
           evenSel + evenScan);

    printf("\nmom/n rises per decade by");
    for (int i = 1; i < NNS; i++)
        printf(" %+.2f", momMean[i] / nValues[i] - momMean[i-1] / nValues[i-1]);
    printf(",\n     a step that shrinks toward zero; a hidden log2 n factor\n");
    printf("     would instead add a fixed %.2f every decade.\n", log2(10.0));

    printf("\nT(n) = T(n/2) + n on the average pivot  ->  E[C] = 2(1+ln2)n\n");
    printf("     ~ 3.386n, and sel/n stays inside a band around that value\n");
    printf("     across five decades instead of growing: the median costs\n");
    printf("     Theta(n), no order statistic other than rank n/2 is ever\n");
    printf("     computed, and moved%% shows the list left unsorted.\n");
    printf("T(n) <= T(n/5) + T(7n/10) + cn  ->  Theta(n) worst case for the\n");
    printf("     median-of-medians pivot, at a constant a few times larger,\n");
    printf("     which is the price of never being unlucky.\n");
    printf("Sorting first would cost Theta(n log n), so sort/sel grows like\n");
    printf("     log2(n)/3.386: %.2f at n = %d, %.2f at n = %d, and the gap\n",
           sortMean[0] / selMean[0], nValues[0],
           sortMean[NNS-1] / selMean[NNS-1], nValues[NNS-1]);
    printf("     widens by a further factor of log2(10) with every decade.\n");
    printf("The two-way partition is the trap: with a strict < it makes\n");
    printf("     3n^2/8 comparisons on equal keys (37502499 at n = 10000,\n");
    printf("     against 10000 for the three-way one), so duplicates alone\n");
    printf("     turn the linear algorithm quadratic.\n\n");
    return 0;
}
