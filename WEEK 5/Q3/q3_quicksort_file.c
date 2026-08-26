/* q3_quicksort_file.c
 *
 * DAA Q3 : implement quicksort on n random elements stored in a file.
 *
 * n random integers are written to a text file, read back, sorted, and written
 * out again, so the file is the actual input to the measurement rather than a
 * formality.  The sort is instrumented, and every comparison counted here is a
 * three-way one -- the single <0 / 0 / >0 answer a qsort comparator gives -- so
 * the counts are directly comparable with the library sort used as a yardstick.
 *
 * The implementation is the one worth writing: a random pivot, a three-way
 * partition, and recursion only on the smaller side with the larger side taken
 * by the loop, which bounds the stack at log2 n frames however the pivots fall.
 * Its average cost is 1.386 n log2 n comparisons.
 *
 * The second table is the reason all three of those choices are there.  It runs
 * three pivot rules over six input patterns and shows the textbook version --
 * last element, two-way partition -- reaching n(n-1)/2 comparisons on sorted,
 * reversed, organ-pipe and constant input, the four shapes real data most often
 * takes; median-of-three repairs the first three and not the fourth; the random
 * three-way version is the only one that survives all six.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define NNS 6           /* list sizes in the growth table */
#define PATN 10000      /* size used for the pattern table */
#define NPAT 6
#define NRULE 3

typedef enum { PIV_LAST = 0, PIV_MED3 = 1, PIV_RAND = 2 } Rule;

static const char *DATA_FILE   = "q3_random_input.txt";
static const char *SORTED_FILE = "q3_sorted_output.txt";

long long cmps = 0;         /* three-way comparisons made by the sort */
long long swaps = 0;        /* element swaps */
long long parts = 0;        /* partition calls, one comparison dearer each */
long long sortCmps = 0;     /* comparisons made by the library qsort */
static int depth = 0, maxDepth = 0;

static int nValues[NNS] = {10, 100, 1000, 10000, 100000, 1000000};
static int runsOf[NNS]  = {4000, 4000, 1000, 200, 40, 6};
static double cMean[NNS], sMean[NNS], libMean[NNS], pMean[NNS];
static int dMax[NNS];
static long long bytesIn[NNS], bytesOut[NNS];

static const char *patName[NPAT] = {"random", "sorted", "reversed",
                                    "all equal", "ten values", "organ pipe"};
static const char *ruleName[NRULE] = {"last elem", "median-3", "random 3w"};
static long long patCmps[NPAT][NRULE];
static int patDepth[NPAT][NRULE];

/* The classical average for quicksort on n distinct keys, exactly:
 *     C(n) = 2(n+1)H(n) - 4n,   H(n) = 1 + 1/2 + ... + 1/n.
 * Its leading term is 2n ln n = 1.386 n log2 n, but the -4n drags the ratio
 * c/(n log2 n) well below 1.386 at every n a computer will ever see. */
static double classicAvg(int n) {
    double h = 0.0;
    for (int i = 1; i <= n; i++) h += 1.0 / i;
    return 2.0 * (n + 1) * h - 4.0 * n;
}

static void swap(int *a, int i, int j) { int t = a[i]; a[i] = a[j]; a[j] = t; swaps++; }

/* ---------------------------------------------------------------- partitions */

/* Two-way Lomuto partition with a strict <: everything below the returned index
 * is < pivot, everything above is >= pivot.  Exactly hi-lo comparisons.  Equal
 * keys all pile up on the high side, which is the flaw the third pattern finds. */
static int partitionLomuto(int *a, int lo, int hi, int pivotIdx) {
    int p, i = lo;
    parts++;
    swap(a, pivotIdx, hi);
    p = a[hi];
    for (int j = lo; j < hi; j++) {
        cmps++;
        if (a[j] < p) { if (i != j) swap(a, i, j); i++; }
    }
    swap(a, i, hi);
    return i;
}

/* Three-way partition: a[lo..lt-1] < p, a[lt..gt] == p, a[gt+1..hi] > p, at
 * exactly hi-lo+1 comparisons.  The equal block is never recursed into, so a
 * list of d distinct values costs O(n log d) however long the list is. */
static void partition3(int *a, int lo, int hi, int p, int *ltOut, int *gtOut) {
    int lt = lo, i = lo, gt = hi;
    parts++;
    while (i <= gt) {
        cmps++;
        if (a[i] < p) swap(a, lt++, i++);
        else if (a[i] > p) swap(a, i, gt--);
        else i++;
    }
    *ltOut = lt; *gtOut = gt;
}

static int med3(int *a, int lo, int hi) {
    int mid = lo + (hi - lo) / 2, x = a[lo], y = a[mid], z = a[hi];
    cmps += 3;
    if ((x <= y && y <= z) || (z <= y && y <= x)) return mid;
    if ((y <= x && x <= z) || (z <= x && x <= y)) return lo;
    return hi;
}

/* ---------------------------------------------------------------- the sort */

/* Recursion on the smaller side, iteration on the larger: the recursive call is
 * always on at most half the window, so the depth cannot exceed log2 n even when
 * the comparison count is quadratic. */
static void quickSort(int *a, int lo, int hi, Rule rule) {
    depth++;
    if (depth > maxDepth) maxDepth = depth;
    while (lo < hi) {
        int lt, gt;
        if (rule == PIV_RAND) {
            int p = a[lo + rand() % (hi - lo + 1)];
            partition3(a, lo, hi, p, &lt, &gt);
        } else {
            int idx = (rule == PIV_MED3) ? med3(a, lo, hi) : hi;
            lt = gt = partitionLomuto(a, lo, hi, idx);
        }
        if (lt - lo < hi - gt) {                /* smaller side by recursion */
            quickSort(a, lo, lt - 1, rule);
            lo = gt + 1;
        } else {
            quickSort(a, gt + 1, hi, rule);
            hi = lt - 1;
        }
    }
    depth--;
}

static void sortWith(int *a, int n, Rule rule) {
    depth = 0; maxDepth = 0;
    quickSort(a, 0, n - 1, rule);
}

static int cmpInt(const void *x, const void *y) {
    int p = *(const int *)x, q = *(const int *)y;
    sortCmps++;
    return (p < q) ? -1 : (p > q);
}

/* ---------------------------------------------------------------- the file */

static long long writeFile(const char *path, const int *a, int n) {
    FILE *f = fopen(path, "w");
    long long bytes;
    if (!f) { printf("cannot write %s\n", path); exit(1); }
    fprintf(f, "%d\n", n);                      /* header: how many follow */
    for (int i = 0; i < n; i++) fprintf(f, "%d\n", a[i]);
    bytes = ftell(f);
    fclose(f);
    return bytes;
}

static int *readFile(const char *path, int *nOut) {
    FILE *f = fopen(path, "r");
    int n, *a;
    if (!f) { printf("cannot read %s\n", path); exit(1); }
    if (fscanf(f, "%d", &n) != 1 || n <= 0) { printf("bad header\n"); exit(1); }
    a = malloc(sizeof(int) * (size_t)n);
    if (!a) { printf("out of memory\n"); exit(1); }
    for (int i = 0; i < n; i++)
        if (fscanf(f, "%d", &a[i]) != 1) { printf("short file\n"); exit(1); }
    fclose(f);
    *nOut = n;
    return a;
}

/* ---------------------------------------------------------------- checking */

/* Sortedness plus two order-independent invariants of the multiset.  Together
 * they say the output is a permutation of the input in non-decreasing order,
 * and neither needs a second sort to establish. */
static void checksum(const int *a, int n, long long *sum, long long *x) {
    long long s = 0, v = 0;
    for (int i = 0; i < n; i++) { s += a[i]; v ^= a[i]; }
    *sum = s; *x = v;
}

static void verify(const int *a, int n, long long sum0, long long xor0,
                   const char *what) {
    long long sum1, xor1;
    for (int i = 1; i < n; i++)
        if (a[i - 1] > a[i]) {
            printf("MISMATCH %s: out of order at i=%d (%d > %d)\n",
                   what, i, a[i - 1], a[i]);
            exit(1);
        }
    checksum(a, n, &sum1, &xor1);
    if (sum1 != sum0 || xor1 != xor0) {
        printf("MISMATCH %s: multiset changed (sum %lld->%lld xor %lld->%lld)\n",
               what, sum0, sum1, xor0, xor1);
        exit(1);
    }
}

/* ---------------------------------------------------------------- patterns */

static void makePattern(int *a, int n, int pat) {
    switch (pat) {
    case 0: for (int i = 0; i < n; i++) a[i] = rand(); break;
    case 1: for (int i = 0; i < n; i++) a[i] = i; break;
    case 2: for (int i = 0; i < n; i++) a[i] = n - i; break;
    case 3: for (int i = 0; i < n; i++) a[i] = 7; break;
    case 4: for (int i = 0; i < n; i++) a[i] = rand() % 10; break;
    default:
        for (int i = 0; i < n; i++) a[i] = (i < n / 2) ? i : n - i;
        break;
    }
}

static void patternTable(void) {
    int *a = malloc(sizeof(int) * PATN), *b = malloc(sizeof(int) * PATN);
    if (!a || !b) { printf("out of memory\n"); exit(1); }
    for (int p = 0; p < NPAT; p++) {
        long long sum0, xor0;
        makePattern(b, PATN, p);
        checksum(b, PATN, &sum0, &xor0);
        for (int r = 0; r < NRULE; r++) {
            long long c0 = cmps;
            memcpy(a, b, sizeof(int) * PATN);
            sortWith(a, PATN, (Rule)r);
            verify(a, PATN, sum0, xor0, patName[p]);
            patCmps[p][r] = cmps - c0;
            patDepth[p][r] = maxDepth;
        }
    }
    free(a); free(b);
}

/* ---------------------------------------------------------------- the sweep */

/* Write n random integers to the file, read them back, and sort the read-back
 * copy runs times; the last sorted copy is written out to the second file. */
static void measure(int idx, int keep) {
    int n = nValues[idx], runs = runsOf[idx], m = 0;
    long long c0, s0, t0, p0, sum0, xor0;
    int *gen = malloc(sizeof(int) * (size_t)n);
    int *orig, *work = malloc(sizeof(int) * (size_t)n);
    if (!gen || !work) { printf("out of memory\n"); exit(1); }

    for (int i = 0; i < n; i++) gen[i] = rand();
    bytesIn[idx] = writeFile(DATA_FILE, gen, n);
    free(gen);

    orig = readFile(DATA_FILE, &m);
    if (m != n) { printf("MISMATCH file: wrote %d read %d\n", n, m); exit(1); }
    checksum(orig, n, &sum0, &xor0);

    c0 = cmps; s0 = swaps; p0 = parts; dMax[idx] = 0;
    for (int t = 0; t < runs; t++) {
        memcpy(work, orig, sizeof(int) * (size_t)n);
        sortWith(work, n, PIV_RAND);
        verify(work, n, sum0, xor0, "quicksort");
        if (maxDepth > dMax[idx]) dMax[idx] = maxDepth;
    }
    cMean[idx] = (double)(cmps - c0) / runs;
    sMean[idx] = (double)(swaps - s0) / runs;
    pMean[idx] = (double)(parts - p0) / runs;

    bytesOut[idx] = writeFile(SORTED_FILE, work, n);

    memcpy(work, orig, sizeof(int) * (size_t)n);        /* the yardstick */
    t0 = sortCmps;
    qsort(work, n, sizeof(int), cmpInt);
    verify(work, n, sum0, xor0, "library qsort");
    libMean[idx] = (double)(sortCmps - t0);

    free(orig); free(work);
    if (!keep) { remove(DATA_FILE); remove(SORTED_FILE); }
}

int main(int argc, char **argv) {
    int keep = (argc > 1 && strcmp(argv[1], "--keep") == 0);

    patternTable();
    for (int i = 0; i < NNS; i++) measure(i, keep);

    printf("\n===============================================\n");
    printf(" QUICKSORT ON N RANDOM ELEMENTS FROM A FILE\n");
    printf("===============================================\n");
    printf("random pivot, three-way partition, smaller side recursed first\n");
    printf("-------------------------------------------------------------"
           "-----------------\n");
    printf("%8s %5s %12s %12s %8s %7s %6s %6s %5s\n", "n", "runs", "cmps",
           "pred", "obs/pred", "c/nlg2n", "swp/n", "depth", "lg n");
    printf("-------------------------------------------------------------"
           "-----------------\n");
    for (int i = 0; i < NNS; i++) {
        int n = nValues[i];
        double lg = (n > 1) ? log2((double)n) : 1.0;
        double pred = classicAvg(n) + pMean[i];
        printf("%8d %5d %12.0f %12.0f %8.3f %7.3f %6.2f %6d %5.1f\n",
               n, runsOf[i], cMean[i], pred, cMean[i] / pred,
               cMean[i] / (n * lg), sMean[i] / n, dMax[i], lg);
    }
    printf("pred = 2(n+1)H(n) - 4n + one comparison per partition call, the\n");
    printf("classical average plus what the three-way scan costs over a two-way\n");
    printf("one; c/nlg2n is the same figure against the leading term alone.\n");
    printf("the file round trip, per row: %lld bytes in and %lld out at n = %d\n",
           bytesIn[NNS-1], bytesOut[NNS-1], nValues[NNS-1]);
    {   /* the yardstick: the library sort on the same read-back array */
        int n = nValues[NNS-1];
        double floorCmps = lgamma((double)n + 1.0) / log(2.0);
        printf("the library qsort on that same read-back data made %.0f\n",
               libMean[NNS-1]);
        printf("comparisons against this quicksort's %.0f -- log2(n!) = %.0f is\n",
               cMean[NNS-1], floorCmps);
        printf("the floor no comparison sort can beat, and the library one comes\n");
        printf("within %.1f%% of it while quicksort's 2 ln 2 = 1.386 average is\n",
               100.0 * (libMean[NNS-1] / floorCmps - 1.0));
        printf("%.0f%% above it -- the price of sorting in place.\n",
               100.0 * (cMean[NNS-1] / floorCmps - 1.0));
    }
    printf("%s\n", keep ? "both files kept (--keep)"
                        : "both files removed on the way out; pass --keep to look");

    printf("\npivot rule against input shape, n = %d, comparisons\n", PATN);
    printf("--------------------------------------------------------------\n");
    printf("%-11s %12s %12s %12s %8s\n", "pattern", ruleName[0], ruleName[1],
           ruleName[2], "spread");
    printf("--------------------------------------------------------------\n");
    for (int p = 0; p < NPAT; p++) {
        long long worst = patCmps[p][0], best = patCmps[p][0];
        for (int r = 1; r < NRULE; r++) {
            if (patCmps[p][r] > worst) worst = patCmps[p][r];
            if (patCmps[p][r] < best) best = patCmps[p][r];
        }
        printf("%-11s %12lld %12lld %12lld %8.0f\n", patName[p], patCmps[p][0],
               patCmps[p][1], patCmps[p][2], (double)worst / (double)best);
    }
    printf("spread is the dearest rule over the cheapest on that shape; the\n");
    printf("worst case n(n-1)/2 = %lld and the average %.0f, so every cell\n",
           (long long)PATN * (PATN - 1) / 2, classicAvg(PATN));
    printf("is somewhere on that scale.  ");
    {
        int dm = 0;
        for (int p = 0; p < NPAT; p++)
            for (int r = 0; r < NRULE; r++)
                if (patDepth[p][r] > dm) dm = patDepth[p][r];
        printf("The deepest stack anywhere in the\ntable was %d frames "
               "against lg n = %.1f, quadratic cells included.\n",
               dm, log2((double)PATN));
    }

    printf("\nT(n) = 2T(n/2) + n on an even split, T(n) = T(n-1) + n on the\n");
    printf("     worst one, so quicksort lies between Theta(n log n) and\n");
    {
        double lo = 9.9, hi = 0.0;
        for (int i = 0; i < NNS; i++) {
            double r = cMean[i] / (classicAvg(nValues[i]) + pMean[i]);
            if (r < lo) lo = r;
            if (r > hi) hi = r;
        }
        printf("     Theta(n^2).  obs/pred stays between %.3f and %.3f of the\n",
               lo, hi);
    }
    printf("     exact average over five decades, so the growth table is the\n");
    printf("     first of those.  Note that c/nlg2n reads %.2f at n = %d,\n",
           cMean[NNS-1] / (nValues[NNS-1] * log2((double)nValues[NNS-1])),
           nValues[NNS-1]);
    printf("     not 1.386: the -4n term is still worth %.2f of it there, and\n",
           4.0 / log2((double)nValues[NNS-1]));
    printf("     only vanishes as n -> infinity.\n");
    printf("The last-element pivot needs %.0f comparisons on already sorted\n",
           (double)patCmps[1][0]);
    printf("     input, %.0f times what it needs on random input, because the\n",
           (double)patCmps[1][0] / (double)patCmps[0][0]);
    printf("     partition splits off one element at a time.  At n = 1000000\n");
    printf("     that would be 5x10^11 comparisons, hours instead of a second.\n");
    printf("Median-of-three repairs sorted and reversed and organ-pipe input\n");
    printf("     but not equal keys (%lld comparisons, past n(n-1)/2 because\n",
           patCmps[3][1]);
    printf("     the rule itself pays three per call), because a two-way\n");
    printf("     partition has nowhere to put a run of equal elements.  The\n");
    printf("     three-way partition needs %lld -- one pass, no recursion --\n",
           patCmps[3][2]);
    printf("     and on ten distinct values %lld against %lld.\n",
           patCmps[4][2], patCmps[4][0]);
    printf("Recursing on the smaller side keeps the stack at log2 n frames\n");
    printf("     even where the comparison count is quadratic, which is why\n");
    printf("     the quadratic cells above finished at all.\n\n");
    return 0;
}
