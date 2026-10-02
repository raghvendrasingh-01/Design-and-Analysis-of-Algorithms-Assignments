# Q2 - Coin Change: Total Number of Ways

The program counts distinct combinations of an unlimited supply of distinct
positive denominations. Coin order does not matter. Input is `n`, `n` distinct
positive denominations, and the target amount, separated by whitespace.

## Algorithm and correctness

`ways[a]` is initialized with `ways[0] = 1`, representing the empty
combination. Processing denominations one at a time, the ascending update
`ways[a] += ways[a-c]` appends the current coin to every combination already
counted. Because a denomination is processed only once and updates are
ascending, each combination is counted exactly once, in order of its largest
processed coin. Thus `ways[V]` is precisely the number of order-independent
combinations.

## Complexity

Time is `O(nV)` and space is `O(V)`. Counts are stored in `uint64_t`; if the
exact answer would exceed `UINT64_MAX`, the program reports that it cannot
represent the result instead of silently wrapping.

## Build and run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q2_coin_change_ways.c -o q2
./q2 < input.txt
```

Malformed input, non-positive values, duplicate denominations, and allocation
size overflow are rejected.

### Sample input

```text
3
1 2 5
5
```

The sample output is in [`sample.txt`](sample.txt); the four combinations are
`5`, `2+2+1`, `2+1+1+1`, and `1+1+1+1+1`.
