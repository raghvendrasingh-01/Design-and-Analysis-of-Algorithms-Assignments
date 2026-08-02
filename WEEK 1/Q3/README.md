# Q3 — Performance Analysis of Bubble Sort

> Compare a bubble sort that stops early once the array is sorted against one that
> always completes all `n−1` passes, by counting comparisons.

| | |
|---|---|
| **Source** | [`q3_bubble_sort_performance_analysis.c`](q3_bubble_sort_performance_analysis.c) |
| **Sample file** | [`sample.txt`](sample.txt) |
| **Input** | One integer `n` — array size, maximum 25 |
| **Build** | `gcc -Wall -Wextra q3_bubble_sort_performance_analysis.c -o q3` |
| **Writes** | `bubble_sort_analysis.csv` in the current directory |

---

## Problem Statement

Using C, implement two different versions of bubble sort for randomised data
sequences:

1. Bubble sort that terminates if the array is sorted before the `(n−1)`ᵗʰ pass.
2. Bubble sort that always completes the `(n−1)`ᵗʰ pass.

Plot the number of comparisons in both cases to analyse their efficiency.

---

## Approach

One random array is generated, then **copied twice** so both variants sort
identical data. Comparing them on different inputs would make the counts
meaningless.

**Version 1 — early termination.** Each pass tracks whether any swap occurred. If
a pass completes with no swaps, the array is already sorted and the outer loop
breaks immediately.

**Version 2 — full passes.** Structurally identical, minus the flag. It runs all
`n−1` passes unconditionally, even when the array was sorted after the first one.

Both increment a global counter on every element comparison. The comparison count
is the cost model here — a machine-independent measure that does not depend on
compiler flags or CPU speed. Results are written to CSV and drawn as an ASCII bar
chart.

---

## Time Complexity

| Version | Best case | Worst case | Comparisons (worst) |
|---------|-----------|------------|---------------------|
| Early termination | **Θ(n)** — one clean pass over sorted input | **O(n²)** | `n(n−1)/2` |
| Full passes | **Θ(n²)** — no input escapes early | **Θ(n²)** | exactly `n(n−1)/2`, always |

The early-termination version has a genuinely better *best case*: `n−1`
comparisons on already-sorted input. Its worst case is unchanged.

## Space Complexity

**Θ(1)** auxiliary for the sort itself — swaps happen in place through a single
temporary. The program as a whole holds three fixed arrays of size
`MAX = 25`, which is Θ(1) with respect to the input value.

---

## Sample Input

```text
12
```

## Sample Output

The array is generated with a time-seeded `rand()`, so your values and the
early-termination count will differ between runs.

```text
===== Bubble Sort Performance Analysis =====
Enter number of elements (max 25): 12

Random Array:
57 6 75 6 49 1 48 53 96 93 26 67

Comparisons (Early Termination) : 60
Comparisons (Full Pass)         : 66

CSV file 'bubble_sort_analysis.csv' created successfully.
Open it in Excel, LibreOffice or Google Sheets and insert a Line Chart.

Comparison Graph
------------------------------
Early Termination : ############ (60)
Full Pass         : ██████████████ (66)

Time Complexity:
Early Termination : Best O(n), Worst O(n^2)
Full Pass         : O(n^2)
Space Complexity  : O(1)

Conclusion:
The early termination version performs fewer comparisons
when the array becomes sorted before all passes.
The full-pass version always performs all passes,
making it less efficient.
```

---

## Sample Explanation

**The full-pass count is fully determined by `n`.** For `n = 12` it is always
`n(n−1)/2 = 12 × 11 / 2 = 66`, regardless of the data. That figure is the
program's fixed baseline.

**The early-termination count depends on the data.** On the run above it used
**60** comparisons: the array was already sorted after 7 passes, so pass 8
completed without a single swap and the loop broke there — skipping passes 9, 10
and 11 and the 6 comparisons they would have cost. The committed CSV records a
different run at **63**, which broke one pass later and saved only 3.

**A constant-factor win, not a complexity win.** The saving here is roughly 9 %,
and it is bounded by how early the array happens to become sorted. Both versions
remain Θ(n²) in the worst case: on reverse-sorted input every pass performs at
least one swap, the flag is never clean, and the early-stopping version does the
identical `n(n−1)/2` comparisons. Skipping trailing passes only pays off once the
data is already ordered — which is exactly why an early exit improves the best
case without changing the asymptotic bound.

## Committed Artefacts

| File | Description |
|------|-------------|
| [`bubble_sort_analysis.csv`](bubble_sort_analysis.csv) | Recorded run — 63 comparisons early-stop vs 66 full-pass |
| [`bubble_sort_analysis.svg`](bubble_sort_analysis.svg) | Plot of the comparison counts |

---

## Build and Run

```bash
gcc -Wall -Wextra q3_bubble_sort_performance_analysis.c -o q3
./q3
```

> **Input constraints.** `n` is not validated against `MAX = 25`; keep it in
> `1..25`. Larger values write past the end of the arrays.

> **Heads up.** The program writes `bubble_sort_analysis.csv` into the *current
> working directory* under a fixed name. Running it from inside `Q3/` overwrites
> the committed copy. To keep it intact, compile from `Q3/` but place the binary
> in a scratch directory and run it there:
>
> ```bash
> mkdir -p /tmp/daa
> gcc -Wall -Wextra q3_bubble_sort_performance_analysis.c -o /tmp/daa/q3
> cd /tmp/daa && ./q3
> ```

## Files

| File | Description |
|------|-------------|
| [`q3_bubble_sort_performance_analysis.c`](q3_bubble_sort_performance_analysis.c) | Solution source |
| [`sample.txt`](sample.txt) | Sample input and output |
| [`bubble_sort_analysis.csv`](bubble_sort_analysis.csv) | Measurements from a recorded run |
| [`bubble_sort_analysis.svg`](bubble_sort_analysis.svg) | Plot of those measurements |
