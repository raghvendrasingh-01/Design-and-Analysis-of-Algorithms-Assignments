#include <stdio.h>
#include <stdlib.h>

#define MAX 20

static long long cost[MAX][MAX];
static int split[MAX][MAX];

static void printOrder(int i, int j) {
    if (i == j) { printf("A%d", i); return; }
    putchar('('); printOrder(i, split[i][j]); printf(" x ");
    printOrder(split[i][j] + 1, j); putchar(')');
}

int main(void) {
    int dimension[] = {30, 35, 15, 5, 10, 20, 25};
    int n = (int)(sizeof(dimension) / sizeof(dimension[0])) - 1;
    for (int i = 1; i <= n; i++) cost[i][i] = 0;
    for (int length = 2; length <= n; length++) {
        for (int i = 1; i + length - 1 <= n; i++) {
            int j = i + length - 1; cost[i][j] = 0x7fffffffffffffffLL;
            for (int k = i; k < j; k++) {
                long long q = cost[i][k] + cost[k + 1][j]
                    + (long long)dimension[i - 1] * dimension[k] * dimension[j];
                if (q < cost[i][j]) { cost[i][j] = q; split[i][j] = k; }
            }
        }
    }
    printf("Dimensions: ");
    for (int i = 0; i <= n; i++) printf("%d%s", dimension[i], i == n ? "\n" : " x ");
    printf("Minimum scalar multiplications: %lld\n", cost[1][n]);
    printf("Optimal parenthesization: "); printOrder(1, n); putchar('\n');
    printf("DP states: O(n^2), transitions per state: O(n).\n");
    printf("Time: O(n^3), space: O(n^2).\n");
    return 0;
}
