# Q3 - Convolution Using the FFT

The convolution of two vectors is the same as multiplying two polynomials whose
coefficients are the vector elements.

## Build and Run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q3_convolution_fft.c -o q3 -lm
./q3
```

Files:

- [`q3_convolution_fft.c`](q3_convolution_fft.c) - solution
- [`sample.txt`](sample.txt) - sample output

## Direct Method

The simple method multiplies every element of `A` with every element of `B`:

```c
result[i + j] += A[i] * B[j];
```

Its time complexity is `O(mn)`.

The two loops run once for every pair `(i, j)`, so there are exactly `m * n`
products. The output initialization adds `m + n - 1` assignments:

```text
T_direct(m,n) = mn + (m + n - 1) = O(mn).
```

## FFT Method

The program:

1. pads both vectors to a power-of-two length `L`;
2. computes the FFT of both vectors;
3. multiplies matching FFT values;
4. computes the inverse FFT; and
5. rounds the answers back to integers.

The FFT divides the vector into even and odd positions:

```text
T(L) = 2T(L/2) + O(L) = O(L log L)
```

Therefore convolution takes `O(L log L)`, which is `O(n log n)` when
`n >= m`.

More explicitly, the program performs two forward FFTs, one inverse FFT, and one
pointwise multiplication pass:

```text
T_fft(L) = 3 * O(L log L) + O(L) = O(L log L).
```

The padded length satisfies `m + n - 1 <= 2n - 1` because `m <= n`, and the next
power of two is less than twice that value. Therefore `L = O(n)`, giving
`T_fft(n) = O(n log n)` for the assignment's `n >= m` condition.

The recursion allocates even and odd arrays whose sizes form a geometric series
`L + L/2 + L/4 + ...`, so the peak extra space is `O(L)`.

## Validation

The FFT result is compared with the direct `O(mn)` result. The program stops
with an error if any coefficient differs.

The implementation uses floating-point complex numbers. Rounding is suitable for
the small integer examples used here, but very large inputs can need an exact
number-theoretic transform.
