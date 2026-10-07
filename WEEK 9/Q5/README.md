# Q5 - Candy Distribution

Input is `n` followed by `n` signed integer ratings. Each child receives at
least one candy, and a child with a higher rating than an immediate neighbour
must receive more candies than that neighbour.

## Algorithm and correctness

The left-to-right pass stores the minimum candies required by the increasing
run ending at each child: if `rating[i] > rating[i-1]`, it uses
`left[i-1] + 1`, otherwise it resets to one. A right-to-left pass computes
the analogous requirement for decreasing runs and assigns

```text
candies[i] = max(left_requirement[i], right_requirement[i]).
```

The left pass satisfies every left-neighbour constraint minimally; the right
pass satisfies every right-neighbour constraint minimally. Taking the maximum
preserves both sets of constraints. Any valid allocation must meet each of
these two one-sided lower bounds, so the vector is component-wise minimal and
its sum is globally minimal.

## Complexity

The two passes take `O(n)` time and the ratings, auxiliary left bounds, and
candy vector use `O(n)` space.

## Build and run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q5_candy_distribution.c -o q5
./q5 < input.txt
```

Malformed signed integers, allocation overflow, and arithmetic overflow are
rejected. `n = 0` is accepted and produces an empty vector with total zero.
`sample.txt` contains output from an actual run.
