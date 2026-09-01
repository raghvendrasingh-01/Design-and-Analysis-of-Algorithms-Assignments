#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

static void printArray(const int a[], int n) {
    for (int i = 0; i < n; i++) printf("%d%s", a[i], i + 1 == n ? "\n" : " ");
}

/* Bubble sort is used because it is short and easy to understand. */
static void sortArray(int a[], int n) {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (a[j] > a[j + 1]) swap(&a[j], &a[j + 1]);
}

static int maximum(const int a[], int n) {
    int answer = a[0];
    for (int i = 1; i < n; i++) if (a[i] > answer) answer = a[i];
    return answer;
}

static void twoLargest(const int a[], int n, int *first, int *second) {
    if (a[0] > a[1]) {
        *first = a[0];
        *second = a[1];
    } else {
        *first = a[1];
        *second = a[0];
    }

    for (int i = 2; i < n; i++) {
        if (a[i] > *first) {
            *second = *first;
            *first = a[i];
        } else if (a[i] > *second) {
            *second = a[i];
        }
    }
}

static double mean(const int a[], int n) {
    double sum = 0.0;
    for (int i = 0; i < n; i++) sum += a[i];
    return sum / n;
}

static double median(const int a[], int n) {
    int *copy = malloc(sizeof(int) * (size_t)n);
    if (!copy) exit(EXIT_FAILURE);
    memcpy(copy, a, sizeof(int) * (size_t)n);
    sortArray(copy, n);

    double answer;
    if (n % 2 == 1) answer = copy[n / 2];
    else answer = ((double)copy[n / 2 - 1] + copy[n / 2]) / 2.0;

    free(copy);
    return answer;
}

static double standardDeviation(const int a[], int n) {
    double avg = mean(a, n), sum = 0.0;
    for (int i = 0; i < n; i++) {
        double difference = a[i] - avg;
        sum += difference * difference;
    }
    return sqrt(sum / n);
}

static int mode(const int a[], int n) {
    int *copy = malloc(sizeof(int) * (size_t)n);
    if (!copy) exit(EXIT_FAILURE);
    memcpy(copy, a, sizeof(int) * (size_t)n);
    sortArray(copy, n);

    int answer = copy[0], bestCount = 1, currentCount = 1;
    for (int i = 1; i < n; i++) {
        currentCount = copy[i] == copy[i - 1] ? currentCount + 1 : 1;
        if (currentCount > bestCount) {
            bestCount = currentCount;
            answer = copy[i];
        }
    }

    free(copy);
    return answer;
}

static int removeDuplicates(const int a[], int n, int result[]) {
    int *copy = malloc(sizeof(int) * (size_t)n);
    if (!copy) exit(EXIT_FAILURE);
    memcpy(copy, a, sizeof(int) * (size_t)n);
    sortArray(copy, n);

    int size = 0;
    for (int i = 0; i < n; i++)
        if (i == 0 || copy[i] != copy[i - 1]) result[size++] = copy[i];

    free(copy);
    return size;
}

static void reverseArray(int a[], int n) {
    for (int i = 0; i < n / 2; i++) swap(&a[i], &a[n - 1 - i]);
}

/* Returns the first index of the < pivot part. */
static int partition(int a[], int n, int pivot) {
    int next = 0;
    for (int i = 0; i < n; i++)
        if (a[i] >= pivot) swap(&a[next++], &a[i]);
    return next;
}

int main(void) {
    int a[] = {31, 7, 64, 12, 7, 89, 45, 7, 23, 56};
    int n = (int)(sizeof(a) / sizeof(a[0]));
    int first, second, distinct[10], copy[10];

    printf("Input: ");
    printArray(a, n);

    twoLargest(a, n, &first, &second);
    printf("Maximum: %d\n", maximum(a, n));
    printf("Largest and second largest: %d %d\n", first, second);
    printf("Mean: %.2f\n", mean(a, n));
    printf("Median: %.2f\n", median(a, n));
    printf("Standard deviation: %.2f\n", standardDeviation(a, n));
    printf("Mode: %d\n", mode(a, n));

    int distinctCount = removeDuplicates(a, n, distinct);
    printf("Without duplicates: ");
    printArray(distinct, distinctCount);

    memcpy(copy, a, sizeof(a));
    reverseArray(copy, n);
    printf("Reversed: ");
    printArray(copy, n);

    memcpy(copy, a, sizeof(a));
    int split = partition(copy, n, 31);
    printf("Partition around 31: ");
    printArray(copy, n);
    printf("Split index: %d ([>= 31] followed by [< 31])\n", split);

    printf("\nWorst-case time complexities\n");
    printf("Maximum, two largest, mean, standard deviation: O(n)\n");
    printf("Median, mode, remove duplicates (bubble sort): O(n^2)\n");
    printf("Reverse and partition: O(n)\n");
    return 0;
}
