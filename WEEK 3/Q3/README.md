# Q3 — Max and Min using Divide and Conquer

> Find both the maximum and the minimum of an array of `n` elements using at
> most `3n/2` comparisons.

| | |
|---|---|
| **Source** | [`q3_max_min_divide_conquer.c`](q3_max_min_divide_conquer.c) |
| **Sample** | [`sample.txt`](sample.txt) |
| **Input** | None — built-in sweep over n |
| **Build** | `gcc -Wall -Wextra q3_max_min_divide_conquer.c -o q3` |
| **Output** | Printed to the terminal |

---

## Problem Statement

Using the divide-and-conquer approach, find the maximum and minimum elements
of an array of size `n` so that the number of comparisons is bounded by
`3n/2`. Implement in C to validate.

---

## The Algorithm

The obvious scan compares every element against both running extremes, costing
`2n − 2` comparisons. Divide and conquer does better:

```
MaxMin(A, lo, hi):
    if lo == hi:                          # one element
        return (A[lo], A[lo])                     # 0 comparisons
    if hi − lo == 1:                      # two elements
        if A[lo] < A[hi]: return (A[lo], A[hi])   # 1 comparison
        else:             return (A[hi], A[lo])
    mid = (lo + hi) / 2
    (min1, max1) = MaxMin(A, lo, mid)
    (min2, max2) = MaxMin(A, mid+1, hi)
    return (min(min1, min2), max(max1, max2))     # 2 comparisons
```

The gain comes from the base case. A pair is resolved with **one** comparison
that yields both its min and its max, where the naive scan would spend two.
Combining two solved halves costs exactly two comparisons regardless of the
sub-problem size.

---

## Complexity

```
T(n) = 2·T(n/2) + 2,   T(2) = 1,  T(1) = 0
```

Solving for `n = 2ᵏ`:

```
T(n) = 2 T(n/2) + 2
     = 4 T(n/4) + 4 + 2
     = 8 T(n/8) + 8 + 4 + 2
     ...
     = 2^(k−1) T(2) + (2^(k−1) + ... + 4 + 2)
     = n/2 · 1 + (n − 2)
     = 3n/2 − 2
```

So `T(n) = 3n/2 − 2`, which is within the required `3n/2` bound and saves
25% against the naive `2n − 2`.

### Why 3n/2 is optimal

`3n/2 − 2` is the information-theoretic lower bound for this problem. Every
element except the eventual max must lose at least one comparison, and every
element except the eventual min must win at least one — `n/2` pairwise
comparisons resolve `n/2` elements out of max contention and `n/2` out of min
contention simultaneously, then `n/2 − 1` more each are needed to find the
winner of each pool. No algorithm can do better.

### The iterative twin

The program also implements a `pairwise()` version: process elements two at a
time, compare the pair to each other (1), then the smaller against the running
min (1) and the larger against the running max (1) — 3 comparisons per 2
elements, so `3n/2` again, but with no recursion stack. It is the same
algorithm with the recursion unrolled, and the output confirms the identical
count.

---

## Sample Output

```
=====================================================
 COMPARISONS TO FIND BOTH MAX AND MIN
=====================================================
----------------------------------------------------------------------
       n        D&C   pairwise      naive     3n/2 - 2      saved
----------------------------------------------------------------------
       2          1          1          2            1      50.0%
       4          4          4          6            4      33.3%
      16         22         22         30           22      26.7%
      64         94         94        126           94      25.4%
     256        382        382        510          382      25.1%
    1024       1534       1534       2046         1534      25.0%
    4096       6142       6142       8190         6142      25.0%

T(n) = 2T(n/2) + 2, T(2) = 1  ->  T(n) = 3n/2 - 2.
The D&C column equals the 3n/2 - 2 column at every size, so the
bound is met exactly, and it saves 25% against the 2n - 2 scan.
```

### Reading the output

**The D&C column equals the `3n/2 − 2` column at every single size** — 1, 4,
22, 94, 382, 1534, 6142. Not approximately: exactly, at every one of the seven
sizes across a 2048× range. The recurrence solution is confirmed as an exact
identity, not an asymptotic bound. This is the strongest form of validation a
comparison count can give.

**The pairwise column is identical to the D&C column at every size.** The
recursion and its unrolled iterative form do the same comparisons in a
different order — as they must, since they are the same algorithm. Two
independent implementations agreeing exactly is a check on both.

**The saving converges to exactly 25%.** The ratio is
`(2n − 2 − (3n/2 − 2)) / (2n − 2) = (n/2)/(2n − 2)`, which tends to
`1/4` as n grows. The measured column reads 50.0%, 33.3%, 26.7%, 25.4%,
25.1%, 25.0%, 25.0% — settling onto the predicted limit from above, because
the `−2` term matters at small n and fades at large n.

**All three methods agree on the answers.** Every run checks the D&C result,
the pairwise result and the naive scan result against each other, and aborts
on any mismatch. The comparison counts are only printed because all three
returned the same min and max at every size.

---

## Committed Artefacts

The program itself writes nothing to disk. The plots below were generated by
[`../make_plots.py`](../make_plots.py), which compiles the source, runs it, and
parses the table above — so no figure can disagree with the measured counts.

| File | Description |
|------|-------------|
| [`plots/1_comparisons.png`](plots/1_comparisons.png) | D&C, pairwise and naive counts against the `3n/2 − 2` line, log–log |
| [`plots/2_saving.png`](plots/2_saving.png) | Percentage saved over the naive scan, converging on 25% from above |

---

## Files

| File | Description |
|------|-------------|
| [`q3_max_min_divide_conquer.c`](q3_max_min_divide_conquer.c) | Solution source |
| [`sample.txt`](sample.txt) | Sample build/run output |
| [`plots/`](plots/) | Figures generated from the measured table |
