# Q5 - Hitting a Moving Target

The program stores the set of positions where the unseen target may still be.
After shooting position `s`, that position is removed; every remaining position
then moves to an adjacent spot. A BFS searches over these possible-position
sets and reaches the empty set when a guaranteed strategy exists.

For six spots, one shortest strategy is:

```text
2 3 4 5 5 4 3 2
```

The approach is also a proof procedure: reaching the empty set means every
possible target path has been hit. There are `2^n` sets, `n` possible shots, and
each transition scans the `n` positions, so the worst-case time and space are
`O(n^2 2^n)` and `O(2^n)` respectively.

Build and run:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q5_moving_target.c -o q5
./q5
```

### Correctness and complexity analysis

At any point, a bit in the state mask means that the target could still be at
that spot. Shooting `s` removes `s`, because a target there would have been
hit. Every remaining position is then mapped to its adjacent positions, which
is exactly the target's allowed movement. Thus the transition computes all and
only the states possible after a missed shot. Reaching mask zero proves that
no target path remains, so the strategy guarantees a hit. BFS also proves the
reported strategy uses the fewest shots.

There are `2^n` possible masks and `n` shots from each mask. Building one next
mask scans `n` positions, giving `O(n^2 2^n)` time in this implementation. The
visited, parent, shot, and queue arrays require `O(2^n)` space. The strategy
itself is reconstructed in `O(n)` additional space.
