/* q4_strassen_matrix_multiply.c
 *
 * DAA Q4 : multiply two n x n matrices by Strassen's method - seven
 * half-size products in place of eight.  Scalar multiplications and
 * additions are counted separately and swept over n, and every product is
 * checked against the naive triple loop before its counts are believed.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NNS 7

long long mults = 0, adds = 0;      /* scalar operations, counted apart */

static int nValues[NNS] = {2, 4, 8, 16, 32, 64, 128};
static long long strMul[NNS], strAdd[NNS], navMul[NNS], navAdd[NNS];

int *mnew(int n) { return (int *)calloc((size_t)n * n, sizeof(int)); }

/* c = a + s*b on n x n blocks, with s either +1 or -1 */
void mAddSub(int *a, int *b, int *c, int n, int s) {
    for (int i = 0; i < n * n; i++) c[i] = a[i] + s * b[i];
    adds += (long long)n * n;
}

/* Copy the (r,c) quadrant of the n x n matrix s into the half-size d. */
void quadGet(int *s, int n, int r, int c, int *d) {
    int h = n / 2;
    for (int i = 0; i < h; i++)
        for (int j = 0; j < h; j++)
            d[i * h + j] = s[(i + r * h) * n + (j + c * h)];
}

/* Write the half-size d back into the (r,c) quadrant of s. */
void quadPut(int *d, int n, int r, int c, int *s) {
    int h = n / 2;
    for (int i = 0; i < h; i++)
        for (int j = 0; j < h; j++)
            s[(i + r * h) * n + (j + c * h)] = d[i * h + j];
}

/* The definition, straight from the textbook: n^3 multiplications. */
void naiveMul(int *a, int *b, int *c, int n, int count) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            int s = 0;
            for (int k = 0; k < n; k++) s += a[i * n + k] * b[k * n + j];
            c[i * n + j] = s;
        }
    if (count) {
        mults += (long long)n * n * n;
        adds  += (long long)n * n * (n - 1);
    }
}

/* Strassen.  Quadrants are indexed 0=11, 1=12, 2=21, 3=22. */
void strassen(int *a, int *b, int *c, int n) {
    int h = n / 2, i;
    int *A[4], *B[4], *M[7], *t1, *t2;
    if (n == 1) { c[0] = a[0] * b[0]; mults++; return; }

    t1 = mnew(h); t2 = mnew(h);
    for (i = 0; i < 4; i++) {
        A[i] = mnew(h); B[i] = mnew(h);
        quadGet(a, n, i / 2, i % 2, A[i]);
        quadGet(b, n, i / 2, i % 2, B[i]);
    }
    for (i = 0; i < 7; i++) M[i] = mnew(h);

    /* The seven products - this is the whole trick: eight would be the
     * obvious block algorithm, and seven is what beats n^3. */
    mAddSub(A[0], A[3], t1, h, +1); mAddSub(B[0], B[3], t2, h, +1);
    strassen(t1, t2, M[0], h);                  /* M1 = (A11+A22)(B11+B22) */
    mAddSub(A[2], A[3], t1, h, +1);
    strassen(t1, B[0], M[1], h);                /* M2 = (A21+A22) B11      */
    mAddSub(B[1], B[3], t2, h, -1);
    strassen(A[0], t2, M[2], h);                /* M3 = A11 (B12-B22)      */
    mAddSub(B[2], B[0], t2, h, -1);
    strassen(A[3], t2, M[3], h);                /* M4 = A22 (B21-B11)      */
    mAddSub(A[0], A[1], t1, h, +1);
    strassen(t1, B[3], M[4], h);                /* M5 = (A11+A12) B22      */
    mAddSub(A[2], A[0], t1, h, -1); mAddSub(B[0], B[1], t2, h, +1);
    strassen(t1, t2, M[5], h);                  /* M6 = (A21-A11)(B11+B12) */
    mAddSub(A[1], A[3], t1, h, -1); mAddSub(B[2], B[3], t2, h, +1);
    strassen(t1, t2, M[6], h);                  /* M7 = (A12-A22)(B21+B22) */

    /* C11 = M1+M4-M5+M7,  C12 = M3+M5,  C21 = M2+M4,  C22 = M1-M2+M3+M6 */
    mAddSub(M[0], M[3], t1, h, +1); mAddSub(t1, M[4], t2, h, -1);
    mAddSub(t2, M[6], t1, h, +1);   quadPut(t1, n, 0, 0, c);
    mAddSub(M[2], M[4], t1, h, +1); quadPut(t1, n, 0, 1, c);
    mAddSub(M[1], M[3], t1, h, +1); quadPut(t1, n, 1, 0, c);
    mAddSub(M[0], M[1], t1, h, -1); mAddSub(t1, M[2], t2, h, +1);
    mAddSub(t2, M[5], t1, h, +1);   quadPut(t1, n, 1, 1, c);

    for (i = 0; i < 4; i++) { free(A[i]); free(B[i]); }
    for (i = 0; i < 7; i++) free(M[i]);
    free(t1); free(t2);
}

void measure(int idx) {
    int n = nValues[idx], i;
    long long before;
    int *a = mnew(n), *b = mnew(n), *cs = mnew(n), *cn = mnew(n);
    for (i = 0; i < n * n; i++) { a[i] = rand() % 9 - 4; b[i] = rand() % 9 - 4; }

    before = mults; adds = 0;
    strassen(a, b, cs, n);
    strMul[idx] = mults - before; strAdd[idx] = adds;

    before = mults; adds = 0;
    naiveMul(a, b, cn, n, 1);
    navMul[idx] = mults - before; navAdd[idx] = adds;

    /* The counts mean nothing unless the two agree on every entry. */
    for (i = 0; i < n * n; i++)
        if (cs[i] != cn[i]) { printf("MISMATCH at n=%d\n", n); exit(1); }
    free(a); free(b); free(cs); free(cn);
}

int main(void) {
    int i;
    for (i = 0; i < NNS; i++) measure(i);

    printf("\n=====================================================\n");
    printf(" STRASSEN vs THE NAIVE TRIPLE LOOP\n");
    printf("=====================================================\n");
    printf("-----------------------------------------------------------------"
           "-------------\n");
    printf("%6s %11s %11s %8s %12s %12s %12s\n", "n", "Str mult", "n^3 mult",
           "ratio", "Str add", "Str total", "nav total");
    printf("-----------------------------------------------------------------"
           "-------------\n");
    for (i = 0; i < NNS; i++)
        printf("%6d %11lld %11lld %8.2f %12lld %12lld %12lld\n",
               nValues[i], strMul[i], navMul[i], (double)navMul[i] / strMul[i],
               strAdd[i], strMul[i] + strAdd[i], navMul[i] + navAdd[i]);

    printf("\nT(n) = 7T(n/2) + 18(n/2)^2  ->  multiplications = 7^log2(n) = "
           "n^%.3f\n", log(7.0) / log(2.0));
    printf("Strassen wins on multiplications at every size, but its 18 block\n");
    printf("additions per level cost more than the 4 they save until n is large.\n\n");
    return 0;
}
