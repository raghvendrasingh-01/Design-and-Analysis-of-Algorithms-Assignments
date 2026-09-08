# Q4 - Security Switches

Each configuration is an `n`-bit state. Bit position `0` is printed as the
leftmost switch and position `n-1` as the rightmost switch. This convention is
important: only position `n-1` may always be toggled. A move toggles one switch
only when the condition in the question is satisfied. The program uses
recursive iterative-deepening DFS: it tries all legal paths of length `0`, then
`1`, then `2`, and so on. Therefore the first solution found uses the minimum
number of moves.

Build and run:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q4_security_switches.c -o q4
./q4
```

For four switches the minimum is `10` moves.

### Correctness and complexity analysis

The legality test directly implements the rules for positions numbered from
left to right:

1. position `n-1`, the rightmost switch, is always toggleable;
2. position `i<n-1` requires position `i+1` to be on; and
3. positions `i+2...n-1`, if any, must all be off.

Each legal toggle creates one edge between two bitmask states. A depth-limited
DFS explores all paths up to the current limit. Iterative deepening increases
that limit one level at a time, so the first successful depth is optimal. The
`onPath` array prevents cycles within a trial path. The program then independently
validates every printed transition by
checking the legality condition and confirming that exactly the named bit was
toggled.

There are `V=2^n` configurations and at most `n` legal choices from any one.
The depth-limited DFS can inspect `O(n^d)` paths at depth `d`; iterative
deepening through the optimal depth `D` therefore takes `O(n^D)` time in the
general graph bound, with an additional `O(n)` scan in each legality test.
Since `D <= 2^n-1`, this is an exponential worst-case bound. The legal graph is
highly constrained for this puzzle, and the four-switch instance finishes after
10 moves.

The recursion stack and current path use `O(D)` space. The `onPath` array uses
`O(2^n)` space, so the total worst-case space is `O(2^n)`.
