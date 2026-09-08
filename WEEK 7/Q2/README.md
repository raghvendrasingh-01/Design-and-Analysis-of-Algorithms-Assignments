# Q2 - Super Egg Testing Experiment

`D(e, f)` is the minimum number of drops needed with `e` eggs and `f` floors.
Dropping from floor `x` gives two cases: the egg breaks and leaves
`D(e-1, x-1)`, or it survives and leaves `D(e, f-x)`. Therefore

```text
D(e,f) = 1 + min over x of max(D(e-1,x-1), D(e,f-x)).
```

The base cases are `D(1,f)=f` and `D(e,0)=0`. For two eggs and 100 floors the
program obtains `14` guaranteed drops.

### Correctness and complexity analysis

Consider the first drop at floor `x`. If the egg breaks, the highest safe floor
is below `x`, so there are `x-1` floors and `e-1` eggs left. If it survives, the
answer is above or equal to `x`, so there are `f-x` higher floors and `e` eggs
left. The worst case for this choice is the larger of the two subproblem costs;
adding the current drop gives the recurrence. Taking the minimum over every
possible `x` is optimal because every valid first action is considered.

There are `(E+1)(F+1)=Theta(EF)` table cells. For each cell with `f` floors,
the inner loop tests `f` candidate drop floors. Therefore the total work is
`sum_e sum_f O(f) = O(EF^2)`. The table stores `Theta(EF)` integers, giving
`Theta(EF)` space. The 2-egg/100-floor result is `14`.

Build and run:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q2_super_egg_testing.c -o q2
./q2
```

There are `E*F` DP states and up to `F` candidate floors per state, giving
`O(EF^2)` time and `O(EF)` space.
