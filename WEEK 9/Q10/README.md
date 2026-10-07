# Q10 - Greedy Superstring and the Conjecture Context

The program repeatedly chooses the ordered pair of strings with the largest
suffix/prefix overlap and merges that pair. Exact duplicates and strings
contained in another input string are removed first, since they do not change
the shortest-superstring problem. For at most 12 remaining strings, the
program also runs an exact subset-DP/backtracking reference and compares
lengths; larger cases use greedy only because the exact state space is
exponential.

## Correctness and limits

Every greedy merge contains both selected strings, so induction shows the
final string contains every input string. The subset DP considers every order
of the reduced strings and maximizes the sum of adjacent overlaps; reconstructing
that order gives the exact shortest-superstring length for the small reference
case. The greedy output is therefore always feasible, while the exact
comparison identifies whether it is optimal on a small supplied instance.

This does **not** prove a global approximation guarantee. The long-discussed
greedy superstring conjecture claimed a factor-2 guarantee for maximum-overlap
greedy. The lab handout also describes a September 2026 purported disproof
(with a limiting ratio near 9/4) and notes that it has not yet been officially
validated. This program is an experimental implementation and should not treat
either claim as settled mathematics.

## Complexity

Let `T` be the total input length and `L` the longest original string. The
naive overlap routine may compare `O(length^2)` bytes. Because every merged
string has length at most `T`, scanning all pairs in all greedy rounds has a
conservative `O(n^3 T^2)` bound and `O(T+n)` working space. The exact reference
precomputes overlaps in `O(n^2 L^2)` time, then uses `O(2^n n^2)` DP time and
`O(2^n n + n^2 + T)` space. It is restricted to `n <= 12`.

## Build and run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q10_greedy_superstring.c -o q10
./q10
```

Each string is entered as one non-empty line. `sample.txt` is captured from an
actual run.
