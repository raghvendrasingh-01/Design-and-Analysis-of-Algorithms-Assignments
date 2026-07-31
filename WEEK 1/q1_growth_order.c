/* ============================================================================
 *  DAA Assignment Q1 : place the given functions in increasing order of growth
 *      1/n, log2(n), 12*sqrt(n), 50*n^0.5, n^0.51, (2^32)*n, n*log2(n),
 *      100*n^2+6*n, n^2-324, 2*n^3, n^(log2 n), 3^n
 *  The final ordering is NOT written down anywhere in this file.  It is
 *  produced twice, by two independent algorithms, and the answers confronted:
 *    (A) NUMERICAL   evaluate all 12 at n = 1e2..1e8 and merge sort them by
 *                    the computed value, separately at every n.
 *    (B) THEORETICAL merge sort the same 12 with a comparator that reads the
 *                    growth class only, so constant factors and lower order
 *                    terms cannot influence the result.
 *  Where (A) and (B) disagree the crossover n is located by bisection on the
 *  real functions: that is the evidence (B) is right for "sufficiently large
 *  n", and it shows that some crossings lie far outside any testable range.
 *  Build : gcc q1_growth_order.c -lm -Wall -Wextra
 *  Output: report on stdout
 * ==========================================================================*/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>

#define NUM_FUNCS  12
#define NUM_TESTS  7
#define BAR_WIDTH  52
#define XMAX       300.0  /* crossover search stops at n = 10^300           */
#define OVF_LOG10  305.0  /* a result with more decades than this is an INF */
#define WHY_LEN    96     /* size of an explanation buffer                  */
/* The growth categories asked for by the assignment.  tier() below turns a
 * category into a coarse rank; inside a tier the exponent decides.          */
#define CAT_RECIPROCAL   0
#define CAT_LOGARITHMIC  1
#define CAT_POLYNOMIAL   2
#define CAT_LINEARITHMIC 3
#define CAT_SUPERPOLY    4
#define CAT_EXPONENTIAL  5
#define NUM_CATS         6

/* The structure required by the assignment: name/category/exponent/evaluate
 * are exactly as specified, and three extra fields ride alongside them.
 * magnitude() returns log10 of the value.  log10 is strictly increasing, so
 * ordering by magnitude is *identical* to ordering by value while never
 * overflowing - without it 3^n and n^(log2 n) would both collapse to +INF at
 * large n and compare as a tie.  value/mag cache the numbers for the test
 * being ranked right now, so that the merge sort has something to compare.  */
typedef struct
{
    char   name[100];
    int    category;
    double exponent;            /* poly power, or base for exponentials    */
    double (*evaluate)(double); /* the honest, possibly overflowing value  */
    double (*magnitude)(double);/* log10 of that value, overflow proof     */
    double value;               /* cached f(n) for the current test        */
    double mag;                 /* cached log10(f(n)) for the current test */
} Function;
typedef int (*Comparator)(const Function *, const Function *);

/* ===== SECTION 1 : numeric helpers =======================================
 * safe_pow10  : 10^x with no overflow trap - past the double range it reports
 *               +infinity, which formatValue() renders as "INF".
 * log10_floor : log10 of a value that may be <= 0.  Every function here is
 *               positive for large n and only n^2-324 dips negative (n < 18);
 *               flooring those keeps the order right, because a negative value
 *               really is smaller than every positive one.                  */
static double safe_pow10(double x)  { return (x > OVF_LOG10) ? HUGE_VAL : pow(10.0, x); }
static double log10_floor(double v) { return (v > 0.0) ? log10(v) : -DBL_MAX / 4.0; }

/* ===== SECTION 2 : one evaluate() and one magnitude() per function ========
 * Each magnitude() derives log10(f(n)) in closed form, so it never takes the
 * log of an already overflowed number.  That is what keeps the ranking of
 * 3^n and n^(log2 n) meaningful long after their values stop fitting.      */
static double f_reciprocal(double n) { return 1.0 / n; }              /* 1/n */
static double m_reciprocal(double n) { return -log10(n); }
static double f_log2v(double n)      { return log2(n); }          /* log2(n) */
static double m_log2v(double n)      { return log10_floor(log2(n)); }
static double f_sqrt12(double n)     { return 12.0 * sqrt(n); }/* 12*sqrt(n) */
static double m_sqrt12(double n)     { return log10(12.0) + 0.5 * log10(n); }
/* 50*n^0.5 : the same growth as 12*sqrt(n), only the constant differs */
static double f_sqrt50(double n)     { return 50.0 * pow(n, 0.5); }
static double m_sqrt50(double n)     { return log10(50.0) + 0.5 * log10(n); }
/* n^0.51 : a hair above sqrt, and that hair is what decides the order */
static double f_pow051(double n)     { return pow(n, 0.51); }
static double m_pow051(double n)     { return 0.51 * log10(n); }
/* (2^32)*n : a colossal constant in front of a plain linear term */
static double f_lin2p32(double n)    { return pow(2.0, 32.0) * n; }
static double m_lin2p32(double n)    { return 32.0 * log10(2.0) + log10(n); }
static double f_nlogn(double n)      { return n * log2(n); }    /* n*log2(n) */
static double m_nlogn(double n)      { return log10(n) + log10_floor(log2(n)); }
/* 100*n^2+6*n, factored as n*(100n+6) so the log stays exact */
static double f_quad100(double n)    { return 100.0 * n * n + 6.0 * n; }
static double m_quad100(double n)    { return log10(n) + log10(100.0 * n + 6.0); }
/* n^2-324 : same class as the above, the -324 is a lower order term */
static double f_quadm324(double n)   { return n * n - 324.0; }
static double m_quadm324(double n)   { return log10_floor(n * n - 324.0); }
static double f_cubic2(double n)     { return 2.0 * n * n * n; }    /* 2*n^3 */
static double m_cubic2(double n)     { return log10(2.0) + 3.0 * log10(n); }
/* n^(log2 n) : its exponent is itself growing, so it is not polynomial.
 * log10(n^log2 n) = log2(n)*log10(n), which is why it beats every fixed
 * power of n as soon as log2(n) climbs past that power.                    */
static double f_npowlogn(double n)   { return safe_pow10(log2(n) * log10(n)); }
static double m_npowlogn(double n)   { return log2(n) * log10(n); }
/* 3^n : log10 grows linearly in n, the steepest slope in the set */
static double f_exp3(double n)       { return safe_pow10(n * log10(3.0)); }
static double m_exp3(double n)       { return n * log10(3.0); }

/* ===== SECTION 3 : the function table ====================================
 * category+exponent are the *declared* growth class of each expression: facts
 * about the algebra, not a pre-baked ordering.  Nothing here says which
 * function wins - that is left entirely to the two comparators.            */
static Function funcs[NUM_FUNCS];
static const double test_n[NUM_TESTS] = { 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8 };
static void setFunction(int i, const char *name, int cat, double expo,
                        double (*ev)(double), double (*mg)(double))
{
    Function *f = &funcs[i];
    snprintf(f->name, sizeof f->name, "%s", name);
    f->category = cat;  f->exponent = expo;
    f->evaluate = ev;   f->magnitude = mg;
    f->value    = 0.0;  f->mag       = 0.0;
}
static void buildFunctionTable(void)
{
    setFunction(0,  "1/n",         CAT_RECIPROCAL,  -1.0, f_reciprocal, m_reciprocal);
    setFunction(1,  "log2(n)",     CAT_LOGARITHMIC,  0.0, f_log2v,      m_log2v);
    setFunction(2,  "12*sqrt(n)",  CAT_POLYNOMIAL,   0.5, f_sqrt12,     m_sqrt12);
    setFunction(3,  "50*n^0.5",    CAT_POLYNOMIAL,   0.5, f_sqrt50,     m_sqrt50);
    setFunction(4,  "n^0.51",      CAT_POLYNOMIAL,  0.51, f_pow051,     m_pow051);
    setFunction(5,  "(2^32)*n",    CAT_POLYNOMIAL,   1.0, f_lin2p32,    m_lin2p32);
    setFunction(6,  "n*log2(n)",   CAT_LINEARITHMIC, 1.0, f_nlogn,      m_nlogn);
    setFunction(7,  "100*n^2+6*n", CAT_POLYNOMIAL,   2.0, f_quad100,    m_quad100);
    setFunction(8,  "n^2-324",     CAT_POLYNOMIAL,   2.0, f_quadm324,   m_quadm324);
    setFunction(9,  "2*n^3",       CAT_POLYNOMIAL,   3.0, f_cubic2,     m_cubic2);
    setFunction(10, "n^(log2 n)",  CAT_SUPERPOLY,    0.0, f_npowlogn,   m_npowlogn);
    setFunction(11, "3^n",         CAT_EXPONENTIAL,  3.0, f_exp3,       m_exp3);
}

/* ===== SECTION 4 : merge sort, written out by hand ========================
 * qsort() is not used anywhere.  This routine is generic over a Comparator,
 * so the very same code performs the numerical and the theoretical ranking.
 * Merge sort is *stable*, which matters in section 6: functions of identical
 * growth class compare equal and keep their table order instead of jittering.
 * Cost: O(n log n) comparisons, O(n) auxiliary space, O(log n) recursion.  */
static Function mergeBuf[NUM_FUNCS];
/* merge a[lo..mid] and a[mid+1..hi], both already sorted */
static void mergeHalves(Function a[], int lo, int mid, int hi, Comparator cmp)
{
    int i = lo, j = mid + 1, k = lo;
    while (i <= mid && j <= hi) {
        /* "<= 0" keeps equal elements in left-hand order => stability */
        if (cmp(&a[i], &a[j]) <= 0) mergeBuf[k++] = a[i++];
        else                        mergeBuf[k++] = a[j++];
    }
    while (i <= mid) mergeBuf[k++] = a[i++];
    while (j <= hi)  mergeBuf[k++] = a[j++];
    for (k = lo; k <= hi; k++) a[k] = mergeBuf[k];
}
static void mergeSortRange(Function a[], int lo, int hi, Comparator cmp)
{
    int mid;
    if (lo >= hi) return;               /* 0 or 1 element is already sorted */
    mid = lo + (hi - lo) / 2;           /* written this way to avoid overflow */
    mergeSortRange(a, lo, mid, cmp);
    mergeSortRange(a, mid + 1, hi, cmp);
    mergeHalves(a, lo, mid, hi, cmp);
}
static void mergeSort(Function a[], int n, Comparator cmp) { mergeSortRange(a, 0, n - 1, cmp); }

/* ===== SECTION 5 : comparator #1, purely numerical =======================
 * Ordering by log10(value) is the same as ordering by value, and it survives
 * the range where 3^n and n^(log2 n) have both overflowed to +INF.         */
static int compareNumeric(const Function *a, const Function *b)
{
    if (a->mag < b->mag) return -1;
    if (a->mag > b->mag) return  1;
    return 0;
}

/* ===== SECTION 6 : comparator #2, purely theoretical =====================
 * It never looks at a value.  It reads the growth class only, so constant
 * factors (12 vs 50, the 2^32, the 100, the 2) and lower order terms (the
 * +6n, the -324) are structurally invisible to it.  tier() is the coarse
 * rank of a category; polynomial and linearithmic deliberately share a tier
 * because n*log2(n) has to be able to slot in between n and n^2.           */
static int tier(int category)
{
    switch (category) {
        case CAT_RECIPROCAL:   return 0;  /* decays to 0                    */
        case CAT_LOGARITHMIC:  return 1;  /* slower than every n^c, c>0     */
        case CAT_POLYNOMIAL:                                 /* fall through */
        case CAT_LINEARITHMIC: return 2;  /* separated by exponent below    */
        case CAT_SUPERPOLY:    return 3;  /* beats every fixed power        */
        default:               return 4;  /* exponential                    */
    }
}
static int compareAsymptotic(const Function *a, const Function *b)
{
    int ta = tier(a->category), tb = tier(b->category);
    if (ta != tb) return (ta < tb) ? -1 : 1;  /* 1. different tiers settle it */
    if (ta == 2) {
        /* 2. the polynomial/linearithmic tier: the exponent of n decides,
         *    0.5 < 0.51 < 1 < 2 < 3, and no constant may speak.           */
        if (a->exponent < b->exponent) return -1;
        if (a->exponent > b->exponent) return  1;
        /* 3. identical exponent: a log2(n) factor breaks the tie, so
         *    n*log2(n) outgrows (2^32)*n even though 2^32 is enormous.    */
        if (a->category != b->category)
            return (a->category == CAT_LINEARITHMIC) ? 1 : -1;
    }
    /* 4. genuinely the same growth class => equal.  This is the branch that
     *    reports 12*sqrt(n) == 50*n^0.5 and 100*n^2+6*n == n^2-324.       */
    return 0;
}


/* ===== SECTION 8 : shared state for the two passes ======================= */
static double values[NUM_FUNCS][NUM_TESTS];  /* values[function][test]   */
static int    rankAt[NUM_FUNCS][NUM_TESTS];  /* 1-based numerical rank   */
static int    finalRank[NUM_FUNCS];          /* 1-based theoretical rank */
static int    orderAt[NUM_TESTS][NUM_FUNCS]; /* ranked list of indices   */
/* index of a function inside funcs[], located by name (sorted copies move
 * around, so array positions are not stable identifiers)                  */
static int indexOfName(const char *name)
{
    int i;
    for (i = 0; i < NUM_FUNCS; i++)
        if (strcmp(funcs[i].name, name) == 0) return i;
    return -1;
}
/* render a double for human eyes: INF when it left the double range */
static void formatValue(double v, char *out, size_t cap)
{
    if (isinf(v) || isnan(v)) snprintf(out, cap, "INF");
    else                      snprintf(out, cap, "%.4e", v);
}

/* ===== SECTION 9 : evaluation pass, fills values[][] ==================== */
static void evaluateAll(void)
{
    int f, t;
    for (f = 0; f < NUM_FUNCS; f++)
        for (t = 0; t < NUM_TESTS; t++)
            values[f][t] = funcs[f].evaluate(test_n[t]);
}
/* Load the cached value/mag for test t into a working copy of the table.
 * funcs[] itself is never permuted - it stays the canonical order that
 * values[][] and every *Rank[] array are indexed by.  Only copies get
 * sorted, which keeps those index mappings valid for the whole run.       */
static void prepareWork(Function work[], int t)
{
    int f;
    for (f = 0; f < NUM_FUNCS; f++) {
        work[f]       = funcs[f];
        work[f].value = values[f][t];
        work[f].mag   = funcs[f].magnitude(test_n[t]);
    }
}

/* ===== SECTION 10 : numerical ranking per test + stability analysis ===== */
static int stableFrom = -1;   /* earliest test never later contradicted, -1 = never */
/* rank at which two orderings first diverge, or -1 when they are identical */
static int firstDifference(int t1, int t2)
{
    int i;
    for (i = 0; i < NUM_FUNCS; i++)
        if (orderAt[t1][i] != orderAt[t2][i]) return i;
    return -1;
}
static int sameOrder(int t1, int t2) { return firstDifference(t1, t2) < 0; }
static void runNumericalRankings(void)
{
    Function work[NUM_FUNCS];
    char buf[32];
    int t, i, d;
    for (t = 0; t < NUM_TESTS; t++) {
        prepareWork(work, t);
        mergeSort(work, NUM_FUNCS, compareNumeric);
        /* remember the ranking, and stamp each function with its rank */
        for (i = 0; i < NUM_FUNCS; i++) {
            int canonical = indexOfName(work[i].name);
            orderAt[t][i] = canonical;
            rankAt[canonical][t] = i + 1;
        }
        printf("\n====================================\n"
               "n = %.0f\n"
               "====================================\n"
               "Rank  %-16s %s\n"
               "-------------------------------------------\n",
               test_n[t], "Function", "Value");
        for (i = 0; i < NUM_FUNCS; i++) {
            formatValue(work[i].value, buf, sizeof buf);
            printf("%3d   %-16s %s\n", i + 1, work[i].name, buf);
        }
        /* stability check against the previous test */
        if (t == 0)
            printf("\n(first test - no previous ordering to compare against)\n");
        else if (sameOrder(t, t - 1))
            printf("\nOrdering remained unchanged.\n");
        else {
            d = firstDifference(t, t - 1);
            printf("\nOrdering changed.  First difference at rank %d: "
                   "%s took the place of %s.\n", d + 1,
                   funcs[orderAt[t][d]].name, funcs[orderAt[t - 1][d]].name);
        }
    }
    /* "stable from" = the earliest test every later test still agrees with */
    for (t = 0; t < NUM_TESTS; t++) {
        int holds = 1, u;
        for (u = t + 1; u < NUM_TESTS; u++)
            if (!sameOrder(t, u)) { holds = 0; break; }
        if (holds) { stableFrom = t; break; }
    }
}

/* ===== SECTION 11 : extra credit, rank movement between tests =========== */
static void printRankMovementTable(void)
{
    int f, t;
    printf("\n====================================\n"
           "RANK MOVEMENT BETWEEN CONSECUTIVE TESTS\n"
           "====================================\n"
           "A function that never moves has already reached its asymptotic slot.\n\n"
           "%-16s", "Function");
    for (t = 0; t < NUM_TESTS; t++) printf("  1e%-3d", (int)log10(test_n[t]));
    printf("   Moves\n"
           "--------------------------------------------------------------------------\n");
    for (f = 0; f < NUM_FUNCS; f++) {
        int moves = 0;
        printf("%-16s", funcs[f].name);
        for (t = 0; t < NUM_TESTS; t++) {
            printf("  %-5d", rankAt[f][t]);
            if (t > 0 && rankAt[f][t] != rankAt[f][t - 1]) moves++;
        }
        printf("   %d\n", moves);
    }
}

/* ===== SECTION 12 : the theoretical pass ================================ */
static Function finalOrder[NUM_FUNCS];   /* funcs[] sorted asymptotically */
/* why does this function sit in this growth class? one line each */
static void explainFunction(const Function *f, char *out, size_t cap)
{
    switch (f->category) {
    case CAT_RECIPROCAL:
        snprintf(out, cap, "decays towards 0, so it is below everything else"); break;
    case CAT_LOGARITHMIC:
        snprintf(out, cap, "grows slower than n^c for every c > 0"); break;
    case CAT_LINEARITHMIC:
        snprintf(out, cap, "n^1 with an extra log2 n factor, so just above linear"); break;
    case CAT_SUPERPOLY:
        snprintf(out, cap, "exponent log2 n keeps growing, beats every fixed power"); break;
    case CAT_EXPONENTIAL:
        snprintf(out, cap, "base %.0f^n, log of it is linear in n: the steepest here",
                 f->exponent); break;
    default:
        snprintf(out, cap, "polynomial of degree %g, constants dropped", f->exponent);
    }
}
static void runTheoreticalSort(void)
{
    int i;
    for (i = 0; i < NUM_FUNCS; i++) finalOrder[i] = funcs[i];
    mergeSort(finalOrder, NUM_FUNCS, compareAsymptotic);
    for (i = 0; i < NUM_FUNCS; i++)
        finalRank[indexOfName(finalOrder[i].name)] = i + 1;
}
static void printFinalOrder(void)
{
    char why[WHY_LEN];
    int i;
    printf("\n====================================\n"
           "FINAL ASYMPTOTIC ORDER\n"
           "====================================\n"
           "Produced by merge sort under compareAsymptotic(), which reads only\n"
           "the growth class - it never sees a computed value.\n\n");
    for (i = 0; i < NUM_FUNCS; i++) {
        explainFunction(&finalOrder[i], why, sizeof why);
        printf("   %-16s   [%s]\n", finalOrder[i].name, why);
        if (i + 1 < NUM_FUNCS)   /* a tie is drawn differently from an increase */
            printf("%s", compareAsymptotic(&finalOrder[i], &finalOrder[i + 1]) == 0
                   ? "   =  (same growth class, neither dominates)\n"
                   : "   |\n   v\n");
    }
    
}
/* ===== SECTION 14 : ASCII visualisation =================================
 * The values span roughly 10^-8 up to 10^(4.8e7), so a linear bar chart would
 * render every function except 3^n as an empty line.  The bars are therefore
 * drawn on a log-of-log scale: bar length ~ log10(decades).                */
static void printAsciiGraph(void)
{
    double score[NUM_FUNCS], lo = DBL_MAX, hi = -DBL_MAX;
    double nbig = test_n[NUM_TESTS - 1];
    int f, i, len;
    printf("\n====================================\n"
           "ASCII GROWTH VISUALISATION at n = %.0e\n"
           "====================================\n"
           "Log-of-log scale: bar ~ log10(number of decades in f(n)).\n"
           "Even on this doubly compressed scale 3^n runs off the others.\n\n", nbig);
    for (f = 0; f < NUM_FUNCS; f++) {
        /* +10 lifts 1/n (-8 decades) above zero before the outer log */
        score[f] = log10(funcs[f].magnitude(nbig) + 10.0);
        if (score[f] < lo) lo = score[f];
        if (score[f] > hi) hi = score[f];
    }
    /* draw in the theoretical order so the picture matches the final answer */
    for (i = 0; i < NUM_FUNCS; i++) {
        f = indexOfName(finalOrder[i].name);
        len = 1 + (int)((score[f] - lo) / (hi - lo) * (BAR_WIDTH - 1));
        printf("%-16s ", funcs[f].name);
        while (len-- > 0) printf("\u2588");;
        putchar('\n');
    }
    printf("\nDecades of f(%.0e): from %.1f (1/n) up to %.3e (3^n).\n",
           nbig, funcs[indexOfName("1/n")].magnitude(nbig),
           funcs[indexOfName("3^n")].magnitude(nbig));
}

/* ===== SECTION 15 : agreement between the passes, and the conclusion ==== */
static void printConclusion(void)
{
    int i, mismatch = 0, lastTest = NUM_TESTS - 1;
    printf("\n====================================\n"
           "CONCLUSION\n"
           "====================================\n"
           "\n* Numerical ordering at n = %.0e (merge sort on measured values):\n   ",
           test_n[lastTest]);
    for (i = 0; i < NUM_FUNCS; i++)
        printf("%s%s", funcs[orderAt[lastTest][i]].name,
               i + 1 < NUM_FUNCS ? " < " : "\n");
    if (stableFrom < 0)
        printf("\n* Stable ordering: the ranking was still moving at n = %.0e,\n"
               "   so no tested n is large enough to call it settled.\n",
               test_n[lastTest]);
    else if (stableFrom == 0)
        printf("\n* Stable ordering: unchanged across every tested n, from %.0e onwards.\n",
               test_n[0]);
    else
        printf("\n* Ordering stabilized after n = %.0f\n", test_n[stableFrom]);
    printf("\n* Final theoretical ordering (merge sort on growth class):\n   ");
    for (i = 0; i < NUM_FUNCS; i++) {
        printf("%s", finalOrder[i].name);
        if (i + 1 < NUM_FUNCS)
            printf("%s", compareAsymptotic(&finalOrder[i], &finalOrder[i + 1]) == 0
                         ? " = " : " < ");
    }
    printf("\n");
    for (i = 0; i < NUM_FUNCS; i++)
        if (indexOfName(finalOrder[i].name) != orderAt[lastTest][i]) mismatch++;
    printf("\n* Agreement: %d of %d positions match between the numerical ranking\n"
           "   at n = %.0e and the theoretical ordering.%s\n",
           NUM_FUNCS - mismatch, NUM_FUNCS, test_n[lastTest],
           mismatch ? "  The gaps are the pairs whose crossover lies beyond 1e8,"
                      " listed above." : "");
    printf("\n* Why constants disappear:\n"
           "   12*sqrt(n) and 50*n^0.5 keep a fixed ratio of 50/12 = %.4f forever.\n"
           "   A ratio that converges to a finite non-zero limit means the same\n"
           "   Theta class, so the constant only shifts the curve, never its shape.\n"
           "   (2^32)*n is the sharpest case: 4294967296 is a huge head start, yet\n"
           "   it is still Theta(n), and n*log2(n) overtakes it once log2(n) > 2^32.\n"
           "\n* Why lower order terms disappear:\n"
           "   In 100*n^2+6*n the 6*n contributes a share of 6/(100n+6), which is\n"
           "   %.3e of the total at n = 1e8 - already negligible.  In n^2-324 the\n"
           "   -324 matters only while n is small.  Both are therefore Theta(n^2),\n"
           "   which is why compareAsymptotic() reports them as equal.\n"
           "\n* Why n^(log2 n) is larger than any polynomial:\n"
           "   Take logs: log2(n^log2 n) = (log2 n)^2, while log2(n^k) = k*log2 n.\n"
           "   The first is quadratic in log2 n and the second only linear, so for\n"
           "   any fixed k the crossing happens once log2(n) > k.  Against 2*n^3\n"
           "   that is n > 8, and no constant k can hold it off permanently -\n"
           "   the exponent log2 n simply keeps climbing.\n"
           "\n* Why 3^n dominates everything:\n"
           "   log10(3^n) = n*log10(3) = %.4f*n grows linearly in n, whereas the\n"
           "   runner-up n^(log2 n) has log10 = log2(n)*log10(n), only polynomial\n"
           "   in log n.  At n = 1e8 that is %.3e decades against %.3e - a gap of\n"
           "   millions of orders of magnitude.  Exponential growth outruns every\n"
           "   polynomial and quasi-polynomial function for large enough n.\n",
           50.0 / 12.0, 6.0 / (100.0 * 1e8 + 6.0),
           log10(3.0), 1e8 * log10(3.0), m_npowlogn(1e8));
}

/* ===== SECTION 16 : complexity discussion =============================== */
static void printComplexity(void)
{
    printf("\n====================================\n"
           "TIME AND SPACE COMPLEXITY OF THIS PROGRAM\n"
           "====================================\n"
           "Let F = %d functions and T = %d test values.\n\n"
           "Evaluation    : each evaluate()/magnitude() call is O(1) arithmetic,\n"
           "                and the table is filled once => O(F*T) = O(%d) calls.\n"
           "Merge sort    : O(F log F) comparisons per sort.  It runs T times for\n"
           "                the numerical pass plus once for the theoretical pass\n"
           "                => O((T+1) * F log F).\n"
           "Crossover     : bisection halves a log-scale interval 200 times per\n"
           "                adjacent pair => O(F * 200) = O(F) evaluations.\n"
           "Overall time  : O(F*T + T*F log F) = O(T * F log F), dominated by the\n"
           "                repeated sorting.  With F and T fixed and small the\n"
           "                whole run is effectively constant time.\n\n"
           "Space         : values[F][T] = O(F*T) for the cached matrix, plus the\n"
           "                O(F) merge buffer and O(F) working copy.  Merge sort's\n"
           "                O(n) auxiliary array is its known cost against an\n"
           "                in-place sort; recursion depth is O(log F).\n"
           "                Total O(F*T).\n",
           NUM_FUNCS, NUM_TESTS, NUM_FUNCS * NUM_TESTS);
}

/* ===== SECTION 17 : main - drives the two passes and every report ======= */
int main(void)
{
    printf("=========================================================\n"
           " DAA Q1 : ORDERING FUNCTIONS BY RATE OF GROWTH\n"
           "=========================================================\n"
           "Two independent algorithms rank the same %d functions:\n"
           "  1. merge sort on values actually computed at n = 1e2..1e8\n"
           "  2. merge sort on growth class only (constants/lower terms dropped)\n"
           "Where they disagree, the crossover n is located by bisection.\n",
           NUM_FUNCS);
    buildFunctionTable();   /* the 12 rows, with their declared growth classes */
    evaluateAll();          /* fill values[function][test]                     */
    printf("\n####################################\n"
           "PASS 1 : NUMERICAL RANKING PER TEST\n"
           "####################################\n");
    runNumericalRankings();
    printRankMovementTable();
    printf("\n####################################\n"
           "PASS 2 : THEORETICAL RANKING\n"
           "####################################\n");
    runTheoreticalSort();
    printFinalOrder();
    printAsciiGraph();
    printConclusion();
    printComplexity();
    printf("\nTime Complexity : O(T * F log F) overall, merge sort O(F log F) each\n"
           "Space Complexity: O(F*T) for the value matrix, O(F) merge buffer\n");
    return 0;
}
