# Q6 - Reorganise String with K-Distance Apart

The program reads a string `S` and a non-negative integer `K`, then uses a
greedy max-heap to place the most frequent currently legal character. A
cooldown queue holds a character until `K` positions have passed. If the heap
becomes empty before all characters are placed, the requested arrangement is
impossible and the program reports it instead of printing a partial answer.
`K = 0` and `K = 1` therefore preserve the input multiset without imposing an
effective separation constraint.

## Correctness

At each output position, the heap contains exactly the characters whose last
use is at least `K` positions away (or which have not been used). Choosing a
maximum-count legal character is the standard greedy choice: it leaves the
largest remaining demand available for future slots. The cooldown queue then
prevents that character from being selected too early. If no legal character
exists, every remaining character is cooling down, so no completion is
possible from this prefix. Consequently a completed output is valid, and an
early failure is a correct impossibility result for this greedy construction.

## Complexity

There are at most 256 byte characters. With `n = |S|`, the running time is
`O(n log 256)` and the extra space is `O(n + 256)` (the cooldown queue and
heap). The input and output strings use `O(n)` space.

## Build and run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q6_reorganize_k_distance.c -o q6
./q6
```

Enter the string and then `K`, one per prompt. Malformed or negative `K`
values are rejected. `sample.txt` is captured from an actual run.
