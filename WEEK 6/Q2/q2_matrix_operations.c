#include <math.h>
#include <stdio.h>
#include <string.h>

#define MAX 10
#define POWER_STEPS 50

static void printMatrix(double a[MAX][MAX], int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) printf("%8.2f", a[i][j]);
        putchar('\n');
    }
}

static void addMatrices(double a[MAX][MAX], double b[MAX][MAX],
                        double result[MAX][MAX], int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            result[i][j] = a[i][j] + b[i][j];
}

static void multiplyMatrices(double a[MAX][MAX], double b[MAX][MAX],
                             double result[MAX][MAX], int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            result[i][j] = 0.0;
            for (int k = 0; k < n; k++)
                result[i][j] += a[i][k] * b[k][j];
        }
}

static int isZeroMatrix(double a[MAX][MAX], int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (a[i][j] != 0.0) return 0;
    return 1;
}

static int isSymmetric(double a[MAX][MAX], int n) {
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (a[i][j] != a[j][i]) return 0;
    return 1;
}

/* Gaussian elimination with partial pivoting. */
static double determinant(double a[MAX][MAX], int n) {
    double copy[MAX][MAX];
    memcpy(copy, a, sizeof(copy));
    double answer = 1.0;

    for (int column = 0; column < n; column++) {
        int pivot = column;
        for (int row = column + 1; row < n; row++)
            if (fabs(copy[row][column]) > fabs(copy[pivot][column]))
                pivot = row;

        if (fabs(copy[pivot][column]) < 1e-12) return 0.0;

        if (pivot != column) {
            for (int j = 0; j < n; j++) {
                double temp = copy[column][j];
                copy[column][j] = copy[pivot][j];
                copy[pivot][j] = temp;
            }
            answer = -answer;
        }

        answer *= copy[column][column];
        for (int row = column + 1; row < n; row++) {
            double factor = copy[row][column] / copy[column][column];
            for (int j = column + 1; j < n; j++)
                copy[row][j] -= factor * copy[column][j];
        }
    }
    return answer;
}

static void transposeInPlace(double a[MAX][MAX], int n) {
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++) {
            double temp = a[i][j];
            a[i][j] = a[j][i];
            a[j][i] = temp;
        }
}

/* Finds the dominant eigenvalue and eigenvector approximately. */
static double powerIteration(double a[MAX][MAX], int n, double vector[MAX]) {
    double next[MAX];
    for (int i = 0; i < n; i++) vector[i] = 1.0 / sqrt((double)n);

    for (int step = 0; step < POWER_STEPS; step++) {
        double norm = 0.0;
        for (int i = 0; i < n; i++) {
            next[i] = 0.0;
            for (int j = 0; j < n; j++) next[i] += a[i][j] * vector[j];
            norm += next[i] * next[i];
        }

        norm = sqrt(norm);
        if (norm == 0.0) return 0.0;
        for (int i = 0; i < n; i++) vector[i] = next[i] / norm;
    }

    double eigenvalue = 0.0;
    for (int i = 0; i < n; i++) {
        double rowProduct = 0.0;
        for (int j = 0; j < n; j++) rowProduct += a[i][j] * vector[j];
        eigenvalue += vector[i] * rowProduct;
    }
    return eigenvalue;
}

int main(void) {
    int n = 3;
    double a[MAX][MAX] = {
        {4, 1, 1},
        {1, 3, 0},
        {1, 0, 2}
    };
    double b[MAX][MAX] = {
        {1, 2, 3},
        {0, 1, 4},
        {5, 6, 0}
    };
    double zero[MAX][MAX] = {{0}};
    double result[MAX][MAX] = {{0}};
    double vector[MAX];

    printf("A + B\n");
    addMatrices(a, b, result, n);
    printMatrix(result, n);

    printf("\nA x B\n");
    multiplyMatrices(a, b, result, n);
    printMatrix(result, n);

    printf("\nZero matrix? %s\n", isZeroMatrix(zero, n) ? "Yes" : "No");
    printf("A symmetric? %s\n", isSymmetric(a, n) ? "Yes" : "No");
    printf("det(A) = %.2f\n", determinant(a, n));

    transposeInPlace(b, n);
    printf("\nTranspose of B\n");
    printMatrix(b, n);

    double eigenvalue = powerIteration(a, n, vector);
    printf("\nDominant eigenvalue of A: %.6f\n", eigenvalue);
    printf("Eigenvector: ");
    for (int i = 0; i < n; i++) printf("%.6f%s", vector[i], i + 1 == n ? "\n" : " ");

    printf("\nWorst-case time complexities\n");
    printf("Addition, zero test, symmetry test, transpose: O(n^2)\n");
    printf("Naive multiplication and determinant: O(n^3)\n");
    printf("Power iteration: O(k*n^2), where k is the number of steps\n");
    return 0;
}
