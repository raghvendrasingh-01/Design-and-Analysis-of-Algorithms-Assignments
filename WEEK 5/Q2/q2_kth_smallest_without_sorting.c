/* q2_kth_smallest_without_sorting.c
 *
 * DAA Q2 : find the k'th smallest element of a list of n numbers without
 *          sorting the list.
 *
 * Quickselect again, but now the rank is a parameter, and the cost depends on
 * where in the list the rank sits.  With a random pivot the expected number of
 * comparisons is
 *
 *     C(n,k) = 2( n + k ln(n/k) + (n-k) ln(n/(n-k)) )
 *
 * which is 2n at both ends (k = 1 and k = n), rises to 2(1 + ln 2)n ~ 3.386n at
 * the median, and is symmetric in k <-> n+1-k.  Every row of the first table
 * below carries this prediction beside the measurement.
 *
 * A second method is measured against it: a bounded max-heap of size k, which
 * keeps the k smallest seen so far and answers with its root.  Its worst case is
 * n log2 k, but on a random list only about k ln(n/k) of the elements ever get
 * past the root test, so the expected cost is nearer
 *
 *     1.88k + (n-k) + 2 k log2(k) ln(n/k)
 *
 * -- exactly n-1 comparisons at k = 1, where it is just a scan for the minimum
 * and beats quickselect's 2n.  Where the two curves cross is the point of the
 * comparison, and the table locates it.
 *
 * The input is a shuffled permutation of 1..n, whose k'th smallest is exactly k,
 * so every answer is checked without sorting anything.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NKS 15          /* rank values swept at a fixed n */
#define NNS 6           /* list sizes swept at a fixed k/n */
#define NA 20001        /* the n used for the sweep over k */
#define ARUNS 400       /* quickselect runs per k */
#define HRUNS 40        /* heap runs per k (the heap is far more expensive) */
#define SMALLMAX 30     /* every n up to this, and every k, cross-checked */

long long selCmps = 0;      /* three-way comparisons, quickselect */
long long heapCmps = 0;     /* comparisons made by the bounded max-heap */
long long sortCmps = 0;     /* comparisons made by the reference sort */

static int kValues[NKS] = {1, 2, 5, 20, 100, 500, 2000, 5000, 8000,
                           10001, 12000, 15000, 18000, 19900, 20001};
static double aSel[NKS], aHeap[NKS];

static int nValues[NNS] = {11, 101, 1001, 10001, 100001, 1000001};
static int runsOf[NNS]  = {4000, 4000, 2000, 800, 200, 50};
static double bSel[NNS], bSort[NNS], bSd[NNS];
static int bK[NNS];

static void swap(int *a, int i, int j) { int t = a[i]; a[i] = a[j]; a[j] = t; }

/* a[lo..hi] rearranged into  < p | == p | > p , at exactly hi-lo+1 three-way
 * comparisons; the equal block is what keeps repeated keys out of trouble. */
static void partition3(int *a, int lo, int hi, int p, int *ltOut, int *gtOut) {
    int lt = lo, i = lo, gt = hi;
    while (i <= gt) {
        selCmps++;
        if (a[i] < p) swap(a, lt++, i++);
        else if (a[i] > p) swap(a, i, gt--);
        else i++;
    }
    *ltOut = lt; *gtOut = gt;
}

/* The k'th smallest, k counted from 1.  Only the side of the partition that can
 * still contain rank k is kept; the other is discarded unexamined, which is the
 * single difference from quicksort and the reason this is linear. */
static int kthSmallest(int *a, int n, int k) {
    int lo = 0, hi = n - 1, want = k - 1;
    while (lo < hi) {
        int p = a[lo + rand() % (hi - lo + 1)], lt, gt;
        partition3(a, lo, hi, p, &lt, &gt);
        if (want < lt) hi = lt - 1;
        else if (want > gt) lo = gt + 1;
        else return p;
    }
    return a[lo];
}

/* Sift a[i] down a max-heap of size m, counting every comparison. */
static void siftDown(int *a, int m, int i) {
    for (;;) {
        int l = 2 * i + 1, r = l + 1, big = i;
        if (l < m) { heapCmps++; if (a[l] > a[big]) big = l; }
        if (r < m) { heapCmps++; if (a[r] > a[big]) big = r; }
        if (big == i) return;
        swap(a, i, big);
        i = big;
    }
}

/* Bounded max-heap of size k over one pass of the list: the heap holds the k
 * smallest elements seen so far, so its root is the k'th smallest overall.
 * About 2k comparisons to build, then one comparison per remaining element plus
 * a sift-down only for those that get in -- O(n log k) in the worst case, and
 * exactly n-1 comparisons when k = 1. */
static int kthByHeap(const int *src, int n, int k) {
    int *h = malloc(sizeof(int) * (size_t)k), root;
    if (!h) { printf("out of memory\n"); exit(1); }
    for (int i = 0; i < k; i++) h[i] = src[i];
    for (int i = k / 2 - 1; i >= 0; i--) siftDown(h, k, i);
    for (int i = k; i < n; i++) {
        heapCmps++;
        if (src[i] < h[0]) { h[0] = src[i]; siftDown(h, k, 0); }
    }
    root = h[0];
    free(h);
    return root;
}

static int cmpInt(const void *x, const void *y) {
    int p = *(const int *)x, q = *(const int *)y;
    sortCmps++;
    return (p < q) ? -1 : (p > q);
}

static int kthBySorting(int *a, int n, int k) {      /* the referee */
    qsort(a, n, sizeof(int), cmpInt);
    return a[k - 1];
}

static void shuffled(int *a, int n) {           /* a permutation of 1..n */
    for (int i = 0; i < n; i++) a[i] = i + 1;
    for (int i = n - 1; i > 0; i--) swap(a, i, rand() % (i + 1));
}

static void fail(const char *what, int n, int k, int got, int want) {
    printf("MISMATCH %s: n=%d k=%d got %d want %d\n", what, n, k, got, want);
    exit(1);
}

/* C(n,k) = 2( n + k ln(n/k) + (n-k) ln(n/(n-k)) ), the expected number of
 * comparisons for a random pivot; the log terms vanish at k = 1 and k = n. */
static double predict(double n, double k) {
    double t = 2.0 * n;
    if (k > 1.0) t += 2.0 * k * log(n / k);
    if (k < n)   t += 2.0 * (n - k) * log(n / (n - k));
    return t;
}

static void example(void) {
    int list[10] = {23, 4, 91, 15, 8, 42, 16, 77, 30, 1};
    int work[10];
    int ranks[4] = {1, 3, 5, 10};
    int want[4]  = {1, 8, 16, 91};

    printf("\nworked example, n = 10\n  input :");
    for (int i = 0; i < 10; i++) printf(" %d", list[i]);
    printf("\n  sorted: 1 4 8 15 16 23 30 42 77 91   (for reference only)\n");
    for (int j = 0; j < 4; j++) {
        int got;
        for (int i = 0; i < 10; i++) work[i] = list[i];
        got = kthSmallest(work, 10, ranks[j]);
        printf("  k = %2d -> %2d", ranks[j], got);
        if (got != want[j]) fail("example", 10, ranks[j], got, want[j]);
        if (j == 0) printf("   the minimum");
        if (j == 2) printf("   the lower median");
        if (j == 3) printf("   the maximum");
        printf("\n  after :");
        for (int i = 0; i < 10; i++) printf(" %d", work[i]);
        printf("\n");
    }
}

/* Every n up to SMALLMAX, every rank 1..n, duplicate-heavy keys, all three
 * methods required to agree. */
static void smallCheck(void) {
    long long s0 = sortCmps, q0 = selCmps, h0 = heapCmps;
    int a[SMALLMAX], b[SMALLMAX], c[SMALLMAX], checks = 0;
    for (int n = 1; n <= SMALLMAX; n++)
        for (int t = 0; t < 8; t++) {
            for (int i = 0; i < n; i++)
                a[i] = b[i] = c[i] = rand() % (n / 3 + 2);      /* many ties */
            for (int k = 1; k <= n; k++) {
                int q, h, s;
                for (int i = 0; i < n; i++) a[i] = b[i];
                q = kthSmallest(a, n, k);
                h = kthByHeap(b, n, k);
                for (int i = 0; i < n; i++) c[i] = b[i];
                s = kthBySorting(c, n, k);
                if (q != s) fail("quickselect vs sort", n, k, q, s);
                if (h != s) fail("heap vs sort", n, k, h, s);
                checks++;
            }
        }
    sortCmps = s0; selCmps = q0; heapCmps = h0;     /* not part of the tables */
    printf("\ncross-check: %d (n,k) pairs with repeated keys, n = 1..%d and\n"
           "every rank 1..n, quickselect and heap and sort all agree\n",
           checks, SMALLMAX);
}

/* Sweep the rank at a fixed n. */
static void sweepK(int idx) {
    int k = kValues[idx], n = NA;
    long long s0, h0;
    int *a = malloc(sizeof(int) * (size_t)n);
    int *b = malloc(sizeof(int) * (size_t)n);
    if (!a || !b) { printf("out of memory\n"); exit(1); }

    s0 = selCmps;
    for (int t = 0; t < ARUNS; t++) {
        int got;
        shuffled(a, n);
        got = kthSmallest(a, n, k);
        if (got != k) fail("quickselect", n, k, got, k);
    }
    aSel[idx] = (double)(selCmps - s0) / ARUNS;

    h0 = heapCmps;
    for (int t = 0; t < HRUNS; t++) {
        int got;
        shuffled(b, n);
        got = kthByHeap(b, n, k);
        if (got != k) fail("heap", n, k, got, k);
    }
    aHeap[idx] = (double)(heapCmps - h0) / HRUNS;
    free(a); free(b);
}

/* Sweep n at a fixed rank fraction k = ceil(n/4), against the cost of sorting. */
static void sweepN(int idx) {
    int n = nValues[idx], runs = runsOf[idx], k = (n + 3) / 4;
    long long s0, t0;
    double sum = 0.0, sumsq = 0.0;
    int *a = malloc(sizeof(int) * (size_t)n);
    if (!a) { printf("out of memory\n"); exit(1); }
    bK[idx] = k;

    s0 = selCmps;
    for (int t = 0; t < runs; t++) {
        long long before = selCmps;
        double x;
        int got;
        shuffled(a, n);
        got = kthSmallest(a, n, k);
        if (got != k) fail("quickselect", n, k, got, k);
        x = (double)(selCmps - before) / n;
        sum += x; sumsq += x * x;
    }
    bSel[idx] = (double)(selCmps - s0) / runs;
    bSd[idx] = sqrt(sumsq / runs - (sum / runs) * (sum / runs)) / sqrt((double)runs);

    shuffled(a, n);
    t0 = sortCmps;
    if (kthBySorting(a, n, k) != k) fail("reference sort", n, k, 0, k);
    bSort[idx] = (double)(sortCmps - t0);
    free(a);
}

int main(void) {
    example();
    smallCheck();
    for (int i = 0; i < NKS; i++) sweepK(i);
    for (int i = 0; i < NNS; i++) sweepN(i);

    printf("\n=================================================\n");
    printf(" THE K'TH SMALLEST WITHOUT SORTING\n");
    printf("=================================================\n");
    printf("cost against the rank, n = %d, mean of %d runs (%d for the heap)\n",
           NA, ARUNS, HRUNS);
    printf("-------------------------------------------------------------------\n");
    printf("%6s %6s %10s %6s %7s %8s %10s %7s\n", "k", "k/n", "selCmps",
           "sel/n", "pred/n", "obs/pred", "heapCmps", "heap/n");
    printf("-------------------------------------------------------------------\n");
    for (int i = 0; i < NKS; i++) {
        double pred = predict((double)NA, (double)kValues[i]);
        printf("%6d %6.3f %10.0f %6.3f %7.3f %8.3f %10.0f %7.3f\n",
               kValues[i], (double)kValues[i] / NA, aSel[i], aSel[i] / NA,
               pred / NA, aSel[i] / pred, aHeap[i], aHeap[i] / NA);
    }

    printf("\ngrowth at a fixed rank fraction k = n/4\n");
    printf("-------------------------------------------------------------------"
           "-----------\n");
    printf("%8s %5s %8s %11s %6s %6s %7s %11s %8s\n", "n", "runs", "k",
           "selCmps", "sel/n", "+-", "pred/n", "sortCmps", "sort/sel");
    printf("-------------------------------------------------------------------"
           "-----------\n");
    for (int i = 0; i < NNS; i++) {
        int n = nValues[i];
        printf("%8d %5d %8d %11.0f %6.3f %6.3f %7.3f %11.0f %8.2f\n",
               n, runsOf[i], bK[i], bSel[i], bSel[i] / n, bSd[i],
               predict((double)n, (double)bK[i]) / n, bSort[i],
               bSort[i] / bSel[i]);
    }
    printf("+- is the standard deviation of the mean of sel/n over the runs.\n");

    printf("\nC(n,k) = 2( n + k ln(n/k) + (n-k) ln(n/(n-k)) ) is what obs/pred\n");
    printf("     tests, and it holds column by column: 2n at both ends, a peak\n");
    printf("     of 2(1+ln2)n ~ 3.386n at the median, symmetric in k <-> n+1-k,\n");
    printf("     and linear in n at any fixed fraction k/n.  It is an\n");
    printf("     asymptotic formula, so the small rows of the second table sit\n");
    printf("     below it -- %.2f against %.2f at n = 11, where a list has no\n",
           bSel[0] / nValues[0], predict((double)nValues[0], (double)bK[0]) / nValues[0]);
    printf("     room for the logarithms to mean anything; from n = 1001 on it\n");
    printf("     is inside a few per cent.\n");
    printf("The bounded max-heap is the cheaper method at both ends and much\n");
    printf("     dearer in between: %.3fn against %.3fn at k = 1, %.3fn against\n",
           aHeap[0] / NA, aSel[0] / NA, aHeap[4] / NA);
    printf("     %.3fn at k = %d, then %.3fn against %.3fn at k = %d -- so the\n",
           aSel[4] / NA, kValues[4], aHeap[5] / NA, aSel[5] / NA, kValues[5]);
    printf("     curves cross between those two ranks, and at the median the\n");
    printf("     heap costs %.1f times what quickselect does.  Its worst case\n",
           aHeap[9] / aSel[9]);
    printf("     is n log2 k, but only about k ln(n/k) elements get past the\n");
    printf("     root test on a random list, so the cost runs nearer\n");
    printf("     1.88k + (n-k) + 2 k log2(k) ln(n/k) -- some 10%% above what is\n");
    printf("     measured, since a sift-down from the root usually stops short\n");
    printf("     of the bottom.  Both ends of that estimate are exact: %.0f\n",
           aHeap[0]);
    printf("     = n-1 comparisons at k = 1, a bare scan for the minimum, and\n");
    printf("     %.3fn at k = n, where the heap is the whole list, nothing is\n",
           aHeap[NKS-1] / NA);
    printf("     ever tested, and all that is left is Floyd's build.  So the\n");
    printf("     heap curve is not symmetric in k <-> n+1-k as quickselect's\n");
    printf("     is: it peaks near k = n/e ~ %.0f (%.3fn at k = %d, against\n",
           NA / exp(1.0), aHeap[8] / NA, kValues[8]);
    printf("     %.3fn at the median) and then falls back as the build cost\n",
           aHeap[9] / NA);
    printf("     takes over from the insertions.\n");
    printf("Sorting first costs Theta(n log n) and answers every rank at once;\n");
    printf("     selection answers one rank in Theta(n), which is why sort/sel\n");
    printf("     grows from %.2f to %.2f across the decades.  Asking for many\n",
           bSort[0] / bSel[0], bSort[NNS-1] / bSel[NNS-1]);
    printf("     ranks at once is where sorting takes the lead back.\n\n");
    return 0;
}
