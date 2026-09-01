# Q1 - Operations on an Unsorted Array

This program demonstrates nine common operations on an unsorted integer array.
It uses a small fixed array so every result can be checked by hand.

## Build and Run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q1_array_operations.c -o q1 -lm
./q1
```

Files:

- [`q1_array_operations.c`](q1_array_operations.c) - solution
- [`sample.txt`](sample.txt) - sample output

## Approach

| Operation | Simple method used | Worst-case time |
|---|---|---:|
| Maximum | Scan the array once | `O(n)` |
| Largest two | Keep the largest and second largest values | `O(n)` |
| Mean | Add all values and divide by `n` | `O(n)` |
| Median | Copy and bubble-sort the array | `O(n^2)` |
| Standard deviation | Compute the mean, then squared differences | `O(n)` |
| Mode | Sort, then count equal consecutive values | `O(n^2)` |
| Remove duplicates | Sort, then keep one copy of each value | `O(n^2)` |
| Reverse | Swap the first and last values, moving inward | `O(n)` |
| Partition | Move every value `>= pivot` to the front | `O(n)` |

Bubble sort is used for the sorting-based operations because it is short and easy
to understand. A faster sort could reduce those operations to `O(n log n)`.

## Complexity Derivation

- **Maximum:** the loop checks positions `1` through `n - 1`, so it performs
  `n - 1` comparisons: `T(n) = n - 1 = O(n)`.
- **Two largest:** after two values are initialized, each of the remaining
  `n - 2` values uses at most two comparisons. Thus
  `T(n) <= 1 + 2(n - 2) = O(n)`.
- **Mean:** the sum loop performs `n` additions and the final division is
  constant time: `T(n) = n + 1 = O(n)`.
- **Standard deviation:** computing the mean takes `O(n)`, and the second loop
  takes `n` more iterations. Therefore `T(n) = O(n) + O(n) = O(n)`.
- **Median:** bubble sort uses
  `sum(i = 1 to n - 1) (n - i) = n(n - 1)/2 = O(n^2)` comparisons. Reading
  the middle value afterward is `O(1)`, so the total remains `O(n^2)`.
- **Mode:** sorting costs `O(n^2)` and counting equal runs costs `n - 1` steps:
  `T(n) = O(n^2) + O(n) = O(n^2)`.
- **Remove duplicates:** it has the same sort plus one scan, so
  `T(n) = O(n^2) + O(n) = O(n^2)`.
- **Reverse:** the two-pointer loop runs `floor(n/2)` times. Hence
  `T(n) = floor(n/2) = O(n)`.
- **Partition:** every element is examined once, so `T(n) = n = O(n)`.

The sorting-based methods allocate a copy of the array, using `O(n)` extra
space. The scan, reverse, and partition methods use `O(1)` extra space.

For an even number of elements, the median is the average of the two middle
values. The sample array therefore has median `27`.

The second-largest result means the second position in descending order.
Duplicate maximum values are allowed.

## Input

The example array is declared inside `main`:

```c
int a[] = {31, 7, 64, 12, 7, 89, 45, 7, 23, 56};
```

Change this array to test other values. Keep at least two elements because the
largest-two operation needs two positions.
