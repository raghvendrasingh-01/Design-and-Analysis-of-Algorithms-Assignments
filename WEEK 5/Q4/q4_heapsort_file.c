/* q4_heapsort_file.c
 *
 * DAA Q4 : implement heap sort on n randomly generated elements stored in a
 *          file, and do the complexity analysis.
 *
 * n random integers are written to a text file, read back, and sorted in place;
 * the sorted list is written out again.  Comparisons are counted the same way as
 * in Q3 -- one three-way answer per test -- so the two sorts can be put side by
 * side, and the last column of the growth table does exactly that.
 *
 * The analysis has three separate parts, and each has its own measurement here.
 *
 *   build   Floyd's bottom-up build sifts down from n/2-1 to 0.  A node at
 *           height h costs at most 2h comparisons and there are at most
 *           n/2^(h+1) of them, and sum over h of 2h/2^(h+1) = 2, so the build
 *           costs at most 2n comparisons -- linear, not n log n.  Building the
 *           same heap by inserting one element at a time and sifting up costs
 *           n log2 n in the worst case, and the second table runs both.
 *
 *   extract n-1 swaps of the root to the end, each followed by a sift-down of
 *           the full remaining height, about 2 log2 i comparisons for the i'th,
 *           so 2n log2 n in total.  This is the dominant term.
 *
 *   bound   Both parts are bounded above whatever the input looks like: there
 *           is no shape of input that makes a sift-down longer than the tree is
 *           tall.  The third table sweeps the same six shapes that took
 *           quicksort from n log n to n(n-1)/2 in Q3, and heapsort's dearest
 *           shape costs the same as its cheapest to within a small factor.
 *           That guarantee is the whole reason to prefer it, and it is paid for
 *           in the last column of the first table: about 1.4 times quicksort's
 *           average number of comparisons.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define NNS 6
#define PATN 100000     /* size used for the input-shape table */
#define NPAT 6

static const char *DATA_FILE   = "q4_random_input.txt";
static const char *SORTED_FILE = "q4_sorted_output.txt";

long long buildCmps = 0;    /* comparisons in Floyd's bottom-up build */
long long extrCmps = 0;     /* comparisons in the n-1 extractions */
long long insCmps = 0;      /* comparisons in the sift-up build */
long long swaps = 0;
static long long *tally = &buildCmps;       /* where siftDown reports */

static int nValues[NNS] = {10, 100, 1000, 10000, 100000, 1000000};
static int runsOf[NNS]  = {4000, 4000, 1000, 200, 40, 6};
static double bMean[NNS], eMean[NNS], insRand[NNS];
static double insAsc[NNS], buAsc[NNS];
static long long bytesIn[NNS], bytesOut[NNS];

static const char *patName[NPAT] = {"random", "sorted", "reversed",
                                    "all equal", "ten values", "organ pipe"};
static long long patCmps[NPAT];

static void swap(int *a, int i, int j) { int t = a[i]; a[i] = a[j]; a[j] = t; swaps++; }

/* ------------------------------------------------------------------- the heap */

/* Sift a[i] down a max-heap of size m: two comparisons per level -- one to pick
 * the larger child, one to test it against the parent -- so a sift-down of a
 * node at height h costs at most 2h. */
static void siftDown(int *a, int m, int i) {
    for (;;) {
        int l = 2 * i + 1, big;
        if (l >= m) return;
        big = l;
        if (l + 1 < m) { (*tally)++; if (a[l + 1] > a[l]) big = l + 1; }
        (*tally)++;
        if (a[i] >= a[big]) return;
        swap(a, i, big);
        i = big;
    }
}

/* Floyd, bottom-up: every subtree below i is already a heap when i is sifted. */
static void buildHeap(int *a, int n) {
    tally = &buildCmps;
    for (int i = n / 2 - 1; i >= 0; i--) siftDown(a, n, i);
}

/* The other way to build one: insert a[i] and let it climb.  One comparison per
 * level climbed, and an ascending list makes every new element climb to the
 * root, which is where the n log2 n comes from. */
static void buildByInsertion(int *a, int n) {
    for (int i = 1; i < n; i++) {
        int j = i;
        while (j > 0) {
            int par = (j - 1) / 2;
            insCmps++;
            if (a[j] <= a[par]) break;
            swap(a, j, par);
            j = par;
        }
    }
}

static void heapSort(int *a, int n) {
    buildHeap(a, n);
    tally = &extrCmps;
    for (int i = n - 1; i > 0; i--) {           /* largest to the back, sift */
        swap(a, 0, i);
        siftDown(a, i, 0);
    }
}

/* ------------------------------------------------------------------- the file */

static long long writeFile(const char *path, const int *a, int n) {
    FILE *f = fopen(path, "w");
    long long bytes;
    if (!f) { printf("cannot write %s\n", path); exit(1); }
    fprintf(f, "%d\n", n);
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

/* ------------------------------------------------------------------ checking */

static void checksum(const int *a, int n, long long *sum, long long *x) {
    long long s = 0, v = 0;
    for (int i = 0; i < n; i++) { s += a[i]; v ^= a[i]; }
    *sum = s; *x = v;
}

static void verifySorted(const int *a, int n, long long sum0, long long xor0,
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
        printf("MISMATCH %s: multiset changed\n", what);
        exit(1);
    }
}

/* Every parent at least as large as both children -- the property the whole
 * method rests on, checked directly rather than assumed. */
static void verifyHeap(const int *a, int n, const char *what) {
    for (int i = 0; i < n; i++) {
        int l = 2 * i + 1, r = l + 1;
        if ((l < n && a[l] > a[i]) || (r < n && a[r] > a[i])) {
            printf("MISMATCH %s: heap property fails at i=%d\n", what, i);
            exit(1);
        }
    }
}

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

/* The classical average for quicksort, for the last column of the first table. */
static double classicAvg(int n) {
    double h = 0.0;
    for (int i = 1; i <= n; i++) h += 1.0 / i;
    return 2.0 * (n + 1) * h - 4.0 * n;
}

static void example(void) {
    int a[10] = {23, 4, 91, 15, 8, 42, 16, 77, 30, 1};
    int work[10];
    long long sum0, xor0;

    printf("\nworked example, n = 10\n  input      :");
    for (int i = 0; i < 10; i++) printf(" %d", a[i]);
    checksum(a, 10, &sum0, &xor0);
    memcpy(work, a, sizeof a);
    buildHeap(work, 10);
    verifyHeap(work, 10, "example build");
    printf("\n  after build:");
    for (int i = 0; i < 10; i++) printf(" %d", work[i]);
    printf("\n               91 at the root, every parent >= its children\n");
    memcpy(work, a, sizeof a);
    heapSort(work, 10);
    verifySorted(work, 10, sum0, xor0, "example sort");
    printf("  sorted     :");
    for (int i = 0; i < 10; i++) printf(" %d", work[i]);
    printf("\n");
}

/* One n of the growth table: the file round trip, then runs sorts of the
 * read-back copy, then the two build methods for comparison. */
static void measure(int idx, int keep) {
    int n = nValues[idx], runs = runsOf[idx], m = 0;
    long long b0, e0, i0, sum0, xor0;
    int *gen = malloc(sizeof(int) * (size_t)n);
    int *orig, *work = malloc(sizeof(int) * (size_t)n);
    int insRuns = (runs < 20) ? runs : 20;
    if (!gen || !work) { printf("out of memory\n"); exit(1); }

    for (int i = 0; i < n; i++) gen[i] = rand();
    bytesIn[idx] = writeFile(DATA_FILE, gen, n);
    free(gen);

    orig = readFile(DATA_FILE, &m);
    if (m != n) { printf("MISMATCH file: wrote %d read %d\n", n, m); exit(1); }
    checksum(orig, n, &sum0, &xor0);

    b0 = buildCmps; e0 = extrCmps;
    for (int t = 0; t < runs; t++) {
        memcpy(work, orig, sizeof(int) * (size_t)n);
        buildHeap(work, n);
        verifyHeap(work, n, "build");
        tally = &extrCmps;
        for (int i = n - 1; i > 0; i--) { swap(work, 0, i); siftDown(work, i, 0); }
        verifySorted(work, n, sum0, xor0, "heapsort");
    }
    bMean[idx] = (double)(buildCmps - b0) / runs;
    eMean[idx] = (double)(extrCmps - e0) / runs;
    bytesOut[idx] = writeFile(SORTED_FILE, work, n);

    i0 = insCmps;                               /* sift-up build, random input */
    for (int t = 0; t < insRuns; t++) {
        memcpy(work, orig, sizeof(int) * (size_t)n);
        buildByInsertion(work, n);
        verifyHeap(work, n, "insertion build");
    }
    insRand[idx] = (double)(insCmps - i0) / insRuns;

    for (int i = 0; i < n; i++) work[i] = i;    /* ... and ascending input */
    i0 = insCmps;
    buildByInsertion(work, n);
    verifyHeap(work, n, "insertion build, ascending");
    insAsc[idx] = (double)(insCmps - i0);

    for (int i = 0; i < n; i++) work[i] = i;    /* bottom-up on the same */
    b0 = buildCmps;
    buildHeap(work, n);
    verifyHeap(work, n, "bottom-up build, ascending");
    buAsc[idx] = (double)(buildCmps - b0);

    free(orig); free(work);
    if (!keep) { remove(DATA_FILE); remove(SORTED_FILE); }
}

static void patternTable(void) {
    int *a = malloc(sizeof(int) * PATN), *b = malloc(sizeof(int) * PATN);
    if (!a || !b) { printf("out of memory\n"); exit(1); }
    for (int p = 0; p < NPAT; p++) {
        long long sum0, xor0, c0;
        makePattern(b, PATN, p);
        checksum(b, PATN, &sum0, &xor0);
        memcpy(a, b, sizeof(int) * PATN);
        c0 = buildCmps + extrCmps;
        heapSort(a, PATN);
        verifySorted(a, PATN, sum0, xor0, patName[p]);
        patCmps[p] = buildCmps + extrCmps - c0;
    }
    free(a); free(b);
}

int main(int argc, char **argv) {
    int keep = (argc > 1 && strcmp(argv[1], "--keep") == 0);
    double worst = 0.0, best = 1e30, randR;

    example();
    for (int i = 0; i < NNS; i++) measure(i, keep);
    patternTable();

    printf("\n==============================================\n");
    printf(" HEAPSORT ON N RANDOM ELEMENTS FROM A FILE\n");
    printf("==============================================\n");
    printf("------------------------------------------------------------------"
           "-----\n");
    printf("%8s %5s %10s %6s %12s %8s %8s %6s\n", "n", "runs", "build",
           "bld/n", "extract", "ext/nlgn", "tot/nlgn", "vs qs");
    printf("------------------------------------------------------------------"
           "-----\n");
    for (int i = 0; i < NNS; i++) {
        int n = nValues[i];
        double lg = (n > 1) ? log2((double)n) : 1.0, tot = bMean[i] + eMean[i];
        printf("%8d %5d %10.0f %6.3f %12.0f %8.3f %8.3f %6.2f\n",
               n, runsOf[i], bMean[i], bMean[i] / n, eMean[i],
               eMean[i] / (n * lg), tot / (n * lg), tot / classicAvg(n));
    }
    printf("vs qs is heapsort's comparisons over quicksort's exact average\n");
    printf("2(n+1)H(n) - 4n.  At n = %d the file held %lld bytes in, %lld out\n",
           nValues[NNS-1], bytesIn[NNS-1], bytesOut[NNS-1]);
    printf("%s\n", keep ? "both files kept (--keep)"
                        : "both files removed on the way out; pass --keep to look");

    printf("\nbuilding the heap: bottom-up against successive insertion\n");
    printf("--------------------------------------------------------------\n");
    printf("%8s %8s %9s %10s %11s\n", "n", "bu/n", "insRnd/n", "insAsc/n",
           "insAsc/nlgn");
    printf("--------------------------------------------------------------\n");
    for (int i = 0; i < NNS; i++) {
        int n = nValues[i];
        double lg = (n > 1) ? log2((double)n) : 1.0;
        printf("%8d %8.3f %9.3f %10.3f %11.3f\n", n, bMean[i] / n,
               insRand[i] / n, insAsc[i] / n, insAsc[i] / (n * lg));
    }
    printf("bu is Floyd's build on the same random file, insRnd the sift-up\n");
    printf("build on it, insAsc the sift-up build on 0..n-1 ascending.\n");

    printf("\ninput shape, n = %d, total comparisons\n", PATN);
    printf("---------------------------------------------\n");
    printf("%-12s %14s %9s %7s\n", "pattern", "cmps", "/nlg2n", "/random");
    printf("---------------------------------------------\n");
    randR = (double)patCmps[0] / (PATN * log2((double)PATN));
    for (int p = 0; p < NPAT; p++) {
        double r = (double)patCmps[p] / (PATN * log2((double)PATN));
        if (r > worst) worst = r;
        if (r < best) best = r;
        printf("%-12s %14lld %9.3f %7.3f\n", patName[p], patCmps[p], r,
               (double)patCmps[p] / (double)patCmps[0]);
    }

    printf("\nbuild: a node at height h costs at most 2h and there are at most\n");
    printf("     n/2^(h+1) of them, and sum(2h/2^(h+1)) = 2, so the build stays\n");
    printf("     under 2n comparisons whatever the input.  bld/n reads %.3f on\n",
           bMean[NNS-1] / nValues[NNS-1]);
    printf("     the random file at n = %d, and the ascending list -- where\n",
           nValues[NNS-1]);
    printf("     every node does sift its full height -- reads %.6f, so the\n",
           buAsc[NNS-1] / nValues[NNS-1]);
    printf("     bound is tight and the build is Theta(n), not n log n.\n");
    printf("insert: the sift-up build is Omega(n log n) instead.  Half the\n");
    printf("     indices lie in the bottom level, so an ascending list, where\n");
    printf("     every new element climbs to the root, costs at least\n");
    printf("     (n/2)(log2 n - 1); measured %.0f against %.0f for bottom-up on\n",
           insAsc[NNS-1], buAsc[NNS-1]);
    printf("     the same list, a factor of %.1f at n = %d and growing with\n",
           insAsc[NNS-1] / buAsc[NNS-1], nValues[NNS-1]);
    printf("     log n.  On random input it is linear too (insRnd/n ~ %.2f), so\n",
           insRand[NNS-1] / nValues[NNS-1]);
    printf("     the ascending column is where the two methods part.\n");
    printf("extract: n-1 sift-downs of the full height, 2 log2 i each, giving\n");
    printf("     2n log2 n; ext/nlgn reads %.3f and tot/nlgn %.3f.\n",
           eMean[NNS-1] / (nValues[NNS-1] * log2((double)nValues[NNS-1])),
           (bMean[NNS-1] + eMean[NNS-1]) / (nValues[NNS-1] * log2((double)nValues[NNS-1])));
    printf("bound: the dearest of the six shapes costs %.3f n log2 n, only\n",
           worst);
    printf("     %.2f times the random figure, and the cheapest %.3f -- that\n",
           worst / randR, best);
    printf("     one is the constant list, where every sift-down stops at the\n");
    printf("     first level and the sort is linear.  No shape costs materially\n");
    printf("     more than random, which is the difference from quicksort: the\n");
    printf("     same six shapes took its textbook form to n(n-1)/2.\n");
    printf("     Heapsort pays %.2f times quicksort's average comparisons for\n",
           (bMean[NNS-1] + eMean[NNS-1]) / classicAvg(nValues[NNS-1]));
    printf("     that guarantee, sorts in place, and is not stable.\n\n");
    return 0;
}
