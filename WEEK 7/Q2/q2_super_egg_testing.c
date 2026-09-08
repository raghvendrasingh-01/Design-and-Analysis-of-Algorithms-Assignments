#include <stdio.h>
#include <stdlib.h>

static int eggDrops(int eggs, int floors) {
    int **dp = malloc((size_t)(eggs + 1) * sizeof(*dp));
    if (!dp) exit(EXIT_FAILURE);
    for (int e = 0; e <= eggs; e++) {
        dp[e] = calloc((size_t)(floors + 1), sizeof(**dp));
        if (!dp[e]) exit(EXIT_FAILURE);
    }
    for (int f = 1; f <= floors; f++) dp[1][f] = f;
    for (int e = 2; e <= eggs; e++) {
        for (int f = 1; f <= floors; f++) {
            dp[e][f] = f;
            for (int drop = 1; drop <= f; drop++) {
                int broken = dp[e - 1][drop - 1];
                int safe = dp[e][f - drop];
                int worst = 1 + (broken > safe ? broken : safe);
                if (worst < dp[e][f]) dp[e][f] = worst;
            }
        }
    }
    int answer = dp[eggs][floors];
    for (int e = 0; e <= eggs; e++) free(dp[e]);
    free(dp);
    return answer;
}

int main(void) {
    int eggs = 2, floors = 100;
    printf("Eggs: %d, floors: %d\n", eggs, floors);
    printf("Minimum guaranteed droppings: %d\n", eggDrops(eggs, floors));
    printf("General DP recurrence: D(e,f)=1+min max(D(e-1,x-1),D(e,f-x)).\n");
    printf("Time: O(E F^2), space: O(E F)\n");
    return 0;
}
