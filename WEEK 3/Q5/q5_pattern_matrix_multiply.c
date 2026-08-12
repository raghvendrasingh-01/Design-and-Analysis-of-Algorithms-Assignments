/* q5_pattern_matrix_multiply.c
 *
 * DAA Q5 : multiply two n x n matrices (n = 2^k) whose quadrants satisfy
 * M = [[M1, M2], [M2, M1]] recursively, down to single elements.  Two
 * half-size products suffice instead of the naive eight, giving O(n^2).
 * Counts are swept over n and every product is checked against the naive
 * triple loop before the counts are believed.
 */

#include <stdio.h>
#include <stdlib.h>

#define NNS 8

long long mults = 0, adds = 0;

static int nValues[NNS] = {2, 4, 8, 16, 32, 64, 128, 256};
static long long dcMul[NNS], dcAdd[NNS], navMul[NNS];

int *mnew(int n) { return (int *)calloc((size_t)n * n, sizeof(int)); }

void blockAddSub(int *a, int *b, int *c, int h, int s) {
    for (int i = 0; i < h * h; i++) c[i] = a[i] + s * b[i];
    adds += (long long)h * h;
}

void quadGet(int *s, int n, int r, int c, int *d) {
    int h = n / 2;
    for (int i = 0; i < h; i++)
        for (int j = 0; j < h; j++)
            d[i*h+j] = s[(i+r*h)*n+(j+c*h)];
}

void quadPut(int *d, int n, int r, int c, int *s) {
    int h = n / 2;
    for (int i = 0; i < h; i++)
        for (int j = 0; j < h; j++)
            s[(i+r*h)*n+(j+c*h)] = d[i*h+j];
}

void naiveMul(int *a, int *b, int *c, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            int s = 0;
            for (int k = 0; k < n; k++) s += a[i*n+k] * b[k*n+j];
            c[i*n+j] = s;
        }
}

/* C = AB where A and B have the block form [[X,Y],[Y,X]].
 * C11 = X1X2 + Y1Y2, C12 = X1Y2 + Y1X2.
 * Using P = (X1+Y1)(X2+Y2), Q = (X1-Y1)(X2-Y2):
 *   C11 = (P+Q)/2,  C12 = (P-Q)/2.
 * Two half-size products, and (X1+/-Y1) and (X2+/-Y2) keep the block form
 * because addition/subtraction preserve the [[U,V],[V,U]] pattern. */
void patternMul(int *a, int *b, int *c, int n) {
    if (n == 1) { c[0] = a[0]*b[0]; mults++; return; }
    int h = n/2;
    int *A1=mnew(h), *A2=mnew(h), *B1=mnew(h), *B2=mnew(h);
    int *SA=mnew(h), *DA=mnew(h), *SB=mnew(h), *DB=mnew(h);
    int *P=mnew(h), *Q=mnew(h), *t=mnew(h);

    quadGet(a,n,0,0,A1); quadGet(a,n,0,1,A2);
    quadGet(b,n,0,0,B1); quadGet(b,n,0,1,B2);
    blockAddSub(A1,A2,SA,h,+1); blockAddSub(A1,A2,DA,h,-1);
    blockAddSub(B1,B2,SB,h,+1); blockAddSub(B1,B2,DB,h,-1);
    patternMul(SA,SB,P,h);
    patternMul(DA,DB,Q,h);
    /* C11 = (P+Q)/2 */
    blockAddSub(P,Q,t,h,+1);
    for (int i = 0; i < h*h; i++) t[i] /= 2;
    quadPut(t,n,0,0,c); quadPut(t,n,1,1,c);
    /* C12 = (P-Q)/2 */
    blockAddSub(P,Q,t,h,-1);
    for (int i = 0; i < h*h; i++) t[i] /= 2;
    quadPut(t,n,0,1,c); quadPut(t,n,1,0,c);

    free(A1);free(A2);free(B1);free(B2);free(SA);free(DA);free(SB);free(DB);
    free(P);free(Q);free(t);
}

void buildPattern(int *m, int n) {
    int *f = (int*)malloc(sizeof(int)*n);
    for (int i = 0; i < n; i++) f[i] = rand()%9 - 4;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) m[i*n+j] = f[i^j];
    free(f);
}

void checkForm(int *m, int n) {
    if (n <= 1) return;
    int h = n/2;
    for (int i = 0; i < h; i++)
        for (int j = 0; j < h; j++)
            if (m[i*n+j] != m[(i+h)*n+j+h] ||
                m[i*n+j+h] != m[(i+h)*n+j]) {
                printf("STRUCTURE BROKEN at n=%d\n", n); exit(1);
            }
}

void measure(int idx) {
    int n = nValues[idx], i;
    long long before;
    int *a = mnew(n), *b = mnew(n), *cd = mnew(n), *cn = mnew(n);
    buildPattern(a,n); buildPattern(b,n); checkForm(a,n); checkForm(b,n);
    before = mults; adds = 0;
    patternMul(a,b,cd,n);
    dcMul[idx] = mults-before; dcAdd[idx] = adds;
    navMul[idx] = (long long)n*n*n;
    naiveMul(a,b,cn,n);
    for (i = 0; i < n*n; i++)
        if (cd[i] != cn[i]) { printf("MISMATCH n=%d\n",n); exit(1); }
    checkForm(cd,n);
    free(a);free(b);free(cd);free(cn);
}

int main(void) {
    int i;
    for (i = 0; i < NNS; i++) measure(i);
    printf("\n=====================================================\n");
    printf(" PATTERN MATRICES: 2 PRODUCTS INSTEAD OF 8\n");
    printf("=====================================================\n");
    printf("---------------------------------------------------------------\n");
    printf("%6s %9s %10s %12s %10s %11s %11s\n",
           "n","D&C mul","D&C add","D&C total","n^3 mul","total/n^2","speedup");
    printf("---------------------------------------------------------------\n");
    for (i = 0; i < NNS; i++) {
        long long tot = dcMul[i]+dcAdd[i];
        printf("%6d %9lld %10lld %12lld %10lld %11.2f %11.1f\n",
               nValues[i], dcMul[i], dcAdd[i], tot, navMul[i],
               (double)tot/((double)nValues[i]*nValues[i]),
               (double)navMul[i]/tot);
    }
    printf("\nT(n) = 2T(n/2) + 6(n/2)^2.  a=2, b=2, log2(2)=1 < 2  ->  O(n^2).\n\n");
    return 0;
}
