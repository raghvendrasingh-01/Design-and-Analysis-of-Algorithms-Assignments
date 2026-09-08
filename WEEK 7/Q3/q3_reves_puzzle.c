#include <stdio.h>
#include <stdlib.h>

static unsigned long long dp[64], split[64];

static void prepare(int n) {
    dp[0] = 0;
    for (int i = 1; i <= n; i++) {
        dp[i] = ~0ULL;
        for (int k = 1; k <= i; k++) {
            unsigned long long candidate = 2 * dp[i - k] + ((1ULL << k) - 1);
            if (candidate < dp[i]) { dp[i] = candidate; split[i] = k; }
        }
    }
}

static void hanoi3(int n, char from, char to, char spare, unsigned long long *moves) {
    if (!n) return;
    hanoi3(n - 1, from, spare, to, moves);
    (*moves)++;
    if (*moves <= 40) printf("Move %llu: disk %d, %c -> %c\n", *moves, n, from, to);
    hanoi3(n - 1, spare, to, from, moves);
}

static void reve(int n, char from, char to, char a, char b, unsigned long long *moves) {
    if (!n) return;
    if (n == 1) { (*moves)++; if (*moves <= 40) printf("Move %llu: disk 1, %c -> %c\n", *moves, from, to); return; }
    int k = (int)split[n];
    reve(n - k, from, a, b, to, moves);
    hanoi3(k, from, to, b, moves);
    reve(n - k, a, to, from, b, moves);
}

int main(void) {
    int n = 8; unsigned long long moves = 0;
    prepare(n);
    printf("Disks: %d\nOptimal split for 8 disks: %llu\n", n, split[n]);
    reve(n, 'A', 'D', 'B', 'C', &moves);
    printf("Total moves: %llu\n", moves);
    printf("DP recurrence: T(n)=min(2T(n-k)+2^k-1), over 1<=k<=n.\n");
    printf("Time to print: O(T(n)); DP time: O(n^2); recursion space: O(n).\n");
    return 0;
}
