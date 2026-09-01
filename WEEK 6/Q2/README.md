# Q2 - Operations on Square Matrices

This program applies the requested operations to two small `3 x 3` matrices.
The matrices are stored in ordinary two-dimensional C arrays.

## Build and Run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q2_matrix_operations.c -o q2 -lm
./q2
```

Files:

- [`q2_matrix_operations.c`](q2_matrix_operations.c) - solution
- [`sample.txt`](sample.txt) - sample output

## Algorithms

| Operation | Method | Worst-case time |
|---|---|---:|
| Addition | Two nested loops | `O(n^2)` |
| Multiplication | Standard three nested loops | `O(n^3)` |
| Zero-matrix test | Check every entry | `O(n^2)` |
| Symmetry test | Compare `a[i][j]` with `a[j][i]` | `O(n^2)` |
| Determinant | Gaussian elimination | `O(n^3)` |
| In-place transpose | Swap values across the diagonal | `O(n^2)` |
| Dominant eigenpair | Power iteration for `k` steps | `O(k n^2)` |

## Complexity Derivation

- **Addition:** two loops visit all `n^2` entries once, so
  `T(n) = n * n = O(n^2)`.
- **Multiplication:** for each of the `n^2` output entries, the inner loop does
  `n` multiply-adds. Therefore `T(n) = n * n * n = O(n^3)`.
- **Zero test:** in the worst case the matrix is all zero, so all `n^2` entries
  are checked: `T(n) = O(n^2)`.
- **Symmetry test:** only entries above the diagonal are checked:
  `sum(i = 0 to n - 1) (n - i - 1) = n(n - 1)/2 = O(n^2)`.
- **Determinant:** elimination step `column` updates roughly `(n - column)^2`
  entries. Thus
  `T(n) = sum(k = 1 to n) k^2 = n(n + 1)(2n + 1)/6 = O(n^3)`.
- **Transpose:** each pair above the diagonal is swapped once, giving
  `n(n - 1)/2 = O(n^2)` swaps.
- **Power iteration:** one step computes a matrix-vector product using `n^2`
  multiplications and additions, plus `O(n)` normalization work. For `k` steps,
  `T(n, k) = k(n^2 + n) = O(k n^2)`.

The matrix output and temporary determinant copy use `O(n^2)` space. Transpose,
tests, and multiplication use only the arrays already shown plus `O(1)` scalar
variables; power iteration uses `O(n)` for its vectors.

### Determinant

Gaussian elimination converts a copy of the matrix into upper-triangular form.
The determinant is the product of the diagonal values. Swapping two rows changes
the sign of the determinant.

### Eigenvalue and Eigenvector

Power iteration repeatedly performs:

```text
v = A * v
v = v / ||v||
```

After enough steps, `v` approaches the dominant eigenvector when the matrix has
a unique dominant eigenvalue. This is an approximate numerical method, not a
method for finding every eigenvalue.

## Input

The two example matrices are declared in `main`. Change their values or change
`n` to test another square matrix. The program supports sizes up to `MAX = 10`.
