# Q1 - Fractional Knapsack with Deterioration

Input is `n` (`0 <= n <= 9`), followed by `n` triples `value weight decay_rate`,
then capacity `W`. Values and capacity are non-negative, weights and decay
rates are positive; all numbers must be finite and representable.

## Interpretation

The statement defines density `value/weight - decay_rate*t`, but does not
specify how consuming a fraction advances time. This solution explicitly uses
**unit-time opportunities**: every item is assigned one slot `t = 0, 1, ...`,
and any chosen fraction is consumed instantaneously at that slot's start.
An unused opportunity still advances the clock. There are no release times;
the schedule may be any permutation of the items. The value of selected
weight `x` is `x * (value/weight - decay_rate*t)`. Selection is optional, so
non-positive-density items need not be consumed and capacity may remain unused.

This is one defensible discrete-time interpretation, not the only possible
model. If elapsed time instead equals selected weight or consumption duration,
it is a different optimization problem. In particular, sorting initial
density or decay rates alone is not a justified optimal rule here.

## Algorithm and correctness

The program enumerates every scheduling permutation. For each fixed schedule,
densities become constants: sort by effective density and take as much weight
as possible from positive-density items, ending with at most one partial item.
The fractional-knapsack exchange argument proves this allocation optimal for
that schedule: replacing lower-density selected weight by unused
higher-density weight cannot decrease value. Since every allowable schedule
is considered, the best of those optimal allocations is globally optimal for
the stated model. Ties keep the first input-index permutation and use input
index for equal-density choices. The exact search is intentionally bounded to
nine items; it does not claim an unproved polynomial-time greedy scheduler.

## Complexity

There are `n!` schedules, each needing `O(n log n)` sorting and `O(n)` scanning:
`O(n! * n log n)` time for `n >= 2` and `O(n)` auxiliary space, including
recursive search depth. Empty and one-item instances are constant-time.

## Build and run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q1_fractional_knapsack_decay.c -o q1
./q1 < input.txt
```

Malformed or non-finite numeric input, out-of-range counts, zero weights or
decay rates, negative capacity, and unsafe arithmetic magnitudes are rejected.
An empty item set or zero capacity has optimal value zero.

### Sample input

```text
4
100 10 0.5
60 20 0.2
120 30 1.0
80 10 0.1
40
```

[`sample.txt`](sample.txt) is captured from an actual run; optimal value is 253.
