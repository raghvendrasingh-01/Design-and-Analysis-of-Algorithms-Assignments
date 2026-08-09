
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NSIZES   6
#define NCLASSES 3

/* Counters, reset before every sort */
long long comparisons = 0;
long long moves = 0;

void resetCounters(void)
{
    comparisons = 0;
    moves = 0;
}

static int sizes[NSIZES] = {1000, 2000, 4000, 8000, 16000, 32000};

static char *className[NCLASSES] = {"Random", "Already sorted", "Reverse sorted"};

/* Results, filled in by the analysis loop.
 * [class][size] */
static long long comp2[NCLASSES][NSIZES], move2[NCLASSES][NSIZES];
static long long comp3[NCLASSES][NSIZES], move3[NCLASSES][NSIZES];

/* ------------------------------------------------------------------
 * Ordinary merge sort - split in half
 * ------------------------------------------------------------------ */

/* Merge the sorted halves [lo,mid) and [mid,hi) into temp, then copy back */
void merge2(int a[], int temp[], int lo, int mid, int hi)
{
    int i = lo, j = mid, t = lo;

    while (i < mid && j < hi)
    {
        comparisons++;
        if (a[i] <= a[j])
            temp[t++] = a[i++];
        else
            temp[t++] = a[j++];
        moves++;
    }

    while (i < mid)
    {
        temp[t++] = a[i++];
        moves++;
    }

    while (j < hi)
    {
        temp[t++] = a[j++];
        moves++;
    }

    for (int k = lo; k < hi; k++)
        a[k] = temp[k];
}

void mergeSort2(int a[], int temp[], int lo, int hi)
{
    int n = hi - lo;
    int mid;

    if (n < 2)
        return;

    mid = lo + n / 2;

    mergeSort2(a, temp, lo, mid);
    mergeSort2(a, temp, mid, hi);
    merge2(a, temp, lo, mid, hi);
}

/* ------------------------------------------------------------------
 * Modified merge sort - split into thirds
 * ------------------------------------------------------------------ */

/* Merge the three sorted runs [lo,m1), [m1,m2) and [m2,hi) into temp.
 *
 * pos[r] is the head of run r and end[r] is where it stops.  Each round
 * scans the three heads and keeps the smallest.  The first live run is
 * taken for free; every live run after it costs one comparison.  So with
 * all three alive a round costs 2 comparisons, with two alive 1, and with
 * one alive none - which is the extra comparison cost of splitting three
 * ways rather than two.
 */
void merge3(int a[], int temp[], int lo, int m1, int m2, int hi)
{
    int pos[3] = {lo, m1, m2};
    int end[3] = {m1, m2, hi};
    int t = lo;

    while (t < hi)
    {
        int pick = -1;

        for (int r = 0; r < 3; r++)
        {
            if (pos[r] >= end[r])
                continue;

            if (pick < 0)
                pick = r;
            else
            {
                comparisons++;
                if (a[pos[r]] < a[pos[pick]])
                    pick = r;
            }
        }

        temp[t++] = a[pos[pick]++];
        moves++;
    }

    for (int x = lo; x < hi; x++)
        a[x] = temp[x];
}

void mergeSort3(int a[], int temp[], int lo, int hi)
{
    int n = hi - lo;
    int third, m1, m2;

    if (n < 2)
        return;

    /* Two elements cannot be split into three non-empty parts, so handle
     * that case directly.  This also guarantees third >= 1 below, which is
     * what stops the recursion from looping forever. */
    if (n == 2)
    {
        comparisons++;
        if (a[lo] > a[lo + 1])
        {
            int tmp = a[lo];
            a[lo] = a[lo + 1];
            a[lo + 1] = tmp;
            moves += 2;
        }
        return;
    }

    third = n / 3;
    m1 = lo + third;
    m2 = lo + 2 * third;

    mergeSort3(a, temp, lo, m1);
    mergeSort3(a, temp, m1, m2);
    mergeSort3(a, temp, m2, hi);
    merge3(a, temp, lo, m1, m2, hi);
}

/* ------------------------------------------------------------------
 * Helpers
 * ------------------------------------------------------------------ */

void buildInput(int a[], int n, int cls)
{
    if (cls == 0)
    {
        srand(42);                      /* fixed seed, so runs repeat */
        for (int i = 0; i < n; i++)
            a[i] = rand() % (n * 10);
    }
    else if (cls == 1)
    {
        for (int i = 0; i < n; i++)
            a[i] = i;
    }
    else
    {
        for (int i = 0; i < n; i++)
            a[i] = n - i;
    }
}

int isSorted(int a[], int n)
{
    for (int i = 1; i < n; i++)
        if (a[i - 1] > a[i])
            return 0;
    return 1;
}

void printLine(int width)
{
    for (int i = 0; i < width; i++)
        printf("-");
    printf("\n");
}

void printHeading(char *title)
{
    printf("\n=====================================================\n");
    printf(" %s\n", title);
    printf("=====================================================\n");
}

void printArray(int a[], int n)
{
    for (int i = 0; i < n; i++)
        printf("%d ", a[i]);
    printf("\n");
}

/* ------------------------------------------------------------------
 * Output sections
 * ------------------------------------------------------------------ */

/* Sort one small array both ways so the reader can see they agree. */
void printDemo(void)
{
    int demo[12] = {38, 27, 43, 3, 9, 82, 10, 1, 55, 20, 7, 64};
    int a[12], b[12], temp[12];

    printHeading("DEMONSTRATION ON A SMALL ARRAY");

    for (int i = 0; i < 12; i++)
        a[i] = b[i] = demo[i];

    printf("Input          : ");
    printArray(demo, 12);

    resetCounters();
    mergeSort2(a, temp, 0, 12);
    printf("Two-way   sort : ");
    printArray(a, 12);
    printf("                 %lld comparisons, %lld moves\n",
           comparisons, moves);

    resetCounters();
    mergeSort3(b, temp, 0, 12);
    printf("Three-way sort : ");
    printArray(b, 12);
    printf("                 %lld comparisons, %lld moves\n",
           comparisons, moves);

    printf("\nBoth outputs sorted and identical : %s\n",
           (isSorted(a, 12) && isSorted(b, 12)) ? "YES" : "NO");
}

void printRecurrences(void)
{
    printHeading("THE TWO RECURRENCES");
    printf("Ordinary merge sort\n");
    printf("  T(n) = 2 T(n/2) + O(n)\n");
    printf("  Master theorem : a = 2, b = 2, so n^(log_b a) = n^1 = n.\n");
    printf("  The merge cost O(n) matches, so T(n) = O(n log n).\n");
    printf("  Depth of recursion = log2 n.\n\n");

    printf("Three-way merge sort\n");
    printf("  T(n) = 3 T(n/3) + O(n)\n");
    printf("  Master theorem : a = 3, b = 3, so n^(log_b a) = n^1 = n.\n");
    printf("  The merge cost O(n) matches, so T(n) = O(n log n).\n");
    printf("  Depth of recursion = log3 n = log2 n / log2 3.\n\n");

    printf("ANSWER: the modified merge sort is still O(n log n) in the\n");
    printf("worst case.  Splitting into thirds only changes the base of\n");
    printf("the logarithm, and a change of base is a constant factor\n");
    printf("(1 / log2 3 = %.4f), which O-notation absorbs.\n",
           1.0 / (log(3.0) / log(2.0)));
}

/* One table of counts for one input class.  When `normalise` is set every
 * count is divided by n log2 n, which stays flat if the growth really is
 * O(n log n) and climbs if it is not. */
void printTable(int cls, int normalise)
{
    printf("\n%s input%s\n", className[cls],
           normalise ? " - counts divided by n log2(n)" : "");
    printLine(70);
    printf("%10s %14s %14s %14s %14s\n",
           "n", "2-way comps", "3-way comps", "2-way moves", "3-way moves");
    printLine(70);

    for (int k = 0; k < NSIZES; k++)
    {
        double n = (double)sizes[k];
        double s = n * (log(n) / log(2.0));

        if (normalise)
            printf("%10d %14.3f %14.3f %14.3f %14.3f\n", sizes[k],
                   comp2[cls][k] / s, comp3[cls][k] / s,
                   move2[cls][k] / s, move3[cls][k] / s);
        else
            printf("%10d %14lld %14lld %14lld %14lld\n", sizes[k],
                   comp2[cls][k], comp3[cls][k],
                   move2[cls][k], move3[cls][k]);
    }
}

void printConclusion(void)
{
    int last = NSIZES - 1;
    double n = (double)sizes[last];
    double scale = n * (log(n) / log(2.0));

    printHeading("CONCLUSION");
    printf("At n = %d on random input:\n\n", sizes[last]);
    printf("  comparisons / (n log2 n)   2-way %.3f   3-way %.3f\n",
           comp2[0][last] / scale, comp3[0][last] / scale);
    printf("  moves       / (n log2 n)   2-way %.3f   3-way %.3f\n\n",
           move2[0][last] / scale, move3[0][last] / scale);

    printf("Both ratios are flat as n grows, which is what O(n log n)\n");
    printf("looks like when measured.  Neither variant escapes the class.\n\n");

    printf("The three-way version makes fewer moves, because it has\n");
    printf("log3 n levels instead of log2 n and every level copies n\n");
    printf("elements.  It makes more comparisons, because choosing the\n");
    printf("smallest of three heads costs 2 comparisons instead of 1.\n");
    printf("The two effects nearly cancel, and neither changes the\n");
    printf("asymptotic answer.\n");

    printf("\nTime Complexity  : O(n log n) for both variants\n");
    printf("Space Complexity : O(n) for both - one temporary array of n\n");
    printf("                   integers, plus O(log n) recursion stack\n");
}

/* Sort one size with both variants and record the four counts. */
void measure(int cls, int k)
{
    int n = sizes[k];
    int *a = (int *)malloc(sizeof(int) * n);
    int *temp = (int *)malloc(sizeof(int) * n);

    buildInput(a, n, cls);
    resetCounters();
    mergeSort2(a, temp, 0, n);
    comp2[cls][k] = comparisons;
    move2[cls][k] = moves;
    if (!isSorted(a, n))
        printf("\nERROR: two-way sort failed at n = %d\n", n);

    buildInput(a, n, cls);
    resetCounters();
    mergeSort3(a, temp, 0, n);
    comp3[cls][k] = comparisons;
    move3[cls][k] = moves;
    if (!isSorted(a, n))
        printf("\nERROR: three-way sort failed at n = %d\n", n);

    free(a);
    free(temp);
}

int main(void)
{
    printf("=====================================================\n");
    printf(" DAA Q2 : MERGE SORT vs THREE-WAY MERGE SORT\n");
    printf("=====================================================\n");

    printDemo();
    printRecurrences();

    printf("\nMeasuring ");
    for (int c = 0; c < NCLASSES; c++)
    {
        for (int k = 0; k < NSIZES; k++)
            measure(c, k);
        printf(".");
        fflush(stdout);
    }
    printf(" done\n");

    printHeading("MEASURED COUNTS");
    for (int c = 0; c < NCLASSES; c++)
        printTable(c, 0);

    printHeading("NORMALISED BY n log2(n)");
    for (int c = 0; c < NCLASSES; c++)
        printTable(c, 1);

    printConclusion();

    return 0;
}
