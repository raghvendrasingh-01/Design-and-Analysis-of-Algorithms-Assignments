# Week 2 Code Compression — Design

**Date:** 2026-08-10
**Scope:** `WEEK 2` only. Week 1 is explicitly out of scope and must not be touched.

---

## Goal

Compress each Week 2 question into a single `.c` file under a line budget, without
weakening any of the evidence the programs currently produce.

| Question | Today | Target | Hard ceiling |
|---|---|---|---|
| Q1 Dictionary Operations | 8 files, 1476 lines | 1 file, ≤500 | 500 |
| Q2 Merge Sort Variants | 1 file, 382 lines | 1 file, ≤200 | 226 |
| Q3 Merging k Sorted Arrays | 1 file, 401 lines | 1 file, ≤200 | 240 |

Q1's budget is firm. Q2 and Q3 carry an agreed ceiling above the target because
their measurement coverage is not negotiable — see "Non-negotiables".

Then update every document that describes the old layout, and reconcile the
committed plots with what the code can actually compute.

---

## Non-negotiables

These constrain every decision below.

1. **No measurement is removed.** Q1 keeps 6 structures × 7 operations × 6 sizes
   = 42 measured cells, each measured and printed independently. Q2 keeps 3 input
   classes × 6 sizes × 4 counters, and both the raw and the normalised table. Q3
   keeps both sweeps, all 6 `k` values, all 5 `n` values, and every normalisation
   column.
2. **Output stays byte-identical.** The compressed programs must reproduce the
   captured baselines exactly: Q1 143 lines, Q2 132 lines, Q3 91 lines.
3. **Nothing is built or run inside the repository.** All compiling and running
   happens in the scratchpad under `/tmp`. (Week 2 is stdout-only — no source
   writes files — but the discipline holds regardless.)
4. **Line savings come from structure and style, never from deleting evidence.**

---

## Q1 — Dictionary Operations

### Current layout

| File | Lines |
|---|---|
| `dictionary.h` | 182 |
| `q1_dictionary_operations.c` | 217 |
| `unsorted_array.c` | 160 |
| `sorted_array.c` | 169 |
| `singly_unsorted.c` | 195 |
| `singly_sorted.c` | 192 |
| `doubly_unsorted.c` | 186 |
| `doubly_sorted.c` | 175 |
| **total** | **1476** |

The six implementations total 1077 lines. The four linked-list files alone are
748 lines (536 excluding comments and blanks) and are pairwise near-identical.
That duplication is the entire reason the budget looks unreachable, and removing
it is the entire reason it becomes reachable.

### Target architecture: one file, two families

`dictionary.h` is absorbed into the single `.c` file. The six representations are
served by two data structures rather than six.

**Array family (~90 lines).** One struct shared by both array rows:

```c
typedef struct { int *data; int n; int sorted; } Array;
```

The `sorted` flag selects, per operation:

| Operation | `sorted == 0` | `sorted == 1` |
|---|---|---|
| Search | linear scan | **real binary search** — the only O(log n) cell in the table |
| Insert | write at index `n` | binary-search the position, then shift |
| Delete | overwrite the hole with the last element | shift the tail down |
| Max / Min / Pred / Succ | scan | index arithmetic |

**List family (~110 lines).** One node type and one list type:

```c
typedef struct Node { int key; struct Node *prev, *next; } Node;
typedef struct { Node *head, *tail; int n; int sorted, dbl; } List;
```

All four linked-list rows fall out of the two flags with no special cases:

| Operation | Decided by |
|---|---|
| Search, Insert, Maximum, Minimum, Successor | `sorted` |
| Delete | `dbl` |
| Predecessor | `sorted && dbl` |

This mapping is not an artefact of the refactor — it is the actual shape of the
claim table, which is what makes the merge safe. Every one of the four rows still
gets its own `List` instance, its own worst-case construction, its own seven
measurements, and its own printed block.

**Shared-node caveat.** The merged node physically carries both `prev` and `next`
even for the two singly linked rows. The singly linked variants are then *defined
by refusing to read `prev`* — which is exactly what makes their Delete and
Predecessor O(n). Step counts are unaffected, because steps are counted in
traversals and comparisons, not in link writes.

Two things must say so explicitly:

- a comment at the `Node` definition, stating that the singly linked rows never
  read `prev` and that this is the source of their O(n) entries;
- one line in the Q1 README's Space Complexity section, noting that the shared
  node stores two pointers for all four list rows while the asymptotic claim
  (one pointer for singly, two for doubly) describes the representations rather
  than this file's `sizeof`.

A second node type is *not* introduced to preserve a literal `sizeof`. It would
cost roughly 80 lines to restate the same algorithms and would change no measured
number.

**Driver (~150 lines, from 217).** Unchanged in substance: the `claim[6][7]`
Big-O string table, `measured[6][7][6]`, `printClaimTable`, `printMeasurements`,
`inferClass`, `printValidation`, and `main` with its per-structure progress dots
and its Time/Space Complexity footer. `measureOne`'s six-way `if`/`else` chain
becomes a small dispatch over `(family, flags)` triples.

**Estimate: ≈455 lines**, against a 500 budget.

### What must still be true afterwards

- `Agreement: 42 of 42 claims reproduced by measurement.`
- Sorted-array Search reads 7, 8, 9, 10, 11, 12 across n = 100 … 3200.
- Sorted-array Insert reads 106 … 3211 (shifts plus binary-search probes).
- Every O(1) cell reads exactly 1.
- Total output: 143 lines, byte-identical to the baseline.

---

## Q2 and Q3 — dense style only

Neither program changes what it computes, in any respect. Savings come from:

- same-line opening braces, one-line `if` bodies, merged closing braces;
- collapsing near-duplicate printers into one table-driven routine:
  - **Q2:** `printTable(cls, normalise)` is already called six times from `main`
    and already branches internally; the two branches and the six call sites fold
    into one loop.
  - **Q3:** `printSweepK` and `printSweepN` are near-identical; they become one
    routine parameterised by the axis being swept, its values, and its column set.

Everything else is preserved verbatim: Q2's `merge2`/`mergeSort2`/`merge3`/
`mergeSort3` (including the `n == 2` base case that guarantees `third >= 1` and
stops the recursion looping), all three `buildInput` classes with `srand(42)`,
the 12-element demo array, the recurrence exposition and the printed
`1/log2 3 = 0.6309`; Q3's `mergeInto`/`method1`/`method2` (including method 1's
buffer swap and method 2's odd-one-out carry), the `srand(42)` array builder, the
k=4/n=3 demo, the theory section and the printed `k/(2 log2 k)` prediction, and
the `isSorted` check after every single run.

The exact landed line count for each file is reported once it is measured. If a
file lands above its ceiling, that is a finding to report, not a licence to cut a
measurement.

---

## Plots

15 of the 18 committed PNGs are faithful — most run a wider range of `n` or `k`
than the C does, but compute the same quantities. 3 measure things the committed
code cannot produce.

**Remove (3):**

| File | Reason |
|---|---|
| `WEEK 2/Q2/plots/4_input_classes.png` | Plots 4 input classes including a "Worst" class; the code defines 3. |
| `WEEK 2/Q2/plots/6_k_way_constant.png` | Plots a tournament merge; the code implements only 2-way and 3-way. |
| `WEEK 2/Q3/plots/4_input_classes.png` | Plots *comparisons* over Interleaved/Random/Blocked input. Q3 counts only `moves` and builds random arrays exclusively. |

**Keep (15):** all 6 Q1 plots; Q2 `1_growth_comparisons`, `2_normalised_constants`,
`3_base_of_logarithm`, `5_tradeoff`; Q3 `1_growth_moves`,
`2_normalised_constants`, `3_speedup`, `5_sweep_n`, `6_merge_tree_shape`.

No README currently references any plot. Each of the three question READMEs gains
a **Plots** section listing its surviving PNGs and what each one shows, and
stating plainly where a plot's range exceeds the committed program's sweep.

---

## Documentation updates

### `WEEK 2/Q1/README.md`

| Location | Change |
|---|---|
| Header table, Header + Sources rows | Collapse to a single Source row naming the one file |
| Header table, Build row | `gcc -Wall -Wextra q1_dictionary_operations.c -o q1` |
| "File Layout" section and its 8-row table | Rewrite to explain the two families and the flag mapping |
| "All seven `.c` files must be compiled together…" paragraph | Replace; there is no longer an `extern` counter or a shared header |
| Build and Run code block | Single-file `gcc` command |
| "Files" table (9 rows) | Reduce to the single source plus `sample.txt` and the plots |
| "the full run prints 143 lines" | Verify against the regenerated output; correct if it moved |
| Space Complexity section | Add the shared-node note |

### `WEEK 2/Q2/README.md` and `WEEK 2/Q3/README.md`

Filenames and build commands are unchanged, so only stale counts and claims need
editing, plus the new Plots section. Known reference points — Q2 lines 8, 11, 227,
231, 233, 235; Q3 lines 9, 12, 235, 239, 241, 243 — are re-checked against the
file at edit time rather than trusted blind.

### `README.md` (root)

| Location | Change |
|---|---|
| Week 2 directory tree (≈lines 102–126) | One source file per question. The tree currently omits the `plots/` directories for Week 2 entirely — add them, at the surviving counts (Q1 6, Q2 4, Q3 5) |
| Question table, Q1 row (≈line 158) | Replace the `[header]` and `[driver]` links with one `[code]` link |
| Dependencies prose (≈line 194) | **Pre-existing inaccuracy.** It reads "Week 1's plots are committed as images and Week 2 prints its measurements as tables". Week 2 has committed plots too. Correct it to say both weeks commit plots, and Week 2 additionally prints its measurements as tables. Left uncorrected, it directly contradicts the new Plots sections |
| Q1 build note (≈lines 263–264) | "**Week 2 Q1 is split across several files** and must be compiled together…" is void — replace |
| Build code block (≈lines 266–270) | `gcc -Wall -Wextra q1_dictionary_operations.c -o q1` instead of `*.c` |
| Q2/Q3 `-lm` note (≈lines 272–278) | Verify it still reads correctly |
| Determinism claim (≈line 280) | Holds, but only after the regenerated `sample.txt` files are byte-verified — that verification is what earns the sentence |

### `sample.txt` files

All three are regenerated from the new binaries. None of them mentions a
filename, a header, or `gcc`, so they need no prose surgery — only regeneration
and byte-verification.

---

## Verification protocol

Per question, in the `/tmp` scratchpad:

1. Compile with `-Wall -Wextra`. Zero warnings is a requirement, not a
   preference. Q2 and Q3 need `-lm`.
2. Run; require exit status 0.
3. Run a second time and `diff` the two outputs — confirms determinism survived.
4. `diff` against the captured baseline. **Any difference fails the change**, and
   is investigated rather than accepted. The baselines live at
   `/tmp/claude-1000/-home-raghvendra-singh-Design-and-Analysis-of-Algorithms-Assignments/27f79da0-f63a-2e4c-7da5-a3d164ee61d5/scratchpad/baseline/`
   as `q1.out` (143 lines), `q2.out` (132) and `q3.out` (91), each already
   confirmed reproducible by a second capture. If that directory has been cleared,
   the baselines are recaptured from the committed sources at `HEAD` *before* any
   source is modified — never after.
5. `wc -l` the source and record it against its budget.
6. Regenerate `sample.txt` from the verified output.

A per-question summary is reported at the end: landed line count, budget,
warnings, and baseline diff result.

---

## Out of scope

- Week 1, in its entirety.
- Any change to an algorithm, a counter, a size sweep, a seed, or a printed
  number.
- Regenerating or editing the plots. The 15 keepers are left exactly as they are;
  the 3 unfaithful ones are deleted, not redrawn. The script that produced them
  is not in the repository and is not reconstructed here.
- Reformatting or restructuring any README section that the compression does not
  invalidate.
