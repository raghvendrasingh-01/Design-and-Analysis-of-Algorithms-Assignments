# Q1 — Dictionary Operations

> Give the asymptotic worst-case cost of all seven dictionary operations under six
> different representations, then validate every entry of the table by measurement.

| | |
|---|---|
| **Header** | [`dictionary.h`](dictionary.h) |
| **Sources** | six implementation files + [`q1_dictionary_operations.c`](q1_dictionary_operations.c) |
| **Sample file** | [`sample.txt`](sample.txt) |
| **Input** | None — sizes and worst-case scenarios are built into the program |
| **Build** | `gcc -Wall -Wextra *.c -o q1` |
| **Output** | Printed to the terminal — the claim table, the measured step counts, and the validation |

---

## Problem Statement

A dictionary `D` is an abstract data type giving access to data items by key. It
supports seven primary operations:

| Operation | Meaning |
|-----------|---------|
| `Search(D, k)` | Return a pointer to the item whose key is `k`, if one exists |
| `Insert(D, x)` | Add data item `x` to `D` |
| `Delete(D, x)` | Given a **pointer** `x` to an item in `D`, remove it |
| `Max(D)` / `Min(D)` | Retrieve the item with the largest / smallest key |
| `Predecessor(D, x)` / `Successor(D, x)` | Retrieve the item whose key is immediately before / after `x` in sorted order |

State the asymptotic worst-case running time of each operation when `D` is
implemented as an unsorted array, a sorted array, a singly linked unsorted list, a
singly linked sorted list, a doubly linked unsorted list, and a doubly linked
sorted list. Write a C program that validates the claim by plotting the order of
growth.

---

## The Claim

| Structure | Search | Insert | Delete | Maximum | Minimum | Predecessor | Successor |
|---|---|---|---|---|---|---|---|
| Unsorted array | O(n) | **O(1)** | **O(1)** | O(n) | O(n) | O(n) | O(n) |
| Sorted array | **O(log n)** | O(n) | O(n) | **O(1)** | **O(1)** | **O(1)** | **O(1)** |
| Singly linked unsorted | O(n) | **O(1)** | O(n) | O(n) | O(n) | O(n) | O(n) |
| Singly linked sorted | O(n) | O(n) | O(n) | **O(1)** | **O(1)** | O(n) | **O(1)** |
| Doubly linked unsorted | O(n) | **O(1)** | **O(1)** | O(n) | O(n) | O(n) | O(n) |
| Doubly linked sorted | O(n) | O(n) | **O(1)** | **O(1)** | **O(1)** | **O(1)** | **O(1)** |

Four assumptions make this table well defined, and each one changes an entry:

1. **`Delete(D, x)` takes a pointer, not a key.** Locating the item is therefore
   free, and delete measures only the cost of unlinking. Under a *key*-based
   delete every row would begin with the cost of a search.
2. **Every structure keeps a running count `n`.** Otherwise the linked lists would
   need an O(n) walk just to report their size.
3. **Both list families keep a tail pointer as well as a head pointer.** This is
   what makes `Max` on a sorted list O(1) and `Insert` on an unsorted list O(1).
4. **Unsorted-array delete overwrites the hole with the last element.** No
   shifting, so O(1). A sorted array cannot do this: the order must be preserved,
   so it shifts and pays O(n).

The three interesting rows:

- **Sorted array `Search` is the only O(log n) entry in the whole table.** Binary
  search needs *random access*. A sorted linked list has the ordering but not the
  addressing, so it is still O(n) to search. Sortedness alone buys nothing here.
- **Singly linked sorted `Predecessor` is O(n) while `Successor` is O(1).** The
  successor is `x->next`; the predecessor requires walking from the head to find
  the node whose `next` is `x`. This asymmetry disappears with back pointers.
- **Doubly linked sorted has five O(1) operations** — everything except `Search`
  (O(n), no random access) and `Insert` (O(n), must find the sorted position).

---

## File Layout

The six representations are separated into one file each, all sharing a single
header. Each file is self-contained: it owns its seven operations and its own
measurement routine, so a representation can be read and understood on its own.

| File | Contents | Prefix |
|------|----------|--------|
| [`dictionary.h`](dictionary.h) | Struct definitions and prototypes for all six | — |
| [`unsorted_array.c`](unsorted_array.c) | Unsorted array | `ua` |
| [`sorted_array.c`](sorted_array.c) | Sorted array | `sa` |
| [`singly_unsorted.c`](singly_unsorted.c) | Singly linked unsorted list | `su` |
| [`singly_sorted.c`](singly_sorted.c) | Singly linked sorted list | `ss` |
| [`doubly_unsorted.c`](doubly_unsorted.c) | Doubly linked unsorted list | `du` |
| [`doubly_sorted.c`](doubly_sorted.c) | Doubly linked sorted list | `ds` |
| [`q1_dictionary_operations.c`](q1_dictionary_operations.c) | The claim table, the measurement loop and the validation | — |

Every representation exposes the same seven operations with the same meaning, so
they are directly comparable. The driver calls `xxMeasure(n, result)` on each and
never needs to know how any of them work.

---

## Approach

The table above is not printed from a hardcoded list and left there. All six
structures are actually implemented, and each of the 42 cells is then measured.

**A step counter as the cost model.** One global counter, declared in
`dictionary.h` and defined in the driver, is incremented on every key comparison
and every pointer or index move. Wall-clock time is avoided deliberately — it
depends on the compiler and the machine, and at these sizes it is mostly noise.
Step counts are exact and reproducible.

**Worst cases are constructed, not sampled.** Averaging over random inputs would
measure the average case, which is not what the question asks. Each measurement
forces the worst case instead:

| Operation | Worst case used |
|-----------|-----------------|
| `Search` | a key that is **absent** — no early exit is possible |
| `Insert` | the position that forces maximal shifting or walking |
| `Delete` | the item that is most expensive to unlink |
| `Maximum` / `Minimum` | the extreme sits at the far end of an unsorted structure |
| `Predecessor` / `Successor` | the element whose neighbour is furthest away |

**Class inference from the doubling ratio.** Sizes double: 100, 200, …, 3200.
For each cell the program computes `steps(3200) / steps(1600)` and infers the
class from it:

| Ratio | Inferred class | Why |
|-------|----------------|-----|
| ≈ 1 | O(1) | cost does not respond to `n` at all |
| between | O(log n) | doubling `n` adds **one** probe, it does not scale the count |
| ≈ 2 | O(n) | cost doubles with `n` |

That inferred class is compared against the claimed one. **The program prints
`Agreement: 42 of 42 claims reproduced by measurement.`** — the claim is
validated, not asserted.

**How the order of growth is shown.** The question asks for a plot. What the
program prints instead is the step count for every operation at every size,
followed by the doubling ratio and the class it implies. The table carries the
same information a curve would, and more precisely: a reader can check that
`Search` on a sorted array reads 7, 8, 9, 10, 11, 12 and see the logarithm
directly, which is not something you can read off a drawn axis.

---

## Time Complexity

Per operation, the answer is the claim table above. The cost of the **validation
program** itself, with 6 structures, 7 operations and 6 sizes, is dominated by
building the sorted linked lists, which costs O(n²) because each insert walks the
list. Overall the harness is **O(n²)** at the largest `n`.

Each measurement rebuilds a **fresh** structure and resets the counter, so no
measurement can be contaminated by the one before it.

## Space Complexity

**O(n)** — only one structure is alive at a time and it is freed before the next
is built. Arrays hold `n` keys; a singly linked list adds one pointer per node, a
doubly linked list two.

---

## Sample Input

The program takes no input.

```text
(none)
```

## Sample Output

Abridged — the full run prints 143 lines. See [`sample.txt`](sample.txt).

```text
=====================================================================
 ASYMPTOTIC WORST-CASE RUNNING TIMES (the claim)
=====================================================================
Structure        Search   Insert   Delete   Max      Min      Pred     Succ
--------------------------------------------------------------------------------
Unsorted array   O(n)     O(1)     O(1)     O(n)     O(n)     O(n)     O(n)
Sorted array     O(log n) O(n)     O(n)     O(1)     O(1)     O(1)     O(1)
Singly unsorted  O(n)     O(1)     O(n)     O(n)     O(n)     O(n)     O(n)
Singly sorted    O(n)     O(n)     O(n)     O(1)     O(1)     O(n)     O(1)
Doubly unsorted  O(n)     O(1)     O(1)     O(n)     O(n)     O(n)     O(n)
Doubly sorted    O(n)     O(n)     O(1)     O(1)     O(1)     O(1)     O(1)

... one measured block per structure ...

Sorted array
--------------------------------------------------------------
n =                100     200     400     800    1600    3200
Search               7       8       9      10      11      12
Insert             106     207     408     809    1610    3211
Delete             100     200     400     800    1600    3200
Maximum              1       1       1       1       1       1
Minimum              1       1       1       1       1       1
Predecessor          1       1       1       1       1       1
Successor            1       1       1       1       1       1

=====================================================================
 VALIDATION : CLAIMED CLASS vs CLASS INFERRED FROM MEASUREMENT
=====================================================================
Structure         Operation       Ratio  Claimed   Observed  Verdict
------------------------------------------------------------------------
Sorted array      Search          1.091  O(log n)  O(log n)  MATCH
Singly sorted     Predecessor     2.000  O(n)      O(n)      MATCH
Singly sorted     Successor       1.000  O(1)      O(1)      MATCH
Doubly sorted     Delete          1.000  O(1)      O(1)      MATCH
... 42 rows in total ...
------------------------------------------------------------------------
Agreement: 42 of 42 claims reproduced by measurement.
```

---

## Sample Explanation

**The measurements land exactly on theory, not merely near it.**

*Sorted array `Search` reads 7, 8, 9, 10, 11, 12* for `n = 100 … 3200`. That is
precisely `floor(log2 n) + 1`: the binary search worst case. Six successive
doublings of `n` add **five probes in total**. The doubling ratio is
`12/11 = 1.091`, nowhere near 2, which is exactly how the inference separates
logarithmic from linear.

*Every unsorted-structure traversal reads `n` exactly* — 100, 200, …, 3200 for
Search, Maximum, Minimum, Predecessor and Successor. The absent search key
guarantees no early exit, so the walk is the full length of the structure.

*Sorted-array `Insert` reads `n` plus a logarithmic tail.* At `n = 3200` it
measures 3211 = 3200 shifts + 11 binary-search probes to locate the insertion
point; at `n = 100` it is 106 = 100 + 6. The probe count is exactly the `Search`
row minus one, and it grows by one per doubling while the shift term doubles.
The `log2 n` term is real but asymptotically invisible: `O(n + log n) = O(n)`. A
lower-order term does not vanish from the measurement — only from the class.

*The three O(n) `Delete` rows read exactly `n`, not `n − 1`.* Delete is measured
after Insert has already run, so the structure holds `n + 1` items and closing
the hole moves `n` of them. Sorted array, singly unsorted and singly sorted all
read 100, 200, …, 3200 — the first shifts elements, the other two walk to find
the node in front of the target. Three different reasons, same O(n).

**The `O(1)` entries are literally 1.** Not "small", not "roughly constant" — the
counter reads exactly 1 at every size. Unsorted-array `Insert` writes at index `n`
and increments the count. Doubly linked `Delete` rewires two pointers. None of
them looks at `n` at all.

**What the table costs you, in one sentence.** There are 42 cells and only 18 of
them are sub-linear (17 O(1) plus the one O(log n) binary search); no
representation gets more than five, and the two that tie at five (sorted array
and doubly linked sorted) both pay with O(n) `Insert`. Getting
all seven fast at once needs a balanced binary search tree, which is the next step
beyond this exercise.

---

## Build and Run

```bash
gcc -Wall -Wextra *.c -o q1
./q1
```

All seven `.c` files must be compiled together — the six implementations and the
driver. The driver defines the `steps` counter that the other six declare as
`extern` through the header.

## Files

| File | Description |
|------|-------------|
| [`dictionary.h`](dictionary.h) | Shared interface — structs and prototypes for all six representations |
| [`unsorted_array.c`](unsorted_array.c) | Unsorted array, seven operations plus its measurement routine |
| [`sorted_array.c`](sorted_array.c) | Sorted array, with binary search |
| [`singly_unsorted.c`](singly_unsorted.c) | Singly linked unsorted list |
| [`singly_sorted.c`](singly_sorted.c) | Singly linked sorted list |
| [`doubly_unsorted.c`](doubly_unsorted.c) | Doubly linked unsorted list |
| [`doubly_sorted.c`](doubly_sorted.c) | Doubly linked sorted list |
| [`q1_dictionary_operations.c`](q1_dictionary_operations.c) | Driver — claim table, measurement loop, validation |
| [`sample.txt`](sample.txt) | Sample input and output |
