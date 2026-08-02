
#include <stdio.h>

#define N_MAX 25
#define PRINT_MOVES_UPTO 4

unsigned long long move_count = 0;
unsigned long long moves[N_MAX + 1];

void hanoi(int n, char from, char to, char aux, int print)
{
    if (n == 0) return;

    hanoi(n - 1, from, aux, to, print);

    move_count++;
    if (print)
        printf("Move disk %d from %c to %c\n", n, from, to);

    hanoi(n - 1, aux, to, from, print);
}

unsigned long long simulate(int n, int print)
{
    move_count = 0;
    hanoi(n, 'A', 'C', 'B', print);
    return move_count;
}

unsigned long long formula(int n)
{
    return (1ULL << n) - 1;
}

void create_csv(void)
{
    FILE *fp = fopen("toh.csv", "w");
    if (!fp)
    {
        printf("Unable to create toh.csv\n");
        return;
    }

    fprintf(fp, "Disks,Moves\n");
    for (int i = 1; i <= N_MAX; i++)
        fprintf(fp, "%d,%llu\n", i, moves[i]);

    fclose(fp);
    printf("\nCSV file 'toh.csv' created successfully.\nOpen it in Excel, LibreOffice or Google Sheets and insert a Line Chart.\n");
}

void print_examples(void)
{
    for (int i = 3; i <= PRINT_MOVES_UPTO; i++)
    {
        printf("\n===== %d DISKS =====\n", i);
        simulate(i, 1);
        printf("Total Moves = %llu\n", move_count);
    }
}

void print_table(void)
{
    printf("\n---------------------------------------------\n");
    printf("Disks\tSimulated\tFormula\n");
    printf("---------------------------------------------\n");

    for (int i = 1; i <= N_MAX; i++)
    {
        moves[i] = simulate(i, 0);
        printf("%2d\t%10llu\t%10llu\n", i, moves[i], formula(i));
    }
}

void print_graph(void)
{
    printf("\n=========== LOG SCALE ASCII GRAPH ===========\n");
    printf("Disks   Moves         Graph\n");
    printf("--------------------------------------------------------\n");

    for (int i = 1; i <= N_MAX; i++)
    {
        printf("%2d   %10llu   ", i, moves[i]);

        for (int j = 0; j < i; j++)
            printf("\u2588");

        printf("\n");
    }
}

int main()
{
    printf("===== Tower of Hanoi =====\n");

    print_examples();
    print_table();

    create_csv();

    print_graph();

    printf("\nTime Complexity  : O(2^n)\n");
    printf("Space Complexity : O(n)\n");

    return 0;
}
