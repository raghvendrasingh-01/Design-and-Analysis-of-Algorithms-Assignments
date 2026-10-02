# Q6 - Edit Distance with Traceback

The program reads source string `A` and target string `B`, then computes the
minimum number of insertions, deletions, and substitutions needed to transform
`A` into `B`. It prints a complete forward traceback, including matches, so the
chosen optimal alignment is visible.

## Algorithm and correctness

`D[i][j]` is the minimum cost for the first `i` source characters and first
`j` target characters. The recurrence takes the minimum of deletion,
insertion, and diagonal match/substitution. Backtracking from `D[m][n]` always
chooses a predecessor that realizes the stored optimum; reversing those steps
therefore gives an optimal transformation. The diagonal tie is chosen first
for a stable, readable traceback.

## Complexity

The dynamic-programming table uses `O(mn)` time and `O(mn)` memory. The
traceback uses `O(m+n)` additional memory.

## Build and run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q6_edit_distance.c -o q6
./q6
```

Enter one string per prompt; empty lines are valid strings. `sample.txt`
contains output from an actual run.

## Sample behavior

For `kitten` and `sitting`, the program reports distance `3` and shows one
optimal sequence of matches, one substitution, and one insertion.
