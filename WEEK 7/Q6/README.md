# Q6 - The Best Time to Be Alive

Each scientist contributes a birth event `(+1)` and a death event `(-1)`. After
sorting all events by year, a sweep maintains the number alive. Events in the
same year are ordered with deaths first, as required by the question.

Build and run:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q6_scientists_alive.c -o q6
./q6
```

Sorting `2n` events takes `O(n log n)` time, and the sweep takes `O(n)`. The
event array uses `O(n)` extra space.

### Correctness and complexity analysis

Before the first event, the alive count is zero. Processing a birth increases
the count exactly when that scientist becomes alive; processing a death reduces
it exactly when the scientist stops being alive. Therefore, after every event,
the running count equals the number of scientists alive at that time. Whenever
the count exceeds the previous maximum, recording the event year records a time
at which the maximum is achieved. Sorting deaths before births in the same year
implements the stated convention that a death occurs first.

The event list has `2n` records. Comparison sorting takes `Theta(2n log(2n))`
which simplifies to `Theta(n log n)`. The final scan performs `2n` updates, or
`Theta(n)`, so sorting dominates. The event list occupies `Theta(n)` space;
the sweep itself uses only constant extra scalar storage.
