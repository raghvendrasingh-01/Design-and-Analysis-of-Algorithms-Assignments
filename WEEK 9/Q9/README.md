# Q9 - Hu-Tucker / Optimal Alphabetic Merge Simulation

This implementation uses the exact interval-DP formulation of the
Hu-Tucker/optimal-alphabetic-tree problem. The input weights remain in their
given order; no split may interleave two intervals. For an interval `[i, j]`,
the recurrence chooses a root split `k` and adds the interval's total weight,
because joining the two child subtrees increases every leaf depth by one:

`DP[i,j] = sum(i..j) + min(DP[i,k] + DP[k+1,j])`.

The program prints the minimum `sum(wi * depth(i))` and the corresponding
ordered merge tree. It is an exact reference implementation rather than a
claim that adjacent Huffman-style merging is always alphabetically legal.

## Correctness

Every alphabetic binary tree has a root whose left and right leaves are two
contiguous, non-empty intervals. Removing that root gives two optimal
subproblems for their intervals; otherwise replacing one with a cheaper tree
would improve the original tree. Conversely, combining optimal subtrees for
every candidate split produces a valid alphabetic tree and adds exactly the
interval sum. Induction on interval length proves that the recurrence and the
stored split produce an optimal alphabetic tree.

## Complexity

There are `O(n^2)` intervals and `O(n)` candidate splits per interval, so the
time complexity is `O(n^3)` and memory complexity is `O(n^2)`. To keep the
interactive classroom program bounded, `1 <= n <= 250`; weights and the final
cost must fit `uint64_t`.

## Build and run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q9_hu_tucker_simulation.c -o q9
./q9
```

Malformed, non-positive, or overflowing weights are rejected. `sample.txt`
contains output from an actual run.
