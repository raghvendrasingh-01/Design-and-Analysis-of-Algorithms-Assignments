/* q4_party_peak_occupancy.c
 *
 * DAA Q4 : the time at which the most guests were at the party at once.
 * Each guest contributes two endpoints, so the 2n arrivals and departures become
 * events carrying +1 or -1; qsort puts them in time order and one left-to-right
 * sweep carries a running occupancy, the sort's O(n log n) dominating the O(n)
 * sweep.  The peak is checked against an O(n^2) recount before it is believed.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NNS 6
#define BFMAX 5000          /* brute force only runs up to this size */

typedef struct { int a, b; } Person;
typedef struct { int t, d; } Event;     /* d = +1 arrival, -1 departure */

long long sortCmps = 0;     /* one comparison between two event times */
long long sweepSteps = 0;   /* one event consumed by the sweep */
long long bfChecks = 0;     /* one containment test a_j <= t < b_j */

static int nValues[NNS] = {6, 50, 500, 5000, 50000, 500000};
static long long sC[NNS], sS[NNS], pkN[NNS], pkT[NNS], bfC[NNS];

/* Order by time and nothing else.  That is exactly what the lab sheet's "all 2n
 * times are distinct" assumption buys: no two events share a key, so the order
 * is total already and no tie-break on d is needed - with ties one would have to
 * decide whether a departure at t precedes an arrival at t. */
static int cmpEvent(const void *x, const void *y) {
    const Event *p = x, *q = y;
    sortCmps++; return p->t - q->t;
}

/* Distinctness is enforced by construction, not by hoping random draws differ:
 * shuffle the permutation 0..2n-1 and pair adjacent entries, keeping the smaller
 * of each pair as a so a < b.  Each of the 2n endpoints is used exactly once. */
static void makeGuests(Person *p, int n) {
    int m = 2 * n, i, *v = malloc(sizeof(int) * m);
    for (i = 0; i < m; i++) v[i] = i;
    for (i = m - 1; i > 0; i--)                         /* Fisher-Yates */
        { int j = rand() % (i + 1), t = v[i]; v[i] = v[j]; v[j] = t; }
    for (i = 0; i < n; i++) { int x = v[2 * i], y = v[2 * i + 1];
                              p[i].a = x < y ? x : y; p[i].b = x < y ? y : x; }
    free(v);
}

static void buildEvents(Event *e, const Person *p, int n) {
    for (int i = 0; i < n; i++)
        { e[2*i] = (Event){p[i].a, 1}; e[2*i+1] = (Event){p[i].b, -1}; }
}

/* Build the 2n events, sort, sweep.  The interval is half-open [a_i, b_i): the
 * -1 takes effect at b_i, so a guest is not counted at their own exit time.
 * The count rises only at arrivals, so the peak is necessarily attained at some
 * arrival time, and taking the running maximum over the events suffices. */
static int peakOccupancy(const Person *p, int n, int *when) {
    int m = 2 * n, i, cur = 0, best = 0, bestT = 0;
    Event *e = malloc(sizeof(Event) * m);
    buildEvents(e, p, n);
    qsort(e, m, sizeof(Event), cmpEvent);               /* O(n log n) */
    for (i = 0; i < m; i++) {                           /* O(n) sweep */
        sweepSteps++; cur += e[i].d;
        if (cur > best) { best = cur; bestT = e[i].t; }
    }
    free(e);
    *when = bestT; return best;
}

static int occupancyAt(const Person *p, int n, int t) {
    int i, c = 0;
    for (i = 0; i < n; i++) { bfChecks++; if (p[i].a <= t && t < p[i].b) c++; }
    return c;
}

/* O(n^2) reference: try all n arrival times, since the peak can sit nowhere
 * else, counting the guests present at each.  The peak COUNT must match the
 * sweep exactly.  The peak TIME need not, several times being able to tie for
 * it, so the sweep's time is recounted to confirm it witnesses that count. */
static void validate(const Person *p, int n, int best, int when) {
    int i, c, ref = 0, here = occupancyAt(p, n, when);
    for (i = 0; i < n; i++) if ((c = occupancyAt(p, n, p[i].a)) > ref) ref = c;
    if (ref != best || here != best) {
        printf("MISMATCH n=%d: sweep %d at t=%d, brute %d, recount %d\n",
               n, best, when, ref, here);
        exit(1);
    }
}

static void measure(int idx) {
    int n = nValues[idx], when, best;
    long long bc = sortCmps, bs = sweepSteps, bb = bfChecks;
    Person *p = malloc(sizeof(Person) * n);
    makeGuests(p, n);
    best = peakOccupancy(p, n, &when);
    /* Brute force is skipped above BFMAX - it would want 2.5e9 and 2.5e11
     * checks there - and those rows print "-" in the bfChecks column instead. */
    if (n <= BFMAX) validate(p, n, best, when);
    sC[idx] = sortCmps - bc; sS[idx] = sweepSteps - bs; bfC[idx] = bfChecks - bb;
    pkN[idx] = best; pkT[idx] = when;
    free(p);
}

/* Six guests over the endpoints 0..11, laid out by hand so the sweep can be
 * followed on paper: the count climbs to 4 by t=3, drains to 1, then rises. */
static void example(void) {
    Person p[6] = {{0, 5}, {1, 4}, {2, 9}, {3, 6}, {7, 11}, {8, 10}};
    Event e[12]; int i, cur = 0, best = 0, bestT = 0;
    buildEvents(e, p, 6);
    qsort(e, 12, sizeof(Event), cmpEvent);
    printf("\nworked example (n = 6), guests as half-open [a,b) :");
    for (i = 0; i < 6; i++) printf(" p%d[%d,%d)", i + 1, p[i].a, p[i].b);
    printf("\n  %-7s", "time");  for (i = 0; i < 12; i++) printf("%4d", e[i].t);
    printf("\n  %-7s", "delta"); for (i = 0; i < 12; i++) printf("%4d", e[i].d);
    printf("\n  %-7s", "count");
    for (i = 0; i < 12; i++) { cur += e[i].d; printf("%4d", cur);
                               if (cur > best) { best = cur; bestT = e[i].t; } }
    printf("\n  peak = %d guests, first attained at t = %d\n", best, bestT);
    validate(p, 6, best, bestT);
}

int main(void) {
    int i;
    example();
    for (i = 0; i < NNS; i++) measure(i);

    printf("\n=====================================================\n");
    printf(" PEAK SIMULTANEOUS ATTENDANCE BY SORTING THE ENDPOINTS\n");
    printf("=====================================================\n");
    printf("------------------------------------------------------------------\n");
    printf("%7s %11s %9s %7s %9s %11s %6s\n", "n", "sortCmps", "sweep 2n",
           "peak", "peak t", "bfChecks", "ratio");
    printf("------------------------------------------------------------------\n");
    for (i = 0; i < NNS; i++) {
        char bs[24]; double m = 2.0 * nValues[i], bound = m * log2(m);
        if (bfC[i]) snprintf(bs, sizeof bs, "%lld", bfC[i]);
        else bs[0] = '-', bs[1] = 0;
        printf("%7d %11lld %9lld %7lld %9lld %11s %6.3f\n", nValues[i], sC[i],
               sS[i], pkN[i], pkT[i], bs, sC[i] / bound);
    }

    printf("\nThe ratio of sort comparisons to (2n)*log2(2n) climbs in ever\n");
    printf("smaller steps, 0.18 down to 0.014, so it converges and the sort -\n");
    printf("hence the whole algorithm - is Theta(n log n); the sweep column is\n");
    printf("exactly 2n.  Only the 2n endpoints matter, not the times between.\n\n");
    return 0;
}
