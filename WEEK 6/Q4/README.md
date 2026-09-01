# Q4 - Sorting a Permutation by Reversals

The only operation allowed to change the permutation is:

```text
reverse(p, i, j)
```

It reverses all values from index `i` to index `j`.

## Build and Run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q4_sort_by_reversals.c -o q4
./q4
```

Files:

- [`q4_sort_by_reversals.c`](q4_sort_by_reversals.c) - solution
- [`sample.txt`](sample.txt) - sample output

## Part A - At Most `n - 1` Reversals

For each position `i`:

1. find the smallest value in `p[i..n-1]`;
2. reverse the range that brings it to position `i`.

One position becomes correct after each pass, so at most `n - 1` reversals are
needed. Finding each minimum takes a scan, so the running time is `O(n^2)`.

The scan lengths are `n - 1`, `n - 2`, ..., `1`, so the number of comparisons is

```text
T(n) = (n - 1) + (n - 2) + ... + 1
     = n(n - 1)/2
     = O(n^2).
```

The number of actual reversal calls is at most `n - 1`, which is the separate
`O(n)` reversal-count result. If reversal length is charged, the total length can
be `O(n^2)`.

A breakpoint is an adjacent pair whose values do not differ by `1` in absolute
value. One reversal can repair at most two boundary breakpoints, so a permutation
with `b` breakpoints needs at least `ceil(b/2)` reversals.

## Part B - `O(n log^2 n)` Reversal Cost

The cost of a reversal is its length. Long selection-style reversals can therefore
cost `O(n^2)` in total.

The second method uses merge sort. During merging, adjacent blocks are rotated
using three reversals:

```text
reverse(left block)
reverse(right block)
reverse(both blocks)
```

A merge of `L` elements costs `O(L log L)`, and merge sort has `O(log n)`
levels. Therefore the total reversal cost is:

```text
O(n log^2 n)
```

Here is the derivation. At one recursion level of a merge, the rotated ranges are
disjoint and contain at most `L` elements. Since each block rotation uses three
reversals, that level costs at most a constant times `L`. A merge has at most
`log2 L` levels, so

```text
M(L) = O(L log L).
```

The complete sort satisfies

```text
S(n) = 2S(n/2) + M(n)
     = 2S(n/2) + O(n log n).
```

There are `log2 n` merge-sort levels, and each level handles a total of `n`
elements. Summing `n log n` across those levels gives

```text
S(n) = O(n log n) * O(log n) = O(n log^2 n).
```

This merge is the least elementary part of Week 6, but it is needed to satisfy
the cost bound requested by the question. The code separates it into small
helpers: `lowerBound`, `upperBound`, `rotate`, and `mergeByReversal`.

## Validation

Both methods sort the assignment example `[1, 4, 3, 2, 5]` and the program
checks that the final result is exactly `[1, 2, 3, 4, 5]`.
