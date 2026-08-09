# Q3 — Merging k Sorted Arrays

> Given `k` sorted arrays of `n` elements each, combine them into one sorted array
> of `kn` elements. Compare merging them in one at a time against merging them in
> pairs.

| | |
|---|---|
| **Source** | [`q3_merge_k_sorted_arrays.c`](q3_merge_k_sorted_arrays.c) |
| **Sample file** | [`sample.txt`](sample.txt) |
| **Input** | None — the `k` and `n` sweeps are built into the program |
| **Build** | `gcc -Wall -Wextra q3_merge_k_sorted_arrays.c -o q3 -lm` |
| **Output** | Printed to the terminal — the two sweeps and their normalised columns |

---

## Problem Statement

Suppose you are given `k` sorted arrays, each with `n` elements, and you want to
combine them into a single sorted array of `kn` elements.

**(a) Method 1.** Use the merge subroutine repeatedly: merge the first two arrays,
then merge the result with the third array, then with the fourth, and so on until
you merge in the `k`-th and final input array. Write a program to validate this
idea, and find the worst-case running time of this method.

**(b) Method 2.** First divide the `k` arrays into `k/2` pairs and use the merge
subroutine to combine each pair, resulting in `k/2` sorted arrays of length `2n`.
Repeat this step until there is only one length-`kn` sorted array. Write a program
to validate this idea. What is the running time as a function of `n` and `k`?

---

## The Answer

| Method | Running time |
|--------|-------------|
| **Method 1** — merge in one at a time | **O(n k²)** |
| **Method 2** — merge in pairs, round by round | **O(n k log k)** |

### Method 1 — why k²

The accumulated block grows every round. Round `i` merges a block of size `i·n`
against one fresh array of size `n`, so it touches `(i+1)·n` elements:

```
2n + 3n + 4n + ... + kn  =  n · (k(k+1)/2 − 1)  =  O(n k²)
```

The real cost is re-copying. An element of the first array is moved during round
1, again in round 2, and again in every round after that — `k−1` times in total.
Method 1 pays for the same elements over and over.

### Method 2 — why k log k

Each round merges disjoint pairs, so every one of the `kn` elements is copied
exactly **once** per round. Pairing halves the number of arrays, so after `log₂ k`
rounds only one array remains:

```
kn per round × log₂(k) rounds  =  O(n k log k)
```

This is exactly the merge tree of merge sort, applied to whole arrays instead of
to single elements — which is why the same `log k` depth argument works.

### The gap

The asymptotic ratio is `Θ(k / log k)`. Putting the leading constants back in —
method 1 costs about `n k² / 2` and method 2 about `n k log₂ k` — the speedup to
expect in practice is:

```
(n k² / 2) / (n k log₂ k)  =  k / (2 log₂ k)
```

At `k = 64` that predicts **5.3×**, and the program measures **5.41×**.

---

## Approach

**One shared merge subroutine.** Both methods call the same `mergeInto`, so the
comparison is between the two *strategies*, not between two different merge
implementations. This matters: the question is about how you schedule the merges,
and using one subroutine keeps that the only variable.

**Element moves as the cost model.** The counter increments once per element
written into an output array. Moves are the natural unit here because the
difference between the methods is precisely how many times each element gets
copied — method 1's whole problem is that it copies early elements repeatedly.

**Method 1 swaps buffers instead of copying back.** After merging into the scratch
buffer, the two pointers are exchanged rather than copying the result back into
the accumulator. Copying back would double the count and measure the
implementation rather than the algorithm.

**Method 2 handles an odd count.** When a round has an odd number of arrays, the
last one is carried forward to the next round untouched. The sweep uses powers of
two so this is not exercised at the measured sizes, but the code is correct
without that assumption.

**Two independent sweeps.** One varies `k` with `n` fixed at 200; the other varies
`n` with `k` fixed at 16. Two variables need two sweeps — one alone cannot
separate a `k²` term from a `kn` term.

**Setup is excluded from the count.** Building the input arrays and the initial
working copies are not counted; only merging work is. Otherwise the O(nk) setup
would pollute the measurement at small `k`.

**Both outputs are verified at every size** to be in ascending order.

**How the order of growth is shown.** Each sweep prints its move counts beside a
normalised column — `M1/(n k²)` and `M2/(nk log k)`. Those two columns are the
evidence: one settles at 0.5 and the other sits at exactly 1.0000, which pins each
method to its claimed formula far more tightly than two rising curves on a shared
axis ever could. Plots of the same measurements are committed under
[`plots/`](plots) and listed below.

---

## Time Complexity

| Method | Time | Space |
|--------|------|-------|
| Method 1 | **O(n k²)** | O(nk) — output plus one working buffer |
| Method 2 | **O(n k log k)** | O(nk) — the arrays live in one round at a time |

Both are linear in `n`. The entire difference between them is in `k`.

---

## Sample Input

The program takes no input.

```text
(none)
```

## Sample Output

Abridged — the full run prints 91 lines. See [`sample.txt`](sample.txt).

```text
=====================================================
 DEMONSTRATION : k = 4 arrays of n = 3 elements
=====================================================
Array 1 :   1   8  15
Array 2 :   2   9  20
Array 3 :   5   6  30
Array 4 :   3  11  12

Method 1 output : 1 2 3 5 6 8 9 11 12 15 20 30
                  27 element moves
Method 2 output : 1 2 3 5 6 8 9 11 12 15 20 30
                  24 element moves

Both sorted : YES

=====================================================
 VARYING k, WITH n FIXED AT 200
=====================================================
------------------------------------------------------------------------------
     k       M1 moves       M2 moves    speedup   M1/(n k^2) M2/(nk log k)
------------------------------------------------------------------------------
     2            400            400       1.00       0.5000       1.0000
     4           1800           1600       1.12       0.5625       1.0000
     8           7000           4800       1.46       0.5469       1.0000
    16          27000          12800       2.11       0.5273       1.0000
    32         105400          32000       3.29       0.5146       1.0000
    64         415800          76800       5.41       0.5076       1.0000

=====================================================
 VARYING n, WITH k FIXED AT 16
=====================================================
----------------------------------------------------------------
       n       M1 moves       M2 moves    speedup    predicted
----------------------------------------------------------------
     100          13500           6400       2.11         2.00
     200          27000          12800       2.11         2.00
     400          54000          25600       2.11         2.00
     800         108000          51200       2.11         2.00
    1600         216000         102400       2.11         2.00
```

---

## Sample Explanation

**Method 2's normalisation is exactly 1.0000 at every value of k.**

The `M2/(nk log k)` column reads `1.0000` for `k = 2, 4, 8, 16, 32, 64` — not
approximately, exactly. This is not luck: at these sizes `k` is a power of two, so
every round pairs up perfectly and moves precisely `kn` elements, `log₂ k` times.
The formula `n k log₂ k` is not an asymptotic approximation here, it is the exact
count. An exact hit across a 32× range of `k` is about as direct a confirmation as
a measurement can give.

**Method 1's normalisation converges to 0.5 from above.** The `M1/(n k²)` column
reads 0.5000, 0.5625, 0.5469, 0.5273, 0.5146, 0.5076. The predicted exact count is
`n(k(k+1)/2 − 1)`, so dividing by `n k²` gives `(k+1)/(2k) − 1/k²`, which tends to
**0.5** as `k` grows. The measured column is doing exactly that: the `+1` in
`k(k+1)/2` is a lower-order term that fades as `k` grows, which is why the ratio
starts high and settles rather than being flat from the start.

**At k = 2 the two methods are identical — 400 moves each.** They must be: with
only two arrays there is one merge to do and both methods do it. The speedup only
appears once there is a scheduling decision to make, and it grows steadily from
there: 1.00, 1.12, 1.46, 2.11, 3.29, 5.41. This is the `k/(2 log₂ k)` curve.

**The n sweep isolates the k dependence.** With `k` fixed at 16, doubling `n`
doubles both move counts exactly — 13500 → 27000 → 54000 → 108000 → 216000 — and
the speedup column sits at 2.11 for every value of `n`. Both methods are strictly
linear in `n`, so `n` cannot be responsible for the gap between them. Combined
with the `k` sweep, this pins the difference on `k` alone, which is what
distinguishes `O(nk²)` from `O(nk log k)` rather than merely showing one is
bigger.

**Measured 2.11 against predicted 2.00 at k = 16.** The prediction
`k/(2 log₂ k) = 16/8 = 2.00` uses the asymptotic form of method 1's cost; the
exact count `n(k(k+1)/2 − 1)` is slightly larger than `nk²/2` at finite `k`, which
accounts for the 5% excess. The gap narrows as `k` grows — at `k = 64` the
prediction is 5.3 against a measured 5.41, about 2%.

**The practical reading.** At `k = 64` method 1 moves 415,800 elements to method
2's 76,800 for the identical result. Both are "just merging", both use the same
merge subroutine, and the only difference is the order the merges are scheduled
in. That is the entire lesson of the question.

---

## Committed Artefacts

The program itself writes nothing to disk. The plots below were produced
separately from its tabulated output and committed alongside the source. Their
`k` and `n` ranges run wider than the program's own sweeps (`k` = 2 … 64 at
`n` = 200, and `n` = 100 … 1600 at `k` = 16); the quantities plotted are the ones
the program counts.

| File | Description |
|------|-------------|
| [`plots/1_growth_moves.png`](plots/1_growth_moves.png) | Element moves for both methods as `k` grows — `nk²` against `nk log k` |
| [`plots/2_normalised_constants.png`](plots/2_normalised_constants.png) | `M1/(n k²)` settling at 0.5 and `M2/(nk log k)` at 1.0 |
| [`plots/3_speedup.png`](plots/3_speedup.png) | Measured speedup against the predicted `k / (2 log₂ k)` |
| [`plots/4_sweep_n.png`](plots/4_sweep_n.png) | Both methods linear in `n`, so the speedup does not move |
| [`plots/5_merge_tree_shape.png`](plots/5_merge_tree_shape.png) | Why the schedule is the whole difference — a skewed chain against a balanced tree |

## Build and Run

```bash
gcc -Wall -Wextra q3_merge_k_sorted_arrays.c -o q3 -lm   # -lm is required: log()
./q3
```

## Files

| File | Description |
|------|-------------|
| [`q3_merge_k_sorted_arrays.c`](q3_merge_k_sorted_arrays.c) | Solution source — both methods, the analysis and the measurement |
| [`sample.txt`](sample.txt) | Sample input and output |
