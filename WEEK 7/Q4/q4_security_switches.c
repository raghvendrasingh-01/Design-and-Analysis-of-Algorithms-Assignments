#include <stdio.h>
#include <stdlib.h>

/* Switch positions are numbered 0..n-1 from left to right. */
static int legal(int state, int position, int n) {
    if (position == n - 1) return 1; /* rightmost switch */
    if (!(state & (1 << (position + 1)))) return 0;
    for (int j = position + 2; j < n; j++)
        if (state & (1 << j)) return 0;
    return 1;
}

static void printBits(int state, int n) {
    for (int i = 0; i < n; i++) putchar((state & (1 << i)) ? '1' : '0');
}

static int validTransition(int before, int after, int position, int n) {
    return legal(before, position, n)
        && after == (before ^ (1 << position));
}

static int depthLimitedDfs(int state, int goal, int n, int depth, int limit,
                           int pathStates[], int pathMoves[], int onPath[]) {
    if (state == goal) return 1;
    if (depth == limit) return 0;

    for (int position = 0; position < n; position++) {
        if (!legal(state, position, n)) continue;
        int next = state ^ (1 << position);
        if (onPath[next]) continue;

        pathStates[depth + 1] = next;
        pathMoves[depth] = position;
        onPath[next] = 1;
        if (depthLimitedDfs(next, goal, n, depth + 1, limit,
                            pathStates, pathMoves, onPath)) return 1;
        onPath[next] = 0;
    }
    return 0;
}

int main(void) {
    int n = 4, total = 1 << n, start = total - 1, goal = 0;
    int *pathStates = malloc((size_t)total * sizeof(*pathStates));
    int *pathMoves = malloc((size_t)total * sizeof(*pathMoves));
    int *onPath = calloc((size_t)total, sizeof(*onPath));
    if (!pathStates || !pathMoves || !onPath) return EXIT_FAILURE;

    int length;
    for (length = 0; length < total; length++) {
        for (int i = 0; i < total; i++) onPath[i] = 0;
        pathStates[0] = start; onPath[start] = 1;
        if (depthLimitedDfs(start, goal, n, 0, length,
                            pathStates, pathMoves, onPath)) break;
    }

    printf("Switches: %d\nMinimum moves: %d\n", n, length);
    printBits(start, n); putchar('\n');
    int state = start, valid = 1;
    for (int i = 0; i < length; i++) {
        int next = pathStates[i + 1], position = pathMoves[i];
        if (!validTransition(state, next, position, n)) valid = 0;
        printBits(next, n);
        printf("  (toggle switch %d from the left)\n", position + 1);
        state = next;
    }
    printf("Every transition obeys the rules: %s\n", valid ? "yes" : "no");
    printf("Method: recursive iterative-deepening DFS (minimum-depth solution).\n");
    printf("Worst-case time: exponential; space: O(2^n).\n");
    free(pathStates); free(pathMoves); free(onPath); return 0;
}
