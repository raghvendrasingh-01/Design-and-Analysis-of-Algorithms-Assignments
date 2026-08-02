# Q5 — Find the Partition Point

> Given an array of 0s followed by 1s, locate the exact index where the transition
> occurs.

| | |
|---|---|
| **Source** | [`q5_partition_point.c`](q5_partition_point.c) |
| **Sample file** | [`sample.txt`](sample.txt) |
| **Input** | `n`, then `n` values (0s followed by 1s) |
| **Build** | `gcc -Wall -Wextra q5_partition_point.c -o q5` |

---

## Problem Statement

Consider an array `A` with `n` elements containing a run of 0s followed by a run
of 1s. Implement a method to find the exact point of transition between them.

---

## Approach

The array is **sorted by construction** — every 0 precedes every 1 — so a linear
scan would waste that structure. The problem reduces to finding the *first
occurrence of 1*, which binary search answers in logarithmic time.

The search maintains a best-known answer:

- If `A[mid] == 1`, this is a candidate transition point. Record it, then continue
  searching **left** (`high = mid − 1`) in case an earlier 1 exists.
- If `A[mid] == 0`, the transition must lie to the right (`low = mid + 1`).

When the range collapses, `ans` holds the smallest index containing a 1, or `−1`
if no 1 was ever seen. `mid` is computed as `(low + high) / 2`.

The result is reported as one of three cases, so the two degenerate arrays are
called out rather than returned as a bare index:

| Result | Meaning |
|--------|---------|
| `−1` | No transition — the array is all 0s |
| `0` | Transition at index 0 — the array is all 1s |
| `k > 0` | The 0 → 1 boundary lies between index `k−1` and `k` |

---

## Time Complexity

**Θ(log n)** — the search interval halves on every iteration, so the loop runs at
most `⌊log₂ n⌋ + 1` times. For `n = 10` that is at most 4 iterations instead of a
10-element scan; for `n = 1,000,000` it is 20 instead of a million.

This is optimal for the comparison model: any algorithm must distinguish between
`n + 1` possible answers, requiring at least `log₂(n+1)` comparisons.

## Space Complexity

**Θ(1)** auxiliary — the search uses four integer variables (`low`, `high`, `ans`
and the loop-local `mid`) and iterates rather than recursing, so no stack grows
with `n`. The input array itself is a fixed `MAX = 100` entries.

---

## Sample Input

```text
10
0 0 0 0 0 1 1 1 1 1
```

## Sample Output

```text
===== Partition Point Finder =====
Enter number of elements: 10
Enter 10 elements (0s followed by 1s):
Array: 0 0 0 0 0 1 1 1 1 1

Partition Point = 5
Transition occurs between index 4 and 5.
0 -> 1 transition found successfully.

Time Complexity  : O(log n)
Space Complexity : O(1)

Conclusion:
Binary Search efficiently finds the first occurrence of 1,
which is the partition point between 0s and 1s.
```

### Additional Cases

Only the lines that differ are shown; the complexity lines and the conclusion
block are identical to the sample output above.

| Input array | Output | Reason |
|-------------|--------|--------|
| `0 0 0 0` | `No transition found.` / `The array contains only 0s.` | No 1 exists, so `ans` stays `−1` |
| `1 1 1 1` | `Transition at index 0.` / `The array contains only 1s.` | Every element is 1 |
| `0 1` | `Partition Point = 1` | Smallest non-trivial transition |

---

## Sample Explanation

The array is `[0, 0, 0, 0, 0, 1, 1, 1, 1, 1]` with indices `0 … 9`. The last 0
sits at index 4 and the first 1 at index 5, so the partition point is **5**.

Binary search reaches it in **3 element comparisons**, against up to 10 for a
linear scan:

| Iteration | low | high | mid | `A[mid]` | Action | ans |
|-----------|-----|------|-----|----------|--------|-----|
| 1 | 0 | 9 | 4 | 0 | go right → `low = 5` | −1 |
| 2 | 5 | 9 | 7 | 1 | record, go left → `high = 6` | 7 |
| 3 | 5 | 6 | 5 | 1 | record, go left → `high = 4` | 5 |
| — | 5 | 4 | — | — | `low > high`, loop ends | **5** |

Note that iteration 2 finds *a* 1 at index 7 but does not stop there — it keeps
searching left, which is what makes the result the *first* 1 rather than an
arbitrary one.

---

## Build and Run

```bash
gcc -Wall -Wextra q5_partition_point.c -o q5
./q5
```

Or pipe the sample input directly:

```bash
printf '10\n0 0 0 0 0 1 1 1 1 1\n' | ./q5
```

> **Input constraints.** `n` is not validated against `MAX = 100`; keep it in
> `1..100`. The array must genuinely be 0s followed by 1s — binary search relies
> on that ordering and gives no meaningful answer on unsorted input.

## Files

| File | Description |
|------|-------------|
| [`q5_partition_point.c`](q5_partition_point.c) | Solution source |
| [`sample.txt`](sample.txt) | Sample input and output |
