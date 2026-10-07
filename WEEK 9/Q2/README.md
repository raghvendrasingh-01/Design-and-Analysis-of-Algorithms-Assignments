# Q2 - Huffman Coding

Input is `n`, followed by `n` pairs consisting of a one-character symbol and a
positive integer frequency. Symbols must be distinct. The program constructs a
minimum-expected-length Huffman tree and prints its canonical codebook.

## Algorithm and correctness

A min-heap repeatedly removes the two least frequent roots, makes them the
children of a new root, and inserts that root. The Huffman greedy exchange
argument says that two least frequent symbols can be siblings at maximum
depth in an optimal tree; contracting them gives the same problem with their
combined frequency. Induction proves that the resulting tree minimizes the
weighted path length and hence expected code length.

The tree supplies each symbol's code length. Entries are then sorted by
`(length, symbol)` and assigned canonical codes: start at all zeroes, increment
the previous binary code, and append zeroes when the next length is larger.
Canonical assignment preserves the Huffman lengths, is prefix-free, and gives
a deterministic codebook independent of tree pointer layout. A one-symbol
alphabet is assigned code `0`.

## Complexity

Heap construction and the `n - 1` merges take `O(n log n)` time. Sorting the
code lengths and emitting codes also takes `O(n log n)` time; code-string work
is `O(nL)` in the worst case, where `L` is the maximum code length. The tree,
heap, lengths, and code strings use `O(n + nL)` space.

## Build and run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q2_huffman_coding.c -o q2
./q2 < input.txt
```

Malformed symbols, duplicate symbols, zero/overflowed frequencies, allocation
failures, and invalid code-length states are rejected. `sample.txt` contains
output from an actual run.
