# Q4 — Towers of Hanoi

> Simulate the puzzle, tabulate the number of moves for `n` discs, and match the
> count against the closed form.

| | |
|---|---|
| **Source** | [`q4_towers_of_hanoi.c`](q4_towers_of_hanoi.c) |
| **Sample file** | [`sample.txt`](sample.txt) |
| **Input** | None — the program sweeps `n = 1 … 25` |
| **Build** | `gcc -Wall -Wextra q4_towers_of_hanoi.c -o q4` |
| **Writes** | `toh.csv` in the current directory |

---

## Problem Statement

Simulate the solution to the Towers of Hanoi problem using C. Plot the total
number of moves required for solving the problem of `n` discs. What can you
conclude about the algorithm from the plot obtained?

---

## Approach

The classical recursive solution: to move `n` discs from a source peg to a
destination peg using an auxiliary peg,

1. move the top `n−1` discs from source to auxiliary,
2. move disc `n` from source to destination,
3. move the `n−1` discs from auxiliary to destination.

The recursion bottoms out at `n = 0`, which requires no moves. A global counter is
incremented at step 2, so the move total is **measured by the simulation** rather
than computed.

That measured count is then checked against the closed form of the recurrence
`T(n) = 2·T(n−1) + 1`, `T(0) = 0`, which solves to `T(n) = 2ⁿ − 1`. The program
implements this as the bit shift `(1ULL << n) - 1` and prints both columns side by
side for `n = 1 … 25`.

The full move list is printed only for `n = 3` and `n = 4` — at 25 discs there are
over 33 million moves. The results are written to CSV and drawn as an ASCII chart
whose bar length is the disc count, so the bars grow linearly while the move
counts grow exponentially.

---

## Time Complexity

**Θ(2ⁿ)** for a single simulation of `n` discs — the recursion tree is a complete
binary tree of depth `n`, making `2^(n+1) − 1` calls to produce `2ⁿ − 1` moves. No
algorithm can do better, since every one of those moves is *required*: the output
itself has exponential size.

The program sweeps `n = 1 … 25`, so the whole run costs
`Θ(Σ 2ⁱ) = Θ(2^26) ≈ 6.7 × 10⁷` moves. The `formula()` check is Θ(1).

## Space Complexity

**Θ(n)** — the recursion stack reaches depth `n`; no auxiliary data structure
scales with the input. The `moves[]` table adds a fixed 26 entries.

---

## Sample Input

The program takes no input.

```text
(none)
```

## Sample Output

Abridged — the full run also prints every move for `n = 3` and `n = 4` and a
25-row ASCII chart. This output is fully deterministic.

```text
===== Tower of Hanoi =====

===== 3 DISKS =====
Move disk 1 from A to C
Move disk 2 from A to B
Move disk 1 from C to B
Move disk 3 from A to C
Move disk 1 from B to A
Move disk 2 from B to C
Move disk 1 from A to C
Total Moves = 7

===== 4 DISKS =====
Move disk 1 from A to B
Move disk 2 from A to C
... (15 moves in total) ...
Total Moves = 15

---------------------------------------------
Disks   Simulated       Formula
---------------------------------------------
 1               1               1
 2               3               3
 3               7               7
 4              15              15
 5              31              31
10            1023            1023
15           32767           32767
20         1048575         1048575
25        33554431        33554431

CSV file 'toh.csv' created successfully.

=========== LOG SCALE ASCII GRAPH ===========
Disks   Moves         Graph
--------------------------------------------------------
 1            1   █
 2            3   ██
 3            7   ███
 4           15   ████
 5           31   █████
...
25     33554431   █████████████████████████

Time Complexity  : O(2^n)
Space Complexity : O(n)
```

---

## Sample Explanation

**The recurrence is exact.** The simulated and formula columns match for **all 25
rows**. Solving `T(n) = 2·T(n−1) + 1` by unrolling:

```text
T(n) = 2·T(n−1) + 1
     = 4·T(n−2) + 2 + 1
     = 8·T(n−3) + 4 + 2 + 1
     = 2ⁿ·T(0) + (2ⁿ⁻¹ + … + 2 + 1)
     = 2ⁿ − 1
```

**What the plot shows.** Each additional disc *doubles* the work: 3 discs need 7
moves, 4 need 15, 25 need 33,554,431. Plotted on a linear axis the curve is
visibly exponential; the ASCII chart makes this concrete, because bar length grows
by one block per disc while the move count doubles.

**The conclusion about the algorithm.** The exponential cost is not an artefact of
this implementation — it is a lower bound of the problem. Producing `2ⁿ − 1` moves
requires at least `2ⁿ − 1` steps, so no cleverer algorithm exists. This makes
Towers of Hanoi the standard example of a problem that is *intractable by nature*
rather than by poor design: 64 discs at one move per second would take roughly 585
billion years.

## Committed Artefacts

| File | Description |
|------|-------------|
| [`toh.csv`](toh.csv) | Move counts for `n = 1 … 25` |
| [`toh.svg`](toh.svg) | Line plot of moves against disc count |

---

## Build and Run

```bash
gcc -Wall -Wextra q4_towers_of_hanoi.c -o q4
./q4
```

> **Heads up.** The program writes `toh.csv` into the *current working directory*
> under a fixed name. Running it from inside `Q4/` overwrites the committed copy.
> To keep it intact, compile from `Q4/` but place the binary in a scratch
> directory and run it there:
>
> ```bash
> mkdir -p /tmp/daa
> gcc -Wall -Wextra q4_towers_of_hanoi.c -o /tmp/daa/q4
> cd /tmp/daa && ./q4
> ```

## Files

| File | Description |
|------|-------------|
| [`q4_towers_of_hanoi.c`](q4_towers_of_hanoi.c) | Solution source |
| [`sample.txt`](sample.txt) | Sample input and output |
| [`toh.csv`](toh.csv) | Move counts for `n = 1 … 25` |
| [`toh.svg`](toh.svg) | Plot of those measurements |
