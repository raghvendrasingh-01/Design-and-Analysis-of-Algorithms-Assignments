# Q6 — Element Uniqueness

> Check whether `n` given numbers are all distinct, and reason about the cost for
> sufficiently large `n`.

| | |
|---|---|
| **Source** | [`q6_element_uniqueness.c`](q6_element_uniqueness.c) |
| **Sample file** | [`sample.txt`](sample.txt) |
| **Input** | `n`, then `n` integers |
| **Build** | `gcc -Wall -Wextra q6_element_uniqueness.c -o q6` |

---

## Problem Statement

For `n` given random numbers, implement a method in C to check whether there are
any duplicates. What can you conclude about the method for a sufficiently large
value of `n`?

---

## Approach

Brute-force pairwise comparison. Every element is compared against every element
that follows it:

- The outer loop runs `i` from `0` to `n−2`.
- The inner loop runs `j` from `i+1` to `n−1`.
- If `A[i] == A[j]`, a duplicate exists and the function returns immediately.

Starting the inner loop at `i+1` rather than `0` matters: it avoids comparing an
element with itself (which would always report a false duplicate) and avoids
checking each pair twice. Every unordered pair is examined exactly once, giving
`n(n−1)/2` comparisons in the worst case rather than `n²`.

The **early return** means the scan stops at the first duplicate found rather than
completing the full sweep.

---

## Time Complexity

| Case | Cost | When |
|------|------|------|
| Best | **Θ(1)** | `A[0] == A[1]` — a duplicate in the very first comparison |
| Worst | **Θ(n²)** | All elements distinct — exactly `n(n−1)/2` comparisons |
| Average | **O(n²)** | |

The worst case is the *unique* case, which is the awkward part: proving that no
duplicate exists requires examining every pair, so the input that produces the
happiest answer is also the most expensive to verify.

## Space Complexity

**Θ(1)** auxiliary — only the loop indices are allocated; nothing is copied,
sorted, or hashed. The input array is a fixed `MAX = 100` entries.

---

## Sample Input

```text
6
10 25 3 47 8 19
```

## Sample Output

```text
===== Element Uniqueness Check =====
Enter number of elements: 6
Enter 6 numbers:

Array: 10 25 3 47 8 19

All elements are unique.

Time Complexity  : O(n^2)
Space Complexity : O(1)

Conclusion:
This method compares every pair of elements.
For sufficiently large values of n, it becomes slow because
the number of comparisons grows quadratically.
More efficient methods such as sorting or hashing can be used
for large datasets.
```

### Additional Case — Duplicate Present

**Input**

```text
6
10 25 3 10 8 19
```

**Output**

```text
Array: 10 25 3 10 8 19

Duplicate elements are present.
```

Only the lines that differ are shown; the complexity lines and the conclusion
block are identical to the sample output above.

---

## Sample Explanation

**First case — all unique.** No two of `10, 25, 3, 47, 8, 19` are equal, so the
algorithm must exhaust every pair before it can conclude uniqueness. That is
`n(n−1)/2 = 6 × 5 / 2 = 15` comparisons — the worst case for `n = 6`.

**Second case — duplicate found.** The value `10` appears at index 0 and index 3.
The scan finds it on the 3ʳᵈ comparison (`i = 0`, `j = 3`) and returns
immediately, skipping the remaining 12. Detecting a duplicate is often cheap;
proving there is none never is.

**Conclusion for large `n`.** The comparison count grows quadratically, so the
method does not scale:

| `n` | Comparisons `n(n−1)/2` |
|-----|------------------------|
| 10 | 45 |
| 100 | 4,950 |
| 1,000 | 499,500 |
| 10,000 | 49,995,000 |
| 100,000 | ~5 × 10⁹ |

Each 10× increase in `n` multiplies the work by roughly **100×**. At `n = 100,000`
the brute-force method needs about five billion comparisons — seconds to minutes
of CPU time for a question that should be near-instant.

**Better alternatives.** Two standard improvements trade space or ordering for
time:

| Method | Time | Space | Note |
|--------|------|-------|------|
| Brute force (this program) | O(n²) | Θ(1) | No extra memory |
| Sort, then scan adjacent pairs | O(n log n) | Θ(1) to Θ(n) | Duplicates become neighbours |
| Hash set | O(n) average | Θ(n) | Worst case still O(n²) on collisions |

Sorting is the usual choice when memory is tight, hashing when it is not. Both
make `n = 100,000` trivial where brute force does not.

---

## Build and Run

```bash
gcc -Wall -Wextra q6_element_uniqueness.c -o q6
./q6
```

Or pipe the sample input directly:

```bash
printf '6\n10 25 3 47 8 19\n' | ./q6
```

> **Input constraints.** `n` is not validated against `MAX = 100`; keep it in
> `1..100`.

## Files

| File | Description |
|------|-------------|
| [`q6_element_uniqueness.c`](q6_element_uniqueness.c) | Solution source |
| [`sample.txt`](sample.txt) | Sample input and output |
