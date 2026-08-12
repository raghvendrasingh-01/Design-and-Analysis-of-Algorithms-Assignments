# Q2 — Search the Defective Coin

> One coin among `n` may be lighter than the rest, or all may be genuine.
> Using only a balance scale, find the lighter coin (or report none) in
> `log₂ n + c` weighings.

| | |
|---|---|
| **Source** | [`q2_defective_coin.c`](q2_defective_coin.c) |
| **Sample** | [`sample.txt`](sample.txt) |
| **Input** | None — built-in sweep with exhaustive verification |
| **Build** | `gcc -Wall -Wextra q2_defective_coin.c -o q2` |
| **Output** | Printed to the terminal |

---

## Problem Statement

A worker dropped a possibly-defective coin into a pile of `n − 1` identical
coins. The defective coin, if it exists, is **lighter** — never heavier. Using
only a balance scale, determine which coin is lighter, or report that none is.

The balance scale reports one of three outcomes for two equal-sized groups:
left pan lighter, right pan lighter, or balanced. It never reports a number.

Required: a divide-and-conquer algorithm running in `log₂ n + c` weighings.

---

## The Algorithm — Split in Three, Not Two

The key insight: **a single weighing has three outcomes, not two.** Splitting
in half wastes information — the balanced case would be impossible and the
scale would only ever distinguish two states. Splitting in thirds uses all
three outcomes:

```
Split coins[lo .. lo+n) into three groups:
    A = first ⌈n/3⌉ coins
    B = next  ⌈n/3⌉ coins
    C = the rest

Weigh A against B (equal sizes, so the comparison is meaningful):
    A lighter  -> the coin is in A;  every coin in B is genuine
    B lighter  -> the coin is in B;  every coin in A is genuine
    balanced   -> A and B are both clean; the coin, if any, is in C
```

### The two pieces of state

Two extra facts are carried through the recursion, and both are needed for
correctness:

**1. `known` — is the coin certainly in this group?**

If a pan tipped, the coin is definitely inside that group. If we recursed on
the balanced case, we only know the coin *might* be in C. This matters at
`n = 1`: a group of one that we know is light **is** the answer, with no
further weighing. Without this flag, the base case cannot distinguish
"this is the coin" from "there is no coin", and reports a false negative.

**2. `ref` — a coin already proved genuine**

When a pan tips, every coin in the *heavier* pan is proved genuine. When both
balance, all of A and B are genuine. So a reference coin is always available
after the first weighing. It is needed for the case `n = 1, known = false`:
weighing that lone coin against a known-genuine one settles it in one step.

### Correctness of the "none" case

If every weighing balances all the way down to an empty group, no group ever
tipped, so every coin was in some balanced pan and proved genuine. The
algorithm returns −1 correctly.

---

## Complexity

Each weighing reduces the pile to at most `⌈n/3⌉`:

```
T(n) = T(n/3) + 1,   T(1) = 0 or 1
```

which solves to:

```
T(n) = ⌈log₃ n⌉ + 1
```

Since `log₃ n = log₂ n / log₂ 3 ≈ 0.63 log₂ n`:

```
T(n) = 0.63 log₂ n + 1  ≤  log₂ n + c    for c = 1
```

The required bound holds. Ternary splitting is in fact *better* than the
`log₂ n` asked for — the extra information in the third outcome buys a
constant-factor improvement, which is exactly the opposite of what happened
in Q1, where ternary splitting cost extra comparisons. Here the split is free:
one weighing gives three outcomes regardless.

---

## Sample Output

```
=====================================================
 WEIGHINGS TO FIND THE LIGHT COIN (or report none)
=====================================================
-------------------------------------------------------------
       n        avg      worst ceil(log3 n)    wc/log2 n
-------------------------------------------------------------
       3       1.44          2            1         1.00
       9       2.21          3            2         0.75
      27       3.12          4            3         0.80
      81       4.11          5            4         0.71
     243       5.10          6            5         0.75
     729       6.10          7            6         0.70
    2187       7.10          8            7         0.67

Every trial's answer was checked against the hidden position,
and every position was tried exhaustively at each size.
Worst case = ceil(log3 n) + 1 <= log2 n + c, so the bound holds.
```

### Reading the output

**The `worst` column is exactly `⌈log₃ n⌉ + 1` at every size** — 2, 3, 4, 5,
6, 7, 8 against 1, 2, 3, 4, 5, 6, 7. This is not an approximation, it is the
exact count. The `+1` is the final confirming weighing against a reference
coin.

**`wc/log2 n` falls from 1.00 to 0.67** and is heading for `1/log₂ 3 ≈ 0.63`.
Every entry is at or below 1.0, so the `log₂ n + c` bound is satisfied with
room to spare — the algorithm beats the requirement.

**The average is one below the worst** at every size (7.10 against 8 at
n = 2187). Most hiding positions are found one level before the deepest,
because the pile shrinks by a factor of 3 and only the last surviving group
needs the confirming weighing.

**Verification is exhaustive, not sampled.** At each size the program hides
the coin at *every* one of the `n` positions in turn, plus the no-coin case,
and checks the reported index against the hidden one. Any wrong answer aborts
the run. The counts are only printed because all `3 + 9 + … + 2187 = 3279`
placements were answered correctly.

**The scale is the only oracle.** `weigh()` is the sole function that reads
coin weights, it compares two equal-sized groups, and it returns only −1, 0
or +1. The search itself never sees a weight — this is enforced by
construction, so the weighing count is the honest cost.

---

## Committed Artefacts

The program itself writes nothing to disk. The plots below were generated by
[`../make_plots.py`](../make_plots.py), which compiles the source, runs it, and
parses the table above — so no figure can disagree with the measured counts.

| File | Description |
|------|-------------|
| [`plots/1_weighings_growth.png`](plots/1_weighings_growth.png) | Measured worst case and average against `log₂ n` and `⌈log₃ n⌉ + 1` |
| [`plots/2_bound_margin.png`](plots/2_bound_margin.png) | Worst case divided by `log₂ n`, staying at or below 1 and heading for `1/log₂ 3 = 0.631` |

---

## Files

| File | Description |
|------|-------------|
| [`q2_defective_coin.c`](q2_defective_coin.c) | Solution source |
| [`sample.txt`](sample.txt) | Sample build/run output |
| [`plots/`](plots/) | Figures generated from the measured table |
