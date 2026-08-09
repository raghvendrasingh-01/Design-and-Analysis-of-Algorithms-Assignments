# Q2 — Merge Sort vs Modified (Three-Way) Merge Sort

> Divide the input into thirds rather than halves, sort each third recursively, and
> combine with a three-way merge. What is the worst-case running time?

| | |
|---|---|
| **Source** | [`q2_merge_sort_variants.c`](q2_merge_sort_variants.c) |
| **Sample file** | [`sample.txt`](sample.txt) |
| **Input** | None — sizes and input classes are built into the program |
| **Build** | `gcc -Wall -Wextra q2_merge_sort_variants.c -o q2 -lm` |
| **Output** | Printed to the terminal — the counts and their normalisation by `n log₂ n` |

---

## Problem Statement

Consider the following modification to merge sort: divide the input array into
thirds (rather than halves), recursively sort each third, and finally combine the
results using a three-way merge subroutine. What is the worst-case running time of
this modified merge sort?

Write a C program to validate your claim by plotting the order of growth for both
the merge sort discussed in class and the modified merge sort described above.

---

## The Answer

**Both are O(n log n) in the worst case.**

| Variant | Recurrence | Master theorem | Depth | Result |
|---------|-----------|----------------|-------|--------|
| Ordinary | `T(n) = 2T(n/2) + O(n)` | `a=2, b=2`, `n^(log₂2) = n` | log₂ n | O(n log n) |
| Modified | `T(n) = 3T(n/3) + O(n)` | `a=3, b=3`, `n^(log₃3) = n` | log₃ n | O(n log n) |

Both land in case 2 of the master theorem: the work done merging at each level is
O(n), and it matches `n^(log_b a) = n`, so the total is `O(n log n)`.

The whole point is the change of base:

```
log₃ n = log₂ n / log₂ 3 = 0.6309 × log₂ n
```

A change of base is a **constant factor**, and O-notation absorbs constant
factors. Splitting into thirds gives you a shallower recursion tree, not a
different growth class. The same argument holds for any fixed branching factor
`k ≥ 2` — you would need `k` to grow with `n` before the class itself moved.

---

## What Actually Changes

The asymptotic class is identical, so the interesting question is what the
constant does. The program counts two things separately, and they move in
opposite directions:

| Counter | Winner | Why |
|---------|--------|-----|
| **Moves** (elements copied) | three-way | Every level copies all `n` elements, and there are `log₃ n` levels instead of `log₂ n`. Fewer levels, less copying — by exactly the factor 0.63. |
| **Comparisons** | two-way | Picking the smallest of *three* heads costs 2 comparisons per output element instead of 1. That doubling more than cancels the shallower tree. |

This is why the modification is not a free win in practice: it trades comparisons
for data movement. Which one matters depends on whether comparing or copying is
the expensive operation for your element type.

---

## Approach

**Both sorts are real implementations, not simulations.** `mergeSort2` splits at
`n/2` and merges with `merge2`. `mergeSort3` splits at `n/3` and `2n/3` and merges
with `merge3`, which repeatedly selects the smallest available head among the
three runs.

**The three-way merge is honest.** A shortcut implementation would merge runs 1
and 2 with the ordinary two-way merge and then merge the result with run 3 — but
that is two two-way merges, not a three-way merge, and it would not have the
comparison cost the question is really about. `merge3` compares the live heads
directly: 2 comparisons per output element while all three runs are alive, 1 once
a run is exhausted, none once two are.

**The n = 2 case is handled separately.** Two elements cannot be split into three
non-empty parts — `n/3` would be 0, the recursion would make no progress, and the
program would loop forever. Handling `n == 2` directly guarantees `third >= 1`
for every recursive call.

**Three input classes.** Random (fixed seed), already sorted, and reverse sorted.
Merge sort's cost barely depends on the input, and showing all three makes that
concrete rather than assumed.

**Both sorts are verified at every size.** The output is checked to be in
ascending order before any count is recorded.

**How the order of growth is shown.** The question asks for a plot. The program
prints the raw counts across six doubling sizes and then divides each one by
`n log₂ n`. That second table is the stronger evidence: a curve drawn on a linear
axis makes `n log n` and `n²` look much alike, whereas a normalised column that
stays at 1.001 across a 32× range of `n` can only mean `n log n`, and one that
climbed would falsify the claim outright. Plots of the same measurements are
committed under [`plots/`](plots) and listed below.

---

## Time Complexity

**O(n log n)** for both variants, in the worst case and in every other case —
merge sort does not have a bad input the way quicksort does. The recursion always
splits evenly and the merge is always linear.

## Space Complexity

**O(n)** for both — one temporary array of `n` integers, reused at every level,
plus the recursion stack, which is O(log n) deep.

---

## Sample Input

The program takes no input.

```text
(none)
```

## Sample Output

Abridged — the full run prints 132 lines. See [`sample.txt`](sample.txt).

```text
=====================================================
 DEMONSTRATION ON A SMALL ARRAY
=====================================================
Input          : 38 27 43 3 9 82 10 1 55 20 7 64
Two-way   sort : 1 3 7 9 10 20 27 38 43 55 64 82
                 32 comparisons, 44 moves
Three-way sort : 1 3 7 9 10 20 27 38 43 55 64 82
                 38 comparisons, 28 moves

Both outputs sorted and identical : YES

ANSWER: the modified merge sort is still O(n log n) in the
worst case.  Splitting into thirds only changes the base of
the logarithm, and a change of base is a constant factor
(1 / log2 3 = 0.6309), which O-notation absorbs.

Random input
----------------------------------------------------------------------
         n    2-way comps    3-way comps    2-way moves    3-way moves
----------------------------------------------------------------------
      1000           8711          10781           9976           6316
      2000          19417          24048          21952          13836
      4000          42771          53232          47904          30290
      8000          93646         116719         103808          65838
     16000         203178         252697         223616         140410
     32000         438436         547036         479232         303674

Random input - counts divided by n log2(n)
----------------------------------------------------------------------
         n    2-way comps    3-way comps    2-way moves    3-way moves
----------------------------------------------------------------------
      1000          0.874          1.082          1.001          0.634
      2000          0.885          1.097          1.001          0.631
      4000          0.894          1.112          1.001          0.633
      8000          0.903          1.125          1.001          0.635
     16000          0.909          1.131          1.001          0.628
     32000          0.915          1.142          1.001          0.634

=====================================================
 CONCLUSION
=====================================================
At n = 32000 on random input:

  comparisons / (n log2 n)   2-way 0.915   3-way 1.142
  moves       / (n log2 n)   2-way 1.001   3-way 0.634
```

---

## Sample Explanation

**The move counts confirm the change-of-base argument to three decimal places.**

This is the strongest single result in the program. Dividing the move count by
`n log₂ n` gives:

| Variant | Measured ratio | Predicted |
|---------|---------------|-----------|
| Two-way | **1.001** | 1.0 — exactly `n` moves per level, `log₂ n` levels |
| Three-way | **0.634** | `1 / log₂ 3` = **0.6309** |

The three-way ratio is not merely "smaller" — it sits on the theoretical constant
`1/log₂3`. That number is the change of base itself, measured. It is stable at
0.628–0.635 across a 32× range of `n`, which is what a genuine constant factor
looks like as opposed to a slow-growing term.

**The comparison counts go the other way, as predicted.** 0.915 for two-way
against 1.142 for three-way at `n = 32000`. Three-way does about 25% more
comparisons because each output element costs 2 comparisons rather than 1, offset
by having 0.63× as many levels: `2 × 0.63 = 1.26`, and `0.915 × 1.26 = 1.15`,
within about 1% of the measured 1.142.

**Both ratios are flat, and flatness is the whole test.** If either variant were
secretly O(n²) or O(n log² n), dividing by `n log₂ n` would produce a column that
climbed steadily. Instead the two-way move column reads 1.001 at every single
size from 1000 to 32000. A drifting column would falsify the claim; a flat one
confirms it.

**The comparison ratio drifts slightly upward** — 0.874 to 0.915 for two-way.
This is not a failure of the O(n log n) claim. On random data the merge loop often
exhausts one run early, and the leftover elements are copied without any
comparison; how often that happens depends weakly on `n`. The lower-order effect
shrinks as `n` grows and never approaches the factor-of-2 drift that a wrong
complexity class would produce. Note that on reverse-sorted input the same column
reads 0.504–0.506 — dead flat — because that input has no early exhaustion at all.

**Input class barely matters.** Sorted and reverse-sorted inputs produce the same
move counts as random ones, because merge sort always splits at the midpoint and
always merges the full width regardless of what the data looks like. Only the
comparison counts shift, and only by a constant.

---

## Committed Artefacts

The program itself writes nothing to disk. The plots below were produced
separately from its tabulated output and committed alongside the source. Their
`n` range runs wider than the program's own sweep of 1000 … 32000; the quantities
plotted are the ones the program counts.

| File | Description |
|------|-------------|
| [`plots/1_growth_comparisons.png`](plots/1_growth_comparisons.png) | Raw comparison counts for both variants as `n` grows |
| [`plots/2_normalised_constants.png`](plots/2_normalised_constants.png) | Each count divided by `n log₂ n` — the flat columns that carry the claim |
| [`plots/3_base_of_logarithm.png`](plots/3_base_of_logarithm.png) | The measured three-way move ratio against `1/log₂3 = 0.6309` |
| [`plots/4_tradeoff.png`](plots/4_tradeoff.png) | Fewer moves against more comparisons — the two effects that nearly cancel |

## Build and Run

```bash
gcc -Wall -Wextra q2_merge_sort_variants.c -o q2 -lm   # -lm is required: log()
./q2
```

## Files

| File | Description |
|------|-------------|
| [`q2_merge_sort_variants.c`](q2_merge_sort_variants.c) | Solution source — both sorts, the recurrences and the measurement |
| [`sample.txt`](sample.txt) | Sample input and output |
