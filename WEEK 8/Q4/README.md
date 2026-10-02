# Q4 - Longest Increasing Subsequence

The program reads `n` followed by `n` integers and finds a longest **strictly**
increasing subsequence. It prints both its length and one reconstructed sequence.
An empty array is accepted; equal values cannot extend a subsequence.

## Algorithm and correctness

For each position `i`, `length[i]` is the longest increasing subsequence ending
at `i`. It is initialized to 1 and considers every earlier `j` with
`A[j] < A[i]`, taking `length[j] + 1` when that is better. Every increasing
subsequence ending at `i` has exactly such a predecessor (or only `A[i]`), so
induction on the index proves each `length[i]` optimal. Predecessor indices
reconstruct a global maximum.

## Complexity

The nested loops take `O(n^2)` time. The DP, predecessor, and reconstruction
arrays use `O(n)` dynamic space.

## Build and run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q4_longest_increasing_subsequence.c -o q4
./q4 < input.txt
```

Malformed input and allocation-size overflow are rejected.

### Sample input

```text
8
10 22 9 33 21 50 41 60
```

The sample output is in [`sample.txt`](sample.txt); one LIS is `10 22 33 50 60`.
