#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dictionary.h"

#define NSIZES   6
#define NSTRUCTS 6

/* The shared step counter declared in dictionary.h */
long long steps = 0;

void resetSteps(void)
{
    steps = 0;
}

static int sizes[NSIZES] = {100, 200, 400, 800, 1600, 3200};

static char *structName[NSTRUCTS] = {
    "Unsorted array",
    "Sorted array",
    "Singly unsorted",
    "Singly sorted",
    "Doubly unsorted",
    "Doubly sorted"
};

static char *opName[NOPS] = {
    "Search", "Insert", "Delete", "Maximum", "Minimum", "Predecessor", "Successor"
};

/* Shorter labels so the wide claim table fits in 80 columns */
static char *opShort[NOPS] = {
    "Search", "Insert", "Delete", "Max", "Min", "Pred", "Succ"
};

/* The claimed worst-case running times */
static char *claim[NSTRUCTS][NOPS] = {
    /*              Search      Insert  Delete  Max     Min     Pred    Succ   */
    /* U array  */ {"O(n)",     "O(1)", "O(1)", "O(n)", "O(n)", "O(n)", "O(n)"},
    /* S array  */ {"O(log n)", "O(n)", "O(n)", "O(1)", "O(1)", "O(1)", "O(1)"},
    /* S unsort */ {"O(n)",     "O(1)", "O(n)", "O(n)", "O(n)", "O(n)", "O(n)"},
    /* S sorted */ {"O(n)",     "O(n)", "O(n)", "O(1)", "O(1)", "O(n)", "O(1)"},
    /* D unsort */ {"O(n)",     "O(1)", "O(1)", "O(n)", "O(n)", "O(n)", "O(n)"},
    /* D sorted */ {"O(n)",     "O(n)", "O(1)", "O(1)", "O(1)", "O(1)", "O(1)"}
};

/* measured[structure][operation][size] */
static long long measured[NSTRUCTS][NOPS][NSIZES];

/* Run one structure at one size, filling in its seven step counts. */
void measureOne(int s, int n, long long result[NOPS])
{
    if (s == 0)
        uaMeasure(n, result);
    else if (s == 1)
        saMeasure(n, result);
    else if (s == 2)
        suMeasure(n, result);
    else if (s == 3)
        ssMeasure(n, result);
    else if (s == 4)
        duMeasure(n, result);
    else
        dsMeasure(n, result);
}

void printLine(int width)
{
    for (int i = 0; i < width; i++)
        printf("-");
    printf("\n");
}

void printClaimTable(void)
{
    printf("\n=====================================================================\n");
    printf(" ASYMPTOTIC WORST-CASE RUNNING TIMES (the claim)\n");
    printf("=====================================================================\n");

    printf("%-17s", "Structure");
    for (int j = 0; j < NOPS; j++)
        printf("%-9s", opShort[j]);
    printf("\n");
    printLine(80);

    for (int i = 0; i < NSTRUCTS; i++)
    {
        printf("%-17s", structName[i]);
        for (int j = 0; j < NOPS; j++)
            printf("%-9s", claim[i][j]);
        printf("\n");
    }
}

void printMeasurements(void)
{
    printf("\n=====================================================================\n");
    printf(" MEASURED WORST-CASE STEP COUNTS\n");
    printf("=====================================================================\n");

    for (int i = 0; i < NSTRUCTS; i++)
    {
        printf("\n%s\n", structName[i]);
        printLine(62);

        printf("%-14s", "n =");
        for (int k = 0; k < NSIZES; k++)
            printf("%8d", sizes[k]);
        printf("\n");

        for (int j = 0; j < NOPS; j++)
        {
            printf("%-14s", opName[j]);
            for (int k = 0; k < NSIZES; k++)
                printf("%8lld", measured[i][j][k]);
            printf("\n");
        }
    }
}

/* Work out the growth class from the last doubling of n.
 *
 *   ratio near 1  -> the cost ignores n           -> O(1)
 *   ratio near 2  -> the cost doubles with n      -> O(n)
 *   in between    -> doubling n added a constant  -> O(log n)
 */
char *inferClass(long long small, long long big)
{
    double ratio;

    if (small <= 0)
        return "O(1)";

    ratio = (double)big / (double)small;

    if (ratio < 1.02)
        return "O(1)";
    else if (ratio < 1.5)
        return "O(log n)";
    else
        return "O(n)";
}

int printValidation(void)
{
    int matches = 0;

    printf("\n=====================================================================\n");
    printf(" VALIDATION : CLAIMED CLASS vs CLASS INFERRED FROM MEASUREMENT\n");
    printf("=====================================================================\n");
    printf("%-17s %-13s %7s  %-9s %-9s %s\n",
           "Structure", "Operation", "Ratio", "Claimed", "Observed", "Verdict");
    printLine(72);

    for (int i = 0; i < NSTRUCTS; i++)
    {
        for (int j = 0; j < NOPS; j++)
        {
            long long small = measured[i][j][NSIZES - 2];
            long long big = measured[i][j][NSIZES - 1];
            double ratio = (small > 0) ? (double)big / (double)small : 1.0;
            char *observed = inferClass(small, big);
            int ok = (strcmp(claim[i][j], observed) == 0);

            if (ok)
                matches++;

            printf("%-17s %-13s %7.3f  %-9s %-9s %s\n",
                   structName[i], opName[j], ratio,
                   claim[i][j], observed, ok ? "MATCH" : "DIFFER");
        }
    }

    printLine(72);
    printf("Agreement: %d of %d claims reproduced by measurement.\n",
           matches, NSTRUCTS * NOPS);

    return matches;
}

int main(void)
{
    printf("=====================================================================\n");
    printf(" DAA Q1 : DICTIONARY OPERATIONS ACROSS SIX REPRESENTATIONS\n");
    printf("=====================================================================\n");

    printClaimTable();

    printf("\nMeasuring ");
    for (int i = 0; i < NSTRUCTS; i++)
    {
        for (int k = 0; k < NSIZES; k++)
        {
            long long result[NOPS];

            measureOne(i, sizes[k], result);

            for (int j = 0; j < NOPS; j++)
                measured[i][j][k] = result[j];
        }
        printf(".");
        fflush(stdout);
    }
    printf(" done\n");

    printMeasurements();
    printValidation();

    printf("\nTime Complexity  : see the claim table above, per structure\n");
    printf("                   and per operation\n");
    printf("Space Complexity : O(n) for every representation.  A singly\n");
    printf("                   linked list adds one pointer per node, a\n");
    printf("                   doubly linked list adds two.\n");

    return 0;
}
