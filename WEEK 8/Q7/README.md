# Q7 - Rod Cutting with Reconstruction

The program reads a rod length `n` and prices `p[i]` for every integral piece
length `i`. It reports the maximum revenue and the exact piece lengths in one
optimal decomposition. Negative prices are accepted; the length-`n` piece is
always considered, so the rod is allowed to remain uncut.

## Algorithm and correctness

Let `R[j]` be the best revenue for a rod of length `j`. For every `j`, the
first piece can have any length `i` from `1` through `j`, so

```text
R[j] = max(R[j-i] + p[i]).
```

`first_piece[j]` stores an `i` attaining that maximum. Every decomposition has
some first piece and an optimal remainder (otherwise the remainder could be
improved), so the recurrence is exhaustive and optimal. Following the stored
pieces from `n` to zero reconstructs an exact sum of `n`.

## Complexity

There are `n` subproblems and up to `n` candidate first pieces per subproblem:
`O(n^2)` time and `O(n)` space.

## Build and run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q7_rod_cutting.c -o q7
./q7
```

The implementation uses checked signed 64-bit revenue arithmetic and rejects
an overflowing candidate.

## Sample behavior

For `n = 8` and prices `1 5 8 9 10 17 17 20`, it reports revenue `22` and
the exact decomposition `2 + 6`.
