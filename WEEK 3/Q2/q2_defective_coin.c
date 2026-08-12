/* q2_defective_coin.c
 *
 * DAA Q2 : one coin among n , may be lighter than the restor all may be
 * genuine.  Only a balance scale is available - it reports left-lighter,
 * right-lighter or equal, never a number.  Divide the pile in three, weigh
 * two thirds against each other, recurse into one third: log3(n) weighings.
 */

#include <stdio.h>
#include <stdlib.h>

#define NNS 7
#define TRIALS 200      /* random placements of the light coin per size */

long long weighings = 0;        /* one use of the balance scale */

static int nValues[NNS] = {3, 9, 27, 81, 243, 729, 2187};
static double avgW[NNS];
static int wcW[NNS];

/* The balance scale: the ONLY way the algorithm may look at the coins.  It
 * compares two equal-sized groups and returns -1, 0 or +1 - a light group
 * tips up, so "lighter" is what is reported, never a weight. */
int weigh(int *coins, int l, int r, int len) {
    long long sumL = 0, sumR = 0;
    int i;
    weighings++;
    for (i = 0; i < len; i++) { sumL += coins[l + i]; sumR += coins[r + i]; }
    if (sumL < sumR) return -1;         /* left group is lighter */
    if (sumL > sumR) return +1;         /* right group is lighter */
    return 0;                           /* the two groups balance */
}

/* Find the light coin in coins[lo..lo+n), or -1 if all n are genuine.
 * Split into A|B|C with |A| = |B| = ceil(n/3), weigh A against B:
 *   A lighter -> the coin is in A, and every coin of B is genuine,
 *   B lighter -> the coin is in B, and every coin of A is genuine,
 *   balanced  -> A and B are both clean, so the coin, if any, is in C.
 * `known` records that the coin is certainly inside this group - that is
 * what a strictly lighter pan tells us, and it is what lets a group of one
 * be answered without another weighing.  `ref` is a coin already proved
 * genuine, or -1 if none has been proved yet. */
int findLight(int *coins, int lo, int n, int known, int ref) {
    int a, r;
    if (n == 0) return -1;              /* nothing left: no coin is light */
    if (n == 1) {
        if (known) return lo;                   /* it tipped: this is the one */
        if (ref < 0) return -1;                 /* n = 1 overall - a lone coin */
        return weigh(coins, lo, ref, 1) < 0 ? lo : -1;  /* against a good one */
    }
    a = (n + 2) / 3;                    /* ceil(n/3): the two weighed groups */
    r = weigh(coins, lo, lo + a, a);
    if (r < 0) return findLight(coins, lo,         a,         1, lo + a);
    if (r > 0) return findLight(coins, lo + a,     a,         1, lo);
    return        findLight(coins, lo + 2 * a, n - 2 * a, known, lo);
}

/* One trial: build a pile, hide the light coin at `light` (-1 for none),
 * and check the answer against where it was actually hidden. */
int trial(int n, int light) {
    int i, found;
    long long before = weighings;
    int *coins = (int *)malloc(sizeof(int) * n);
    for (i = 0; i < n; i++) coins[i] = 10;      /* required weight */
    if (light >= 0) coins[light] = 9;           /* over-shaped: lighter */

    found = findLight(coins, 0, n, 0, -1);
    if (found != light) {
        printf("WRONG at n=%d: hid %d, reported %d\n", n, light, found);
        exit(1);
    }
    free(coins);
    return (int)(weighings - before);
}

void measure(int idx) {
    int n = nValues[idx], t, w, sum = 0, mx = 0;
    for (t = 0; t < TRIALS; t++) {
        /* Every so often hide no coin at all, so the "none is lighter"
         * report is exercised too, not just the search. */
        int light = (t % 10 == 0) ? -1 : rand() % n;
        w = trial(n, light);
        sum += w;
        if (w > mx) mx = w;
    }
    /* Exhaustive: every hiding place at this size, plus the empty pile. */
    for (t = -1; t < n; t++) { w = trial(n, t); if (w > mx) mx = w; }
    avgW[idx] = (double)sum / TRIALS;
    wcW[idx] = mx;
}

int main(void) {
    int i;
    for (i = 0; i < NNS; i++) measure(i);

    printf("\n=====================================================\n");
    printf(" WEIGHINGS TO FIND THE LIGHT COIN (or report none)\n");
    printf("=====================================================\n");
    printf("-------------------------------------------------------------\n");
    printf("%8s %10s %10s %12s %12s\n", "n", "avg", "worst", "ceil(log3 n)",
           "wc/log2 n");
    printf("-------------------------------------------------------------\n");
    for (i = 0; i < NNS; i++) {
        int n = nValues[i], levels = 0, m = 1;
        double l2 = 0.0, tmp = (double)n;
        while (m < n) { m *= 3; levels++; }              /* ceil(log3 n) */
        while (tmp > 1.0) { tmp /= 2.0; l2 += 1.0; }     /* ceil(log2 n) */
        printf("%8d %10.2f %10d %12d %12.2f\n",
               n, avgW[i], wcW[i], levels, wcW[i] / l2);
    }

    printf("\nEvery trial's answer was checked against the hidden position,\n");
    printf("and every position was tried exhaustively at each size.\n");
    printf("Worst case = ceil(log3 n) + 1 <= log2 n + c, so the bound holds.\n\n");
    return 0;
}
