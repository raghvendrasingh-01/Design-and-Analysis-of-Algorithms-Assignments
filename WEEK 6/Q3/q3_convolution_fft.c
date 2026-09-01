#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define PI 3.14159265358979323846

typedef double complex Complex;

/* Recursive divide-and-conquer FFT. */
static void fft(Complex a[], int n, int inverse) {
    if (n == 1) return;

    int half = n / 2;
    Complex *even = malloc(sizeof(Complex) * (size_t)half);
    Complex *odd = malloc(sizeof(Complex) * (size_t)half);
    if (!even || !odd) exit(EXIT_FAILURE);

    for (int i = 0; i < half; i++) {
        even[i] = a[2 * i];
        odd[i] = a[2 * i + 1];
    }

    fft(even, half, inverse);
    fft(odd, half, inverse);

    double sign = inverse ? 1.0 : -1.0;
    for (int k = 0; k < half; k++) {
        Complex root = cexp(sign * 2.0 * PI * I * k / n);
        Complex value = root * odd[k];
        a[k] = even[k] + value;
        a[k + half] = even[k] - value;
        if (inverse) {
            a[k] /= 2.0;
            a[k + half] /= 2.0;
        }
    }

    free(even);
    free(odd);
}

static int nextPowerOfTwo(int value) {
    int answer = 1;
    while (answer < value) answer *= 2;
    return answer;
}

static void convolutionFFT(const int a[], int m, const int b[], int n,
                           long long result[]) {
    int resultSize = m + n - 1;
    int size = nextPowerOfTwo(resultSize);
    Complex *fa = calloc((size_t)size, sizeof(Complex));
    Complex *fb = calloc((size_t)size, sizeof(Complex));
    if (!fa || !fb) exit(EXIT_FAILURE);

    for (int i = 0; i < m; i++) fa[i] = a[i];
    for (int i = 0; i < n; i++) fb[i] = b[i];

    fft(fa, size, 0);
    fft(fb, size, 0);
    for (int i = 0; i < size; i++) fa[i] *= fb[i];
    fft(fa, size, 1);

    for (int i = 0; i < resultSize; i++)
        result[i] = llround(creal(fa[i]));

    free(fa);
    free(fb);
}

/* Simple O(mn) version used only to check the FFT answer. */
static void convolutionDirect(const int a[], int m, const int b[], int n,
                              long long result[]) {
    int resultSize = m + n - 1;
    for (int i = 0; i < resultSize; i++) result[i] = 0;

    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            result[i + j] += (long long)a[i] * b[j];
}

int main(void) {
    int a[] = {1, 2, 3};
    int b[] = {4, 5, 6, 7};
    int m = (int)(sizeof(a) / sizeof(a[0]));
    int n = (int)(sizeof(b) / sizeof(b[0]));
    int resultSize = m + n - 1;
    long long fftResult[6], directResult[6];

    convolutionFFT(a, m, b, n, fftResult);
    convolutionDirect(a, m, b, n, directResult);

    printf("A: 1 2 3\n");
    printf("B: 4 5 6 7\n");
    printf("Convolution: ");
    for (int i = 0; i < resultSize; i++)
        printf("%lld%s", fftResult[i], i + 1 == resultSize ? "\n" : " ");

    for (int i = 0; i < resultSize; i++)
        if (fftResult[i] != directResult[i]) {
            printf("Validation failed\n");
            return 1;
        }

    printf("Validation: FFT result matches direct convolution\n");
    printf("\nComplexity\n");
    printf("Direct convolution: O(m*n)\n");
    printf("FFT convolution: O(L log L), where L is the padded size\n");
    return 0;
}
