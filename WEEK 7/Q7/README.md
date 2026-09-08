# Q7 - Matrix Chain Multiplication

For matrices `A_i ... A_j`, let `M[i][j]` be the minimum scalar multiplications
and `K[i][j]` store the split. The recurrence is

```text
M[i][j] = min over i <= k < j of
          M[i][k] + M[k+1][j] + p[i-1] p[k] p[j].
```

The `K` table reconstructs the optimal parenthesization. For dimensions
`30,35,15,5,10,20,25`, the minimum is `15125` multiplications.

Build and run:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q7_matrix_chain.c -o q7
./q7
```

There are `O(n^2)` intervals and up to `n` split points per interval, giving
`O(n^3)` time and `O(n^2)` space.

### Correctness and complexity analysis

Every parenthesization of `A_i...A_j` has a final multiplication separating the
chain at some `k`. The two sides must independently use optimal
parenthesizations; otherwise replacing a nonoptimal side would improve the complete
solution. The recurrence evaluates the cost of every possible final split and
keeps the least one. The split table stores enough information to reconstruct
that optimal order.

There are `n(n+1)/2 = Theta(n^2)` intervals. An interval of length `l` tests
`l-1` split points. Summing over lengths gives
`sum(l-1)(n-l+1) = Theta(n^3)` scalar candidate evaluations. The cost and split
tables each contain `Theta(n^2)` entries, so the space complexity is
`Theta(n^2)`. Parenthesization printing visits `Theta(n)` matrices and uses
`O(n)` recursion stack space.
