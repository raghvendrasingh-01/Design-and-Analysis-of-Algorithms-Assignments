# Q8 - Optimal Binary Search Tree (OBST)

The program reads sorted distinct keys, successful-search probabilities `p1...pn`,
and unsuccessful-search (dummy-key) probabilities `q0...qn`. It computes the
minimum expected search cost and prints the reconstructed tree, including every
dummy leaf.

## Algorithm and correctness

For the interval `ki...kj`, `E[i,j]` is the minimum weighted search cost and
`W[i,j]` is the sum of all probabilities in that interval and its surrounding
dummy keys. The standard recurrence is

```text
E[i,j] = min(r=i..j) (E[i,r-1] + E[r+1,j] + W[i,j])
W[i,j] = W[i,j-1] + p[j] + q[j]
```

The base case is `E[i,i-1] = q[i-1]`. Every BST interval has exactly one root
`r`; its left and right subtrees are independent optimal subproblems, and all
their searches become one level deeper, represented by adding `W[i,j]`.
Thus the recurrence examines every possible root and is optimal. `root[i,j]`
stores the choice used for reconstruction.

## Complexity

There are `O(n^2)` intervals and `O(n)` possible roots per interval, giving
`O(n^3)` time and `O(n^2)` space.

## Build and run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q8_optimal_bst.c -o q8
./q8
```

Probabilities must be finite, non-negative, and sum to 1 (within a small
floating-point tolerance). The implementation uses `long double` tables.

## Sample behavior

For keys `10 12 20`, successful probabilities `0.15 0.10 0.05`, and dummy
probabilities `0.05 0.10 0.05 0.50`, it prints the minimum expected cost and
an indented tree containing both successful keys and dummy leaves.
