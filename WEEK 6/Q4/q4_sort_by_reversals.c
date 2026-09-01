#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static long long reversalCount;
static long long reversalCost;

static void printArray(const int a[], int n) {
    for (int i = 0; i < n; i++) printf("%d%s", a[i], i + 1 == n ? "\n" : " ");
}

/* The only operation allowed to change the permutation. */
static void reverse(int a[], int left, int right) {
    if (left >= right) return;
    reversalCount++;
    reversalCost += right - left + 1;

    while (left < right) {
        int temp = a[left];
        a[left++] = a[right];
        a[right--] = temp;
    }
}

/* Part A: place i+1 at index i using at most one reversal per position. */
static void sortWithAtMostNReversals(int a[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int position = i;
        for (int j = i + 1; j < n; j++)
            if (a[j] < a[position]) position = j;
        reverse(a, i, position);
    }
}

/* Swaps adjacent blocks [left, middle) and [middle, right). */
static void rotate(int a[], int left, int middle, int right) {
    if (left == middle || middle == right) return;
    reverse(a, left, middle - 1);
    reverse(a, middle, right - 1);
    reverse(a, left, right - 1);
}

static int lowerBound(const int a[], int left, int right, int value) {
    while (left < right) {
        int middle = left + (right - left) / 2;
        if (a[middle] < value) left = middle + 1;
        else right = middle;
    }
    return left;
}

static int upperBound(const int a[], int left, int right, int value) {
    while (left < right) {
        int middle = left + (right - left) / 2;
        if (a[middle] <= value) left = middle + 1;
        else right = middle;
    }
    return left;
}

/* Merges two sorted adjacent parts using rotations made from reversals. */
static void mergeByReversal(int a[], int left, int middle, int right) {
    int leftSize = middle - left;
    int rightSize = right - middle;
    if (leftSize == 0 || rightSize == 0) return;

    if (leftSize + rightSize == 2) {
        if (a[middle] < a[left]) reverse(a, left, middle);
        return;
    }

    int leftCut, rightCut;
    if (leftSize > rightSize) {
        leftCut = left + leftSize / 2;
        rightCut = lowerBound(a, middle, right, a[leftCut]);
    } else {
        rightCut = middle + rightSize / 2;
        leftCut = upperBound(a, left, middle, a[rightCut]);
    }

    rotate(a, leftCut, middle, rightCut);
    int newMiddle = leftCut + (rightCut - middle);
    mergeByReversal(a, left, leftCut, newMiddle);
    mergeByReversal(a, newMiddle, rightCut, right);
}

static void mergeSortByReversal(int a[], int left, int right) {
    if (right - left < 2) return;
    int middle = left + (right - left) / 2;
    mergeSortByReversal(a, left, middle);
    mergeSortByReversal(a, middle, right);
    mergeByReversal(a, left, middle, right);
}

static int breakpoints(const int a[], int n) {
    int count = 0;
    for (int i = 0; i <= n; i++) {
        int current = i == 0 ? 0 : a[i - 1];
        int next = i == n ? n + 1 : a[i];
        if (abs(next - current) != 1) count++;
    }
    return count;
}

static int isSorted(const int a[], int n) {
    for (int i = 0; i < n; i++) if (a[i] != i + 1) return 0;
    return 1;
}

int main(void) {
    int original[] = {1, 4, 3, 2, 5};
    int n = (int)(sizeof(original) / sizeof(original[0]));
    int a[5], b[5];
    memcpy(a, original, sizeof(original));
    memcpy(b, original, sizeof(original));

    int lowerBoundReversals = (breakpoints(original, n) + 1) / 2;
    printf("Input: ");
    printArray(original, n);
    printf("Breakpoint lower bound: %d reversal(s)\n", lowerBoundReversals);

    reversalCount = reversalCost = 0;
    sortWithAtMostNReversals(a, n);
    printf("\nPart A result: ");
    printArray(a, n);
    printf("Reversals: %lld, cost: %lld\n", reversalCount, reversalCost);

    reversalCount = reversalCost = 0;
    mergeSortByReversal(b, 0, n);
    printf("\nPart B result: ");
    printArray(b, n);
    printf("Reversals: %lld, cost: %lld\n", reversalCount, reversalCost);

    if (!isSorted(a, n) || !isSorted(b, n)) {
        printf("Validation failed\n");
        return 1;
    }

    printf("\nValidation: both methods sorted the permutation\n");
    printf("\nComplexity\n");
    printf("Part A: at most n-1 reversals, O(n^2) running time\n");
    printf("Part B: O(n log^2 n) total reversal cost\n");
    return 0;
}
