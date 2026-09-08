# Q3 - Reve's Four-Peg Puzzle

The Frame-Stewart recurrence chooses `k` disks to move with the ordinary
three-peg Hanoi algorithm and recursively moves the remaining `n-k` disks with
four pegs:

```text
T(n) = min [2T(n-k) + (2^k - 1)],  1 <= k <= n.
```

The program stores the best split for every size and recursively prints the
moves. For eight disks the optimal split sequence produces exactly `33` moves.

### Correctness and complexity analysis

For a chosen `k`, first move the top `n-k` disks to an auxiliary peg, move the
bottom `k` disks using the classical three-peg solution, and move the `n-k`
disks onto the destination. These three stages are legal and solve the puzzle,
so the recurrence is an upper bound. The DP compares every possible first
partition and retains the least-cost one; the Frame-Stewart strategy therefore
selects the best strategy among these partitions.

For each `i`, the program tests `i` possible values of `k`. The number of DP
comparisons is `1+2+...+n = Theta(n^2)`. The arrays `dp` and `split` use
`Theta(n)` space. Recursive output takes `Theta(T(n))` time because one record
is printed per move, and recursion depth is `O(n)`. For `n=8`, the computed
minimum is `T(8)=33`.

Build and run:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q3_reves_puzzle.c -o q3
./q3
```

Computing the DP table takes `O(n^2)` time and `O(n)` space. Printing the full
solution takes `O(T(n))` time, which is unavoidable because every move is output.
