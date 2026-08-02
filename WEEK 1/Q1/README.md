# Q1 — Put Them in Order

> Arrange 12 given functions in increasing order of growth for sufficiently large `n`.

| | |
|---|---|
| **Source** | [`q1_growth_order.c`](q1_growth_order.c) |
| **Sample file** | [`sample.txt`](sample.txt) |
| **Input** | None — the function set is built into the program |
| **Build** | `gcc -Wall -Wextra q1_growth_order.c -o q1 -lm` |

---

## Problem Statement

Using an implementation in C, place the following functions in a list by
increasing order of growth for sufficiently large values of `n`:

```text
n·log₂n        12√n         1/n          n^(log₂ n)
100n² + 6n     n^0.51       n² − 324     50n^0.5
2n³            3ⁿ           2³²·n        log₂n
```

---

## Approach

The final ordering is never hardcoded. It is derived **twice, by two independent
routes**, and the two answers are then confronted against each other.

**Pass 1 — numerical ranking.** Each of the 12 functions is evaluated at
`n = 10²` through `10⁸` and sorted at every decade. Ranking is done on a
closed-form `log₁₀(f(n))` *magnitude* rather than on the raw value, because `3ⁿ`
overflows a `double` well before `n = 10⁸` — the printed value is already `INF` at
`n = 10³` — and anything that overflowed would compare as a tie with everything
else that had. Since `log₁₀` is strictly increasing, ordering by magnitude is
identical to ordering by value, and it keeps `3ⁿ` separable from `n^(log₂ n)`,
which does still fit (`4.0123e+212` at `n = 10⁸`).

**Pass 2 — asymptotic ranking.** The same 12 functions are sorted again by a
comparator that reads only the declared growth class and exponent. Constant
factors (12 vs 50, the `2³²`, the 100) and lower-order terms (the `+6n`, the
`−324`) are structurally invisible to it. Functions of genuinely equal growth are
reported as ties rather than broken arbitrarily.

**Sorting.** Both passes share one hand-written **stable merge sort** that is
generic over a function-pointer comparator. `qsort` is never used, so the sort
itself is part of the answer. Stability is what lets equal-growth functions keep
their table order instead of jittering between runs.

The program also prints a rank-movement table (which functions were still
changing position at the largest `n` tested) and an ASCII bar chart drawn on a
log-of-log scale, since the values span roughly `10⁻⁸` to `10^(4.8×10⁷)`.

---

## Time Complexity

Let `F = 12` functions and `T = 7` test values.

| Stage | Cost |
|-------|------|
| Evaluation of the value matrix | Θ(F·T) |
| Merge sort, per pass | Θ(F log F) |
| All sorts (T numerical + 1 asymptotic) | Θ((T+1)·F log F) |
| Canonical index lookup by name | Θ(F) per lookup, Θ(T·F²) in total |
| Pairwise ordering-stability comparison | Θ(T²·F) |
| **Overall** | **Θ(T·F² + T²·F)** |

The name lookups dominate the sorting, since `F² > F log F`. With `F` and `T`
both fixed and small, the whole run is effectively constant time.

## Space Complexity

**Θ(F·T)** — dominated by the cached `values[F][T]` and `rankAt[F][T]` matrices
and the `orderAt[T][F]` ordering log. On top of that sit a Θ(F) merge buffer, a
Θ(F) working copy, and Θ(log F) recursion depth inside the merge sort.

---

## Sample Input

The program takes no input.

```text
(none)
```

## Sample Output

Abridged — the full run prints 302 lines, including a ranked table per decade,
the rank-movement table, the ASCII chart, and the complexity discussion.

```text
=========================================================
 DAA Q1 : ORDERING FUNCTIONS BY RATE OF GROWTH
=========================================================

####################################
PASS 1 : NUMERICAL RANKING PER TEST
####################################

====================================
n = 100
====================================
Rank  Function         Value
-------------------------------------------
  1   1/n              1.0000e-02
  2   log2(n)          6.6439e+00
  ...
 12   3^n              5.1538e+47

... (one table per decade, n = 10^2 .. 10^8) ...

====================================
RANK MOVEMENT BETWEEN CONSECUTIVE TESTS
====================================
A function that never moves has already reached its asymptotic slot.

Function          1e2    1e3    1e4    1e5    1e6    1e7    1e8     Moves
--------------------------------------------------------------------------
1/n               1      1      1      1      1      1      1       0
log2(n)           2      2      2      2      2      2      2       0
...

####################################
PASS 2 : THEORETICAL RANKING
####################################

====================================
FINAL ASYMPTOTIC ORDER
====================================
   1/n                [decays towards 0, so it is below everything else]
   |
   v
   log2(n)            [grows slower than n^c for every c > 0]
   |
   v
   12*sqrt(n)         [polynomial of degree 0.5, constants dropped]
   =  (same growth class, neither dominates)
   50*n^0.5           [polynomial of degree 0.5, constants dropped]
   |
   v
   n^0.51             [polynomial of degree 0.51, constants dropped]
   |
   v
   (2^32)*n           [polynomial of degree 1, constants dropped]
   |
   v
   n*log2(n)          [n^1 with an extra log2 n factor, so just above linear]
   |
   v
   100*n^2+6*n        [polynomial of degree 2, constants dropped]
   =  (same growth class, neither dominates)
   n^2-324            [polynomial of degree 2, constants dropped]
   |
   v
   2*n^3              [polynomial of degree 3, constants dropped]
   |
   v
   n^(log2 n)         [exponent log2 n keeps growing, beats every fixed power]
   |
   v
   3^n                [base 3^n, log of it is linear in n: the steepest here]

====================================
CONCLUSION
====================================

* Numerical ordering at n = 1e+08 (merge sort on measured values):
   1/n < log2(n) < n^0.51 < 12*sqrt(n) < 50*n^0.5 < n*log2(n)
       < n^2-324 < (2^32)*n < 100*n^2+6*n < 2*n^3 < n^(log2 n) < 3^n

* Final theoretical ordering (merge sort on growth class):
   1/n < log2(n) < 12*sqrt(n) = 50*n^0.5 < n^0.51 < (2^32)*n
       < n*log2(n) < 100*n^2+6*n = n^2-324 < 2*n^3 < n^(log2 n) < 3^n

* Agreement: 5 of 12 positions match between the numerical ranking
   at n = 1e+08 and the theoretical ordering.

Time Complexity : O(T * F log F) overall, merge sort O(F log F) each
Space Complexity: O(F*T) for the value matrix, O(F) merge buffer
```

---

## Sample Explanation

**The answer to the question** is the asymptotic ordering from Pass 2:

```text
1/n  <  log₂n  <  12√n = 50n^0.5  <  n^0.51  <  2³²·n
     <  n·log₂n  <  100n²+6n = n²−324  <  2n³  <  n^(log₂ n)  <  3ⁿ
```

Two pairs are genuine **ties**, not orderings: `12√n` and `50n^0.5` are both
Θ(n^0.5) and keep a fixed ratio of `50/12 ≈ 4.1667` forever, while `100n²+6n`
and `n²−324` are both Θ(n²).

**The two passes agree on only 5 of 12 positions**, and that gap is the point of
the exercise:

- `n^0.51` *must* eventually overtake `12√n`, since `0.51 > 0.5`. At `n = 10⁸` it
  has not: `12√n = 120,000` still dwarfs `n^0.51 ≈ 12,023`. The crossover needs
  `n^0.01 > 12`, i.e. `n > 12¹⁰⁰ ≈ 8.3 × 10¹⁰⁷` — far outside any testable range.
- `n²−324` *must* eventually overtake `2³²·n`, but at `n = 10⁸` the constant
  `2³² = 4,294,967,296` is still a large enough head start that the linear
  function measures higher. That crossover sits at `n ≈ 4.29 × 10⁹`, one and a
  half decades past the largest `n` tested.

So "sufficiently large `n`" is a real caveat, not a formality. The numerical pass
reports its disagreement with the asymptotic pass instead of hiding it.

**Why `n^(log₂ n)` beats every polynomial.** Take logs:
`log₂(n^(log₂ n)) = (log₂ n)²`, which is quadratic in `log₂ n`, whereas
`log₂(n^k) = k·log₂ n` is only linear. For any fixed `k` the crossing happens
once `log₂ n > k`, and no constant `k` can hold it off permanently.

**Why `3ⁿ` dominates.** `log₁₀(3ⁿ) = n·log₁₀3 ≈ 0.4771n` grows linearly in `n`.
At `n = 10⁸` that is about `4.771×10⁷` decades against roughly `2.126×10²` decades
for the runner-up `n^(log₂ n)` — a gap of millions of orders of magnitude.

> **Note.** The header comment and startup banner mention locating crossover
> points by bisection. That search is not present in the current source; the
> disagreement between the two passes is reported, but the crossover `n` itself is
> not computed.

## Committed Artefacts

The program itself writes nothing to disk. The plots below were produced
separately from its tabulated output and committed alongside the source.

| File | Description |
|------|-------------|
| [`plots/1_overview_linear.png`](plots/1_overview_linear.png) | All 12 functions on linear axes — the fast growers swamp everything else |
| [`plots/2_group_a_sublinear.png`](plots/2_group_a_sublinear.png) | The sublinear group: `1/n`, `log₂ n`, `12√n`, `50n^0.5`, `n^0.51` |
| [`plots/3_group_b_linear_quad.png`](plots/3_group_b_linear_quad.png) | Linear to quadratic: `2³²·n`, `n log₂ n`, `100n²+6n`, `n²−324` |
| [`plots/4_group_c_fast.png`](plots/4_group_c_fast.png) | The fast group: `2n³`, `n^(log₂ n)`, `3ⁿ` |
| [`plots/5_all_loglog.png`](plots/5_all_loglog.png) | All 12 on log-log axes, where each polynomial becomes a straight line of slope equal to its degree |
| [`plots/6_decades_log10.png`](plots/6_decades_log10.png) | `log₁₀` of each function — the ranking actually used for `3ⁿ` and `n^(log₂ n)` |
| [`plots/7_crossovers.png`](plots/7_crossovers.png) | The two pairs that have not yet crossed by `n = 10⁸` |

---

## Build and Run

```bash
gcc -Wall -Wextra q1_growth_order.c -o q1 -lm   # -lm is required: log, pow, sqrt
./q1
```

The program writes nothing to disk. All output goes to standard output.

## Files

| File | Description |
|------|-------------|
| [`q1_growth_order.c`](q1_growth_order.c) | Solution source |
| [`sample.txt`](sample.txt) | Sample input and output |
| [`plots/`](plots) | Seven plots of the 12 functions — overview, per-group, log-log, decades, and crossovers |
