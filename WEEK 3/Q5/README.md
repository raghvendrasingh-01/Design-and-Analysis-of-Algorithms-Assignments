# Q5 — Multiplying Special-Pattern Matrices

> Two `n × n` matrices (`n = 2ᵏ`) have the recursive block structure
> `M = [[M₁, M₂], [M₂, M₁]]` all the way down to single elements. Multiply
> them in `O(n²)`.

| | |
|---|---|
| **Source** | [`q5_pattern_matrix_multiply.c`](q5_pattern_matrix_multiply.c) |
| **Sample** | [`sample.txt`](sample.txt) |
| **Input** | None — built-in sweep over powers of two |
| **Build** | `gcc -Wall -Wextra q5_pattern_matrix_multiply.c -o q5` |
| **Output** | Printed to the terminal |

---

## Problem Statement

Each matrix, when divided into four equal blocks, has identical diagonal
blocks and identical off-diagonal blocks:

```
M = | M1  M2 |
    | M2  M1 |
```

Each block has the same recursive structure, down to single integers. Give a
divide-and-conquer `O(n²)` algorithm for multiplying two such matrices.

---

## The Algorithm — Two Products, Not Eight

Write `A = [[A₁, A₂], [A₂, A₁]]` and `B = [[B₁, B₂], [B₂, B₁]]`. The block
product is:

```
C11 = A1B1 + A2B2      C12 = A1B2 + A2B1
C21 = A2B1 + A1B2      C22 = A2B2 + A1B1
```

So `C21 = C12` and `C22 = C11` — **the product has the same structure**, and
only two distinct blocks need computing. That alone halves the work, but the
real saving comes next.

### The diagonalising trick

Instead of four products (`A₁B₁`, `A₂B₂`, `A₁B₂`, `A₂B₁`), take the sum and
difference:

```
P = (A1 + A2)(B1 + B2) = A1B1 + A1B2 + A2B1 + A2B2 = C11 + C12
Q = (A1 − A2)(B1 − B2) = A1B1 − A1B2 − A2B1 + A2B2 = C11 − C12
```

and recover both blocks by addition:

```
C11 = (P + Q) / 2        C12 = (P − Q) / 2
```

**Two half-size products.** This works because the structure is exactly the
eigendecomposition of a 2×2 circulant: the matrix `[[X,Y],[Y,X]]` is
diagonalised by the sum/difference basis, so the product decouples into two
independent scalar-like multiplications.

### Why the recursion is well-founded

`A₁ + A₂` and `A₁ − A₂` must themselves have the special form for the
recursion to apply — and they do. If `A₁ = [[U,V],[V,U]]` and
`A₂ = [[W,X],[X,W]]`, then `A₁ + A₂ = [[U+W, V+X],[V+X, U+W]]`, which is
again of the form `[[·,·],[·,·]]` with equal diagonals and equal
off-diagonals. Addition and subtraction preserve the pattern, so every
recursive call receives valid input.

The `/2` is always exact: `P + Q = 2·C11` by construction, so the sum is
guaranteed even and integer arithmetic loses nothing.

---

## Complexity

```
T(n) = 2·T(n/2) + 6(n/2)²
```

Two recursive products, plus six block operations per level (four to form
the sums and differences, two to recover `C11` and `C12`).

By the Master Theorem with `a = 2`, `b = 2`, `f(n) = Θ(n²)`:

```
log_b(a) = log₂ 2 = 1  <  2
```

Case 3 applies — the combine step dominates:

```
T(n) = Θ(n²)
```

as required. Note the contrast with Q4: Strassen has `a = 7` so the
recursion dominates and the exponent rises above 2; here `a = 2` is small
enough that the `Θ(n²)` additions set the bound.

---

## Sample Output

```
=====================================================
 PATTERN MATRICES: 2 PRODUCTS INSTEAD OF 8
=====================================================
---------------------------------------------------------------
     n   D&C mul    D&C add    D&C total    n^3 mul   total/n^2     speedup
---------------------------------------------------------------
     2         2          6            8          8        2.00         1.0
     4         4         36           40         64        2.50         1.6
     8         8        168          176        512        2.75         2.9
    16        16        720          736       4096        2.88         5.6
    32        32       2976         3008      32768        2.94        10.9
    64        64      12096        12160     262144        2.97        21.6
   128       128      48768        48896    2097152        2.98        42.9
   256       256     195840       196096   16777216        2.99        85.6
```

### Reading the output

**Scalar multiplications equal `n` exactly** — 2, 4, 8, 16, 32, 64, 128, 256.
The recursion `2T(n/2)` with `T(1) = 1` multiplication bottoms out at exactly
`n` leaves. Multiplying two `256 × 256` matrices takes **256 scalar
multiplications**, where the definition would take 16,777,216. The entire
product is determined by `n` numbers because each matrix only has `n` distinct
entries.

**`total/n²` converges to 3.00** — 2.00, 2.50, 2.75, 2.88, 2.94, 2.97, 2.98,
2.99. The exact total is `3n² − 2n`, so dividing by `n²` gives `3 − 2/n`,
which approaches 3 from below. The measured column tracks this precisely.
An asymptotic constant landing this cleanly across a 128× range confirms
`Θ(n²)` rather than merely bounding it.

**The speedup doubles with n** — 1.0, 1.6, 2.9, 5.6, 10.9, 21.6, 42.9, 85.6.
This is `n³/3n² = n/3` growing linearly, which is what separating `O(n²)` from
`O(n³)` looks like. At `n = 256` the structured algorithm does 196,096
operations to the naive method's 16.7 million.

**The structure is verified, not assumed.** `checkForm()` confirms both input
matrices genuinely satisfy `M₁ = M₁` and `M₂ = M₂` across the quadrant split
before multiplying, and the same check runs on the *output* — confirming the
claim that the product inherits the pattern. Every result is also compared
entry-by-entry against the naive triple loop, and any mismatch aborts the run.

**Matrices are generated via `f[i ^ j]`.** Applying the block rule at every
level down to single elements forces `M[i][j]` to depend only on `i XOR j`,
so filling from one array of `n` values produces exactly the family the
question describes — and explains why only `n` multiplications are needed.

---

## Committed Artefacts

The program itself writes nothing to disk. The plots below were generated by
[`../make_plots.py`](../make_plots.py), which compiles the source, runs it, and
parses the table above — so no figure can disagree with the measured counts.

| File | Description |
|------|-------------|
| [`plots/1_operations_growth.png`](plots/1_operations_growth.png) | Total operations tracking `3n²` against the naive `n³`, with multiplications flat at `n`, log–log |
| [`plots/2_normalised_constant.png`](plots/2_normalised_constant.png) | Total divided by `n²`, matching `3 − 2/n` and approaching 3 |

---

## Files

| File | Description |
|------|-------------|
| [`q5_pattern_matrix_multiply.c`](q5_pattern_matrix_multiply.c) | Solution source |
| [`sample.txt`](sample.txt) | Sample build/run output |
| [`plots/`](plots/) | Figures generated from the measured table |
