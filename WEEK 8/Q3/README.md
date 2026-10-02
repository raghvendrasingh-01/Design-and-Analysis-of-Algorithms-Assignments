# Q3 - Longest Common Subsequence

The program reads two sequences as two lines (spaces are allowed), computes the
length of their longest common subsequence, and reconstructs one such sequence.
An empty line is a valid sequence.

## Algorithm and correctness

Let `L[i][j]` be the LCS length of the first `i` characters and first `j`
characters. If the final characters match, `L[i][j] = L[i-1][j-1] + 1`; otherwise
it is the larger of `L[i-1][j]` and `L[i][j-1]`. These cases cover every optimal
subsequence, so induction on `i+j` proves the table optimal. Starting at the
bottom-right and following matching diagonals or a larger neighboring value
traces back an actual optimal subsequence.

## Complexity

For lengths `m` and `n`, the algorithm takes `O(mn)` time and `O(mn)` space.

## Build and run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q3_lcs.c -o q3
./q3 < input.txt
```

The input lines are dynamically grown, and table-size/allocation overflow is
checked before use.

### Sample input

```text
ABCBDAB
BDCABA
```

The sample output is in [`sample.txt`](sample.txt). One valid LCS is `BCBA`.
