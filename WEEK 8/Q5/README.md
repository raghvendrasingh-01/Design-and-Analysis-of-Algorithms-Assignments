# Q5 - Maximum Sum Increasing Subsequence

The program reads `n` followed by `n` positive integers and finds a strictly
increasing subsequence with maximum sum. It prints the sum and one subsequence
achieving it. An empty array is accepted and produces sum zero.

## Algorithm and correctness

`sum[i]` stores the largest sum of a strictly increasing subsequence ending at
`A[i]`. Start with `sum[i] = A[i]`; for every earlier `j` with `A[j] < A[i]`,
try `sum[j] + A[i]`. Every non-singleton subsequence ending at `i` has one of
these predecessors, so induction proves each state optimal. The largest state
is the optimal answer, and stored predecessors reconstruct it.

## Complexity

The nested dynamic-programming loops take `O(n^2)` time and the value,
predecessor, and reconstruction arrays use `O(n)` space. `uint64_t` arithmetic
is checked; an answer exceeding `UINT64_MAX` is reported rather than wrapped.

## Build and run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q5_maximum_sum_increasing_subsequence.c -o q5
./q5 < input.txt
```

Non-positive or malformed input and allocation-size overflow are rejected.

### Sample input

```text
7
1 101 2 3 100 4 5
```

The sample output is in [`sample.txt`](sample.txt); the maximum sum is 106 from
`1 2 3 100`.
