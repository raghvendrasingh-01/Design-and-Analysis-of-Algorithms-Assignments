# Q1 - Invert the Coin Triangle

The program represents coin centers by integer lattice coordinates `(x,y)` with
`0 <= y <= x < n`. The inverted triangle is the reflection of this set,
followed by a translation. A coin already occupying a required final position
is shown as `O`; a position that must be vacated or newly occupied is shown as
`X`. Both diagrams use the same coordinate grid, which is important because the
optimal inverted triangle is translated relative to the original. Coordinate
labels and lists of vacated/new positions are printed so every mark can be
checked directly.

For `m = n - 1 = 3q + r`, the minimum is

```text
3q(q + 1)/2 + r(q + 1),  0 <= r < 3.
```

The example has side `4`, so the answer is `3`. The exhaustive lattice check in
the program validates the formula for that example. The formula is obtained by
grouping the `m` gaps between successive rows into groups of three. A complete
group contributes `1 + 2 + ... + (q+1)` moved positions in the three lattice
directions, while the remainder contributes `q+1` for each leftover gap. Hence
`3q(q+1)/2 + r(q+1)`.

Correctness follows from overlap: every coin that remains in the same lattice
position needs no move, while every final position not in the original set must
be filled by a moved coin. Thus the number of moves is exactly
`N - |old ∩ translated-reflection|`. The best translation maximizes this
overlap, and the lattice enumeration checks all translations in the bounded
range needed for a triangle of side `n`. In the display, `O` appears in both
diagrams at exactly those overlapping positions; all other occupied positions
are marked `X`.

### Complexity analysis

The closed-form formula uses a constant number of arithmetic operations, so its
time is `Theta(1)` and its extra space is `Theta(1)`. Printing the two diagrams
requires `Theta(n^2)` character operations because the triangle contains
`N=n(n+1)/2` positions.

For validation, there are `(2n+1)^2 = Theta(n^2)` candidate translations. For
one translation, the program examines `N=Theta(n^2)` final positions. Its
`has` function linearly scans up to `N` old points, making one translation
`Theta(n^4)` in this deliberately simple representation. Consequently the
complete validation is `Theta(n^6)` time and `Theta(n^2)` space for the point
array. Replacing `has` with a boolean lattice table would reduce validation to
`Theta(n^4)` time; the asymptotically constant-time formula is the actual
algorithm for arbitrary input sizes.

Build and run:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q1_invert_coin_triangle.c -o q1
./q1
```

The formula is `O(1)`. The small validation enumerates `O(n^2)` translations and
uses a linear membership scan, so its direct implementation is `O(n^4)`.
