# Q1 - Minimum Coin Change

Given coin denominations with unlimited supply, the program finds the minimum
number of coins needed to make a target amount. Input is three whitespace-separated
parts: `n`, `n` positive denominations, and the target amount. Duplicate
denominations are harmless; an empty denomination list is accepted.

## Algorithm and correctness

`minimum[a]` stores the minimum coins needed for amount `a`. Initially only
`minimum[0] = 0` is known. For every amount and denomination `c`, the recurrence
is `minimum[a] = min(minimum[a], minimum[a-c] + 1)` when `c <= a`. Every optimal
solution ends with one coin, so removing that coin gives a subproblem considered
by the recurrence. Induction on `a` therefore proves the reported value optimal;
an unreached table entry means no combination exists and is reported as `-1`.

## Complexity

Time is `O(nV)` and dynamic-programming space is `O(V)`, where `V` is the target.

## Build and run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q1_minimum_coin_change.c -o q1
./q1 < input.txt
```

The program rejects malformed, non-positive, or unrepresentable input and checks
allocation-size overflow before creating arrays.

### Sample input

```text
3
1 3 4
6
```

The sample output is in [`sample.txt`](sample.txt); the answer is 2 (`3 + 3`).
