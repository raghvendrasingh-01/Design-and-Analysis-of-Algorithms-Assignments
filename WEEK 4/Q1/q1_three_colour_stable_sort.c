/* q1_three_colour_stable_sort.c
 *
 * DAA Q1 : sort number/colour pairs by colour in O(n), numbers still sorted.
 * The input is already sorted by number, so a counting sort on the 3-valued
 * colour key is enough: count the three colours, turn the counts into bucket
 * starts, then copy left to right.  That is 3n item touches and no comparison
 * between numbers at all, hence Theta(n).  Being a stable scan is what keeps
 * the numbers ordered inside a colour; the in-place Dutch-national-flag
 * partition is measured alongside because it is as linear but not stable.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NNS 6

typedef struct { int num; char col; } Item;       /* col in {'R','B','Y'} */

long long cOps = 0, dOps = 0;  /* items read/written: counting sort / flag part. */

static int nValues[NNS] = {12, 100, 1000, 10000, 100000, 1000000};
static long long cntOps[NNS], dnfOps[NNS];
static int dStable[NNS];

static const char ORDER[] = "RBY";           /* the required block order */
static int key(char c) { return c == 'R' ? 0 : c == 'B' ? 1 : 2; }
/* The checker's rank is read off ORDER instead of copying key(), so a wrong
 * convention in the sorter cannot be masked by an identically wrong checker. */
static int rnk(char c) { return (int)(strchr(ORDER, c) - ORDER); }

/* The guarantee the problem hands us: numbers strictly increasing, colours
 * pseudo-random, so no ordering work on the numbers is ever needed. */
static void build(Item *a, int n) {
    int v = 0;
    for (int i = 0; i < n; i++) {
        v += 1 + rand() % 3;                          /* strictly increasing */
        a[i].num = v; a[i].col = ORDER[rand() % 3];
    }
}

/* COUNTING-SORT on key(col).  Three buckets, two passes, n writes.
 * Pass 2 walks the input left to right and only appends within a bucket, so
 * equal-coloured items keep their input order - and the input order was the
 * numeric order.  Stability is therefore the whole proof of correctness. */
static void countingSort(Item *a, int n, Item *out) {
    int cnt[3] = {0, 0, 0}, pos[3];
    for (int i = 0; i < n; i++) { cOps++; cnt[key(a[i].col)]++; }   /* one read */
    pos[0] = 0; pos[1] = cnt[0]; pos[2] = cnt[0] + cnt[1];          /* prefix */
    for (int i = 0; i < n; i++) {
        cOps++;                                                     /* one read */
        int k = key(a[i].col);
        cOps++;                                                     /* one write */
        out[pos[k]++] = a[i];
    }
}

/* DUTCH-NATIONAL-FLAG three-way partition, in place and also linear, but it
 * swaps items across arbitrary distances, so numbers inside a colour end up
 * shuffled.  Kept only to be measured and to fail the stability check. */
static void dutchFlag(Item *a, int n) {
    int lo = 0, mid = 0, hi = n - 1;
    while (mid <= hi) {
        dOps++;                                                     /* one read */
        int k = key(a[mid].col);
        Item t = a[mid];
        if (k == 0)      { a[mid++] = a[lo]; a[lo++] = t; dOps += 2; } /* 2 writes */
        else if (k == 1) { mid++; }
        else             { a[mid] = a[hi];   a[hi--] = t; dOps += 2; }
    }
}

/* (a) colours grouped R then B then Y, ranked off ORDER not the sorter's key. */
static int grouped(Item *a, int n) {
    for (int i = 1; i < n; i++) if (rnk(a[i].col) < rnk(a[i-1].col)) return 0;
    return 1;
}

/* (b) numbers strictly ascending within every colour block.  Given (a) the
 * blocks are contiguous, so adjacent same-colour pairs cover them all. */
static int ordered(Item *a, int n) {
    for (int i = 1; i < n; i++)
        if (a[i].col == a[i-1].col && a[i].num <= a[i-1].num) return 0;
    return 1;
}

/* Per-colour (count, sum, xor) of the numbers.  Identical fingerprints for
 * input and output mean nothing was invented, lost or recoloured. */
static void fingerprint(Item *a, int n, long long fp[3][3]) {
    memset(fp, 0, 9 * sizeof(long long));
    for (int i = 0; i < n; i++) {
        int c = rnk(a[i].col);
        fp[c][0]++; fp[c][1] += a[i].num; fp[c][2] ^= a[i].num;
    }
}

static void show(const char *title, Item *a, int n) {
    printf("%-26s", title);
    for (int i = 0; i < n; i++) printf(" %d%c", a[i].num, a[i].col);
    printf("\n");
}

static void measure(int idx) {
    int n = nValues[idx];
    long long fin[3][3], fout[3][3], before;
    Item *a   = (Item *)malloc(sizeof(Item) * n);
    Item *out = (Item *)malloc(sizeof(Item) * n);
    Item *d   = (Item *)malloc(sizeof(Item) * n);
    build(a, n);
    memcpy(d, a, sizeof(Item) * n);
    before = cOps; countingSort(a, n, out); cntOps[idx] = cOps - before;
    before = dOps; dutchFlag(d, n);         dnfOps[idx] = dOps - before;
    dStable[idx] = ordered(d, n);
    fingerprint(a, n, fin);
    /* The counts mean nothing unless the output is right: counting sort must
     * group, stay ordered and be a permutation; the flag sort must group and
     * permute but, from n = 100 up where luck is not an explanation, must NOT
     * come out ordered - that broken property (b) is the point of the question,
     * so it is asserted here and not merely printed. */
    fingerprint(out, n, fout);
    if (!grouped(out, n) || !ordered(out, n) || memcmp(fin, fout, sizeof fin)) {
        printf("MISMATCH counting sort at n=%d\n", n); exit(1);
    }
    fingerprint(d, n, fout);
    if (!grouped(d, n) || memcmp(fin, fout, sizeof fin) ||
        (n >= 100 && dStable[idx])) {
        printf("MISMATCH flag partition at n=%d (group, permute, not stable)\n", n);
        exit(1);
    }
    if (idx == 0) {
        printf("\nThe n = %d instance in full:\n", n);
        show("input (sorted by num)", a, n);
        show("counting sort (stable)", out, n);
        show("dutch flag (unstable)", d, n);
    }
    free(a); free(out); free(d);
}

int main(void) {
    int i;
    for (i = 0; i < NNS; i++) measure(i);

    printf("\n=====================================================\n");
    printf(" THREE-COLOUR SORT: LINEAR IS EASY, STABLE IS THE POINT\n");
    printf("=====================================================\n");
    printf("---------------------------------------------------------------"
           "-----------\n");
    printf("%9s %11s %11s %11s %7s %9s %9s\n", "n", "count ops", "3n",
           "flag ops", "ops/n", "cs stable", "dnf stable");
    printf("---------------------------------------------------------------"
           "-----------\n");
    for (i = 0; i < NNS; i++) {
        int n = nValues[i];
        printf("%9d %11lld %11d %11lld %7.2f %9s %9s\n",
               n, cntOps[i], 3 * n, dnfOps[i], (double)cntOps[i] / n,
               "PASS", dStable[i] ? "PASS" : "FAIL");   /* cs: else it exited */
    }

    printf("\nThe count column equals 3n exactly and ops/n is flat, so the sort\n");
    printf("is Theta(n); a comparison sort would cost Omega(n log n) for nothing\n");
    printf("on a 3-valued key.  Both algorithms are linear, but only the stable\n");
    printf("scan keeps the numbers ascending inside a colour - linearity alone\n");
    printf("was never the difficulty.\n\n");
    return 0;
}
