/* q1_binary_vs_ternary_search.c
 *
 * DAA Q1 : search x in a sorted list of n elements by binary search (two
 * intervals) and by ternary search (three intervals).  Both are counted in
 * element comparisons, swept over n, and every result is checked against a
 * linear scan before its counts are believed.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NNS 7
#define PROBES 400      /* keys probed per size, to average the counts */

long long cmps = 0;     /* one comparison of x against a list element */

static int nValues[NNS] = {15, 63, 255, 1023, 4095, 16383, 65535};
static double avgBin[NNS], avgTer[NNS], wcBin[NNS], wcTer[NNS];

/* Binary search: one interval split, so one comparison decides the half. */
int binarySearch(int *a, int n, int x) {
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        cmps++;
        if (a[mid] == x) return mid;
        cmps++;                      /* the < test that picks the half */
        if (a[mid] < x) lo = mid + 1;
        else hi = mid - 1;
    }
    return -1;
}

/* Ternary search: two cut points, so up to four comparisons are needed to
 * decide which of the three intervals holds x. */
int ternarySearch(int *a, int n, int x) {
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int third = (hi - lo) / 3;
        int m1 = lo + third, m2 = hi - third;
        cmps++;
        if (a[m1] == x) return m1;
        cmps++;
        if (m2 != m1 && a[m2] == x) return m2;
        cmps++;
        if (x < a[m1]) { hi = m1 - 1; continue; }
        cmps++;
        if (x > a[m2]) { lo = m2 + 1; continue; }
        lo = m1 + 1; hi = m2 - 1;    /* the middle interval */
    }
    return -1;
}

int linearFind(int *a, int n, int x) {
    for (int i = 0; i < n; i++) if (a[i] == x) return i;
    return -1;
}

/* Sweep one size: probe present and absent keys, record mean and worst. */
void measure(int idx) {
    int n = nValues[idx], i;
    long long sumB = 0, sumT = 0, maxB = 0, maxT = 0;
    int *a = (int *)malloc(sizeof(int) * n);
    for (i = 0; i < n; i++) a[i] = 2 * i;            /* sorted, even keys */

    for (i = 0; i < PROBES; i++) {
        /* Alternate a key that is present with one that is absent, so the
         * successful and unsuccessful paths are both exercised. */
        int x = (i % 2) ? 2 * (rand() % n) : 2 * (rand() % n) + 1;
        long long before;
        int rb, rt, expect = linearFind(a, n, x);

        before = cmps; rb = binarySearch(a, n, x);
        if (cmps - before > maxB) maxB = cmps - before;
        sumB += cmps - before;

        before = cmps; rt = ternarySearch(a, n, x);
        if (cmps - before > maxT) maxT = cmps - before;
        sumT += cmps - before;

        /* Both must agree with the scan, else the counts mean nothing. */
        if ((expect < 0) != (rb < 0) || (expect < 0) != (rt < 0) ||
            (rb >= 0 && a[rb] != x) || (rt >= 0 && a[rt] != x)) {
            printf("MISMATCH at n=%d x=%d\n", n, x);
            exit(1);
        }
    }
    avgBin[idx] = (double)sumB / PROBES;  wcBin[idx] = (double)maxB;
    avgTer[idx] = (double)sumT / PROBES;  wcTer[idx] = (double)maxT;
    free(a);
}

int main(void) {
    int i;
    for (i = 0; i < NNS; i++) measure(i);

    printf("\n=====================================================\n");
    printf(" COMPARISONS: BINARY (2 intervals) vs TERNARY (3)\n");
    printf("=====================================================\n");
    printf("--------------------------------------------------------------"
           "----------------\n");
    printf("%8s %9s %9s %9s %9s %8s %9s %9s\n", "n", "bin avg", "ter avg",
           "bin wc", "ter wc", "ratio", "wc/log2 n", "wc/log3 n");
    printf("--------------------------------------------------------------"
           "----------------\n");
    for (i = 0; i < NNS; i++) {
        double l2 = log2((double)nValues[i]), l3 = log((double)nValues[i]) / log(3.0);
        printf("%8d %9.2f %9.2f %9.0f %9.0f %8.2f %9.2f %9.2f\n",
               nValues[i], avgBin[i], avgTer[i], wcBin[i], wcTer[i],
               avgTer[i] / avgBin[i], wcBin[i] / l2, wcTer[i] / l3);
    }

    printf("\nBinary  : 2 comparisons per level, log2(n) levels  -> 2*log2 n\n");
    printf("Ternary : 4 comparisons per level, log3(n) levels  -> 4*log3 n\n");
    printf("Ratio   : 4*log3 n / 2*log2 n = 2*log(2)/log(3) = %.4f\n",
           2.0 * log(2.0) / log(3.0));
    printf("So ternary does about 26%% more comparisons: binary is better.\n\n");
    return 0;
}
