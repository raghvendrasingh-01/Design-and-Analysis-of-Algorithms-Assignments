#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static int read_size(const char *name, size_t *value)
{
    char token[128];
    char *end;
    uintmax_t input;

    if (scanf("%127s", token) != 1 || token[0] == '-') {
        fprintf(stderr, "Invalid %s. Use a non-negative value that fits in memory.\n", name);
        return 0;
    }
    errno = 0;
    input = strtoumax(token, &end, 10);
    if (errno == ERANGE || *end != '\0' || input > (uintmax_t)SIZE_MAX) {
        fprintf(stderr, "Invalid %s. Use a non-negative value that fits in memory.\n", name);
        return 0;
    }
    *value = (size_t)input;
    return 1;
}

static void print_sequence(const uint64_t sequence[], size_t length)
{
    if (length == 0) {
        printf("(empty)\n");
        return;
    }
    for (size_t i = 0; i < length; ++i) {
        if (i != 0) {
            putchar(' ');
        }
        printf("%" PRIu64, sequence[i]);
    }
    putchar('\n');
}

int main(void)
{
    size_t count;
    uint64_t *values = NULL;
    uint64_t *sum = NULL;
    size_t *previous = NULL;
    uint64_t *sequence = NULL;
    size_t best_length = 0;
    size_t best_end = SIZE_MAX;
    uint64_t best_sum = 0;
    int overflow = 0;

    printf("Maximum Sum Increasing Subsequence\n");
    printf("Enter n followed by n positive integers:\n");
    if (!read_size("array length", &count)) {
        return EXIT_FAILURE;
    }
    if (count > SIZE_MAX / sizeof(*values) || count > SIZE_MAX / sizeof(*sum)
        || count > SIZE_MAX / sizeof(*previous)
        || count > SIZE_MAX / sizeof(*sequence)) {
        fprintf(stderr, "Array length is too large.\n");
        return EXIT_FAILURE;
    }
    if (count != 0) {
        values = malloc(count * sizeof(*values));
        sum = malloc(count * sizeof(*sum));
        previous = malloc(count * sizeof(*previous));
        if (values == NULL || sum == NULL || previous == NULL) {
            fprintf(stderr, "Unable to allocate the input or DP arrays.\n");
            free(values);
            free(sum);
            free(previous);
            return EXIT_FAILURE;
        }
    }
    for (size_t i = 0; i < count; ++i) {
        char token[128];
        char *end;
        uintmax_t input;

        if (scanf("%127s", token) != 1 || token[0] == '-') {
            fprintf(stderr, "Every array value must be a positive integer.\n");
            free(values);
            free(sum);
            free(previous);
            return EXIT_FAILURE;
        }
        errno = 0;
        input = strtoumax(token, &end, 10);
        if (errno == ERANGE || *end != '\0' || input == 0
            || input > (uintmax_t)UINT64_MAX) {
            fprintf(stderr, "Every array value must be a positive integer.\n");
            free(values);
            free(sum);
            free(previous);
            return EXIT_FAILURE;
        }
        values[i] = (uint64_t)input;
    }

    for (size_t i = 0; i < count && !overflow; ++i) {
        sum[i] = values[i];
        previous[i] = SIZE_MAX;
        for (size_t j = 0; j < i; ++j) {
            if (values[j] < values[i]) {
                if (sum[j] > UINT64_MAX - values[i]) {
                    overflow = 1;
                    break;
                }
                if (sum[j] + values[i] > sum[i]) {
                    sum[i] = sum[j] + values[i];
                    previous[i] = j;
                }
            }
        }
        if (!overflow && (sum[i] > best_sum
                          || (sum[i] == best_sum && best_end == SIZE_MAX))) {
            best_sum = sum[i];
            best_end = i;
        }
    }
    if (overflow) {
        fprintf(stderr, "The maximum sum exceeds UINT64_MAX; result cannot be represented.\n");
        free(values);
        free(sum);
        free(previous);
        return EXIT_FAILURE;
    }

    if (best_end != SIZE_MAX) {
        for (size_t current = best_end; current != SIZE_MAX; ++best_length) {
            current = previous[current];
        }
        sequence = malloc(best_length * sizeof(*sequence));
        if (sequence == NULL) {
            fprintf(stderr, "Unable to allocate the reconstructed subsequence.\n");
            free(values);
            free(sum);
            free(previous);
            return EXIT_FAILURE;
        }
        {
            size_t current = best_end;
            for (size_t position = best_length; position > 0; --position) {
                sequence[position - 1] = values[current];
                current = previous[current];
            }
        }
    }

    printf("Maximum sum: %" PRIu64 "\n", best_sum);
    printf("MSIS: ");
    print_sequence(sequence, best_length);
    free(sequence);
    free(values);
    free(sum);
    free(previous);
    return EXIT_SUCCESS;
}
