# Q6 — Selection Sort and Loop Invariants

> Sort an array by repeatedly finding the smallest element of the unsorted
> suffix and swapping it into place.  The loop invariant is asserted at
> run time, and comparisons are counted across sorted, reverse, and random
> inputs to confirm they are all `Θ(n²)`.

| | |
|---|---|
| **Source** | [`q6_selection_sort_invariant.c`](q6_selection_sort_invariant.c) |
| **Sample** | [`sample.txt`](sample.txt) |
| **Input** | None — built-in sweep over n with three input patterns |
| **Build** | `gcc -Wall -Wextra q6_selection_sort_invariant.c -o q6` |
| **Output** | Printed to the terminal |

---

## Problem Statement

Implement the selection sort algorithm and prove its correctness by
establishing a loop invariant.  Validate your analysis in C.

---

## The Loop Invariant

**Invariant:** At the start of each pass `i` (for `i = 0, 1, …, n−2`),
the subarray `A[0..i)` contains the `i` smallest elements of the original
array, in sorted order.

| Step | What it means |
|------|--------------|
| **Initialisation** | `i = 0`: the prefix `A[0..0)` is empty — the invariant is vacuously true. |
| **Maintenance** | Pass `i` scans the suffix `A[i..n)`, finds the minimum, swaps it to `A[i]`. After the swap, `A[0..i+1)` contains the `i+1` smallest elements in order. |
| **Termination** | `i = n−1`: the prefix `A[0..n−1)` contains `n−1` smallest elements in order, so `A[n−1]` must be the largest — the array is sorted. |

The program asserts the invariant at the top of every iteration and at
termination, so the proof is checked at run time rather than only argued
on paper.

---

## The Algorithm

```
SELECTION-SORT(A, n):
  for i = 0 to n − 2
      min ← i
      for j = i + 1 to n − 1
          if A[j] < A[min] then min ← j
      exchange A[i] with A[min]
```

The outer loop stops at `n−2`: once `A[0..n−1)` holds the `n−1` smallest
elements in order, the single element at `A[n−1]` must be the largest, so
there is nothing left to swap.

---

## Complexity

**Comparisons:** The inner loop runs `(n−1) + (n−2) + … + 1 = n(n−1)/2`
regardless of the input order.  There is no early exit — the loop must scan
the entire suffix to be certain it has found the minimum.  So:

```
Best case  = Θ(n²)
Worst case = Θ(n²)
Average    = Θ(n²)
```

All three are identical.  The only saving is the swap count, which is
exactly `n−1` — fixed and independent of the data.

---

## Sample Output

```
=====================================================
 SELECTION SORT: COMPARISONS DO NOT DEPEND ON THE INPUT
=====================================================
--------------------------------------------------------------------------
     n      sorted     reverse      random     n(n-1)/2    swaps    C/n^2
--------------------------------------------------------------------------
     8          28          28          28           28        7    0.438
    32         496         496         496          496       31    0.484
   128        8128        8128        8128         8128      127    0.496
   512      130816      130816      130816       130816      511    0.499
  1024      523776      523776      523776       523776     1023    0.500
  2048     2096128     2096128     2096128     2096128     2047    0.500

Invariant : A[0..i) holds the i smallest elements, sorted.
  init    : i = 0, the prefix is empty - vacuously true.
  maint   : pass i puts the smallest of A[i..n) at A[i], so the
            prefix grows by one and stays correct.
  term    : i = n-1, so A[0..n-1) is the n-1 smallest in order
            and A[n-1] must be the largest - the array is sorted.
```

### Reading the output

**The sorted, reverse, and random columns are identical at every size.** The
inner loop always scans the entire suffix regardless of what it finds — there
is no flag, no break, no early exit.  Whether the array is already sorted,
in reverse, or in random order, the scan visits `n−1−i` elements at pass `i`.
This is the key empirical observation that distinguishes selection sort from
insertion sort (which is `Θ(n)` on sorted input) and makes selection sort
the canonical example of "best case = worst case".

**The count equals `n(n−1)/2` exactly** — 28, 496, 8128, 130816, 523776,
2096128.  Not approximately.  Exactly.  Every entry matches the closed form
`n(n−1)/2` to the last digit, confirming that no comparison is ever skipped.

**`C/n²` converges to 0.500.** Dividing the exact count by `n²` gives
`n(n−1)/(2n²) = (n−1)/(2n)`, which tends to `1/2`.  The measured column reads
0.438, 0.484, 0.496, 0.499, 0.500, 0.500 — settling precisely onto the
predicted half, and approaching it from below because the `−1` matters at
small n and fades at large n.

**The swap count is exactly `n−1`** — 7, 31, 127, 511, 1023, 2047.  Every
swap places one element in its final position, and there are `n−1` elements
that need placing (the last one lands in place automatically).  This is the
one thing selection sort gets right about cost, and the reason its `Θ(n²)`
comparisons are considered especially wasteful.

---

## Committed Artefacts

The program itself writes nothing to disk. The plot below was generated by
[`../make_plots.py`](../make_plots.py), which compiles the source, runs it, and
parses the table above — so the figure cannot disagree with the measured counts.

One figure is enough here. The whole finding is that the three input patterns
give the *same* count, and a single log–log panel showing the three markers
sitting on top of one another on the `n(n−1)/2` line says it completely; a
second chart would only restate it.

| File | Description |
|------|-------------|
| [`plots/1_comparisons_identical.png`](plots/1_comparisons_identical.png) | Sorted, reverse and random counts coinciding on `n(n−1)/2`, with swaps on the `n − 1` line below |

---

## Files

| File | Description |
|------|-------------|
| [`q6_selection_sort_invariant.c`](q6_selection_sort_invariant.c) | Solution source |
| [`sample.txt`](sample.txt) | Sample build/run output |
| [`plots/`](plots/) | Figure generated from the measured table |
