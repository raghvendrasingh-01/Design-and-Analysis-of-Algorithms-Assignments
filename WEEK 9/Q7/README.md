# Q7 - Minimise Deviation in an Array

For every input element, the program first applies the only useful upward
operation: an odd value is doubled. It then keeps all values in a max-heap and
repeatedly halves the current maximum while it is even. The best
`maximum - minimum` seen during this two-way greedy process is retained and
the corresponding transformed array is printed.

## Correctness

Doubling an odd value is always beneficial before the descent phase: it makes
that value no smaller while making it available for later halving, so an
optimal solution has every odd input normalized upward. After normalization,
the minimum can only decrease when a current maximum is halved. Therefore the
only candidate states that can improve the deviation are obtained by halving
the current maximum. The max-heap enumerates exactly those states, and the
process stops when the maximum is odd and cannot be reduced. Recording every
state proves that the reported minimum is the best reachable deviation.

## Complexity

Let `M` be the largest normalized value. Each heap update costs `O(log n)`;
there are at most `O(n log M)` halvings. Total time is `O(n log M log n)` and
space is `O(n)`.

## Build and run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q7_minimize_deviation.c -o q7
./q7
```

`n` must be positive and each element must be a positive unsigned 64-bit
integer. Values whose required odd doubling would overflow are rejected.
`sample.txt` contains output from an actual run.
