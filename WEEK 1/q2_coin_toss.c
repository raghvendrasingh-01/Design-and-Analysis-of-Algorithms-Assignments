#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define N_STEPS 6
#define N_COINS 5
#define GRAPH_SCALE 50

int bias[N_COINS] = {50, 60, 70, 90, 30};
long long heads[N_COINS];
long long tails[N_COINS];
double fraction[N_COINS];

int toss(int percent_heads)
{
    return (rand() % 100) < percent_heads;
}

void print_convergence()
{
    printf("\n========== FAIR COIN CONVERGENCE ==========\n");
    printf("---------------------------------------------------------------\n");
    printf("Tosses\t\tHeads\tTails\tFraction\tError\n");
    printf("---------------------------------------------------------------\n");

    long long n = 10;

    for (int i = 0; i < N_STEPS; i++)
    {
        long long h = 0, t = 0;

        for (long long j = 0; j < n; j++)
        {
            if (toss(50))
                h++;
            else
                t++;
        }

        double frac = (double)h / n;

        printf("%8lld\t%5lld\t%5lld\t%.6f\t%.6f\n",
               n, h, t, frac, fabs(frac - 0.5));

        n *= 10;
    }

    printf("\nObservation:\n");
    printf("As the number of tosses increases, the observed probability\n");
    printf("approaches 0.5 (Law of Large Numbers).\n");
}

void print_comparison(int tosses)
{
    FILE *fp = fopen("coin_toss_analysis.csv", "w");

    if (fp == NULL)
    {
        printf("Unable to create CSV file.\n");
        return;
    }

    fprintf(fp, "Bias,Heads,Tails,Observed,Expected,Difference\n");

    printf("\n========== FAIR vs BIASED COINS ==========\n");
    printf("(%d tosses each)\n\n", tosses);

    printf("--------------------------------------------------------------------------\n");
    printf("Bias\tHeads\t\tTails\t\tObserved\tExpected\tDifference\n");
    printf("--------------------------------------------------------------------------\n");

    for (int i = 0; i < N_COINS; i++)
    {
        long long h = 0, t = 0;

        for (int j = 0; j < tosses; j++)
        {
            if (toss(bias[i]))
                h++;
            else
                t++;
        }

        heads[i] = h;
        tails[i] = t;
        fraction[i] = (double)h / tosses;

        double expected = bias[i] / 100.0;
        double diff = fabs(fraction[i] - expected);

        printf("%2d%%\t%8lld\t%8lld\t%.5f\t\t%.5f\t\t%.5f\n",
               bias[i], h, t, fraction[i], expected, diff);

        fprintf(fp, "%d,%lld,%lld,%.5f,%.5f,%.5f\n",
                bias[i], h, t, fraction[i], expected, diff);
    }

    fclose(fp);
printf("\nCSV file 'coin_toss_analysis.csv' created successfully.\nOpen it in Excel, LibreOffice or Google Sheets and insert a Bar Graph.\n");
}

void print_graph()
{
    printf("\n========== ASCII GRAPH ==========\n");
    printf("Observed Head Probability\n\n");

    for (int i = 0; i < N_COINS; i++)
    {
        printf("%2d%% (%.5f) | ", bias[i], fraction[i]);

        int blocks = (int)(fraction[i] * GRAPH_SCALE + 0.5);

        for (int j = 0; j < blocks; j++)
            printf("█");

        printf("\n");
    }
}

int main()
{
    int tosses;

    srand(time(NULL));

    printf("=============================================\n");
    printf("      FAIR vs BIASED COIN TOSS SIMULATION\n");
    printf("=============================================\n");

    printf("\nEnter number of tosses for comparison: ");
    scanf("%d", &tosses);

    print_convergence();
    print_comparison(tosses);
    print_graph();

    printf("\n============== CONCLUSION ==============\n");
    printf("1. A fair coin converges towards probability 0.5.\n");
    printf("2. A biased coin converges towards its own probability.\n");
    printf("3. Increasing the number of tosses reduces random error.\n");
    printf("4. This demonstrates the Law of Large Numbers.\n");

    printf("\nTime Complexity  : O(n)\n");
    printf("Space Complexity : O(1)\n");

    return 0;
}
