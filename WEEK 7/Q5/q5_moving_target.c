#include <stdio.h>
#include <stdlib.h>

/* Return the next possible positions after shooting spot shot. */
static int nextSet(int possible, int shot, int n) {
    int next = 0;
    possible &= ~(1 << shot);
    for (int p = 0; p < n; p++) if (possible & (1 << p)) {
        if (p > 0) next |= 1 << (p - 1);
        if (p + 1 < n) next |= 1 << (p + 1);
    }
    return next;
}

static int findStrategy(int n, int result[]) {
    int states = 1 << n, start = states - 1;
    int *parent = malloc((size_t)states * sizeof(*parent));
    int *shot = malloc((size_t)states * sizeof(*shot));
    int *queue = malloc((size_t)states * sizeof(*queue));
    if (!parent || !shot || !queue) exit(EXIT_FAILURE);
    for (int i = 0; i < states; i++) parent[i] = -1;
    parent[start] = start; int head = 0, tail = 0; queue[tail++] = start;
    while (head < tail && parent[0] == -1) {
        int state = queue[head++];
        for (int s = 0; s < n; s++) {
            int next = nextSet(state, s, n);
            if (parent[next] == -1) { parent[next] = state; shot[next] = s; queue[tail++] = next; }
        }
    }
    int length = 0, state = 0;
    while (state != start) { result[length++] = shot[state]; state = parent[state]; }
    for (int i = 0; i < length / 2; i++) {
        int t = result[i]; result[i] = result[length - 1 - i]; result[length - 1 - i] = t;
    }
    free(parent); free(shot); free(queue); return length;
}

int main(void) {
    int n = 6;
    int sequence[64], length = findStrategy(n, sequence);
    printf("Hiding spots: %d\nShortest guaranteed strategy (%d shots): ", n, length);
    for (int i = 0; i < length; i++) printf("%d%s", sequence[i] + 1, i + 1 == length ? "\n" : " ");
    printf("The BFS state is the set of positions still possible after each shot.\n");
    printf("Time: O(n^2 2^n), space: O(2^n); the empty state proves a guarantee.\n");
    return 0;
}
