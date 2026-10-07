# Q4 - Minimum Cost to Connect Sticks

Input is a positive stick count `n` followed by `n` non-negative lengths.
Connecting lengths `x` and `y` costs `x + y` and produces that length.

## Algorithm and correctness

All lengths are inserted into a min-heap. At every step the two shortest
sticks are removed, merged, and the result is inserted again. The program
prints each merge and the accumulated cost.

This is the Huffman merge greedy. In an optimal merge tree, the two least
lengths can be exchanged into sibling leaves at maximum depth without
increasing cost. Contracting those siblings yields the same problem with
their sum, so induction proves that repeatedly merging the two shortest
sticks minimizes the total cost.

## Complexity

Heap construction takes `O(n log n)` with repeated insertion, and the `n - 1`
merge operations take `O(n log n)` time. The heap uses `O(n)` space.

## Build and run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q4_connect_sticks.c -o q4
./q4 < input.txt
```

Malformed input, an empty stick list, allocation overflow, and length or total
cost overflow are rejected. A single stick correctly has total cost zero.
`sample.txt` contains output from an actual run.
