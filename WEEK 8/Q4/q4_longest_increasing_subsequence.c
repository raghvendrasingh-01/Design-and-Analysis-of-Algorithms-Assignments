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

static void print_sequence(const long long sequence[], size_t length)
{
    if (length == 0) {
        printf("(empty)\n");
        return;
    }
    for (size_t i = 0; i < length; ++i) {
        if (i != 0) {
            putchar(' ');
        }
        printf("%lld", sequence[i]);
    }
    putchar('\n');
}

int main(void)
{
    size_t count;
    long long *values = NULL;
    size_t *length = NULL;
    size_t *previous = NULL;
    long long *sequence = NULL;
    size_t best_length = 0;
    size_t best_end = SIZE_MAX;

    printf("Longest Increasing Subsequence\n");
    printf("Enter n followed by n integers:\n");
    if (!read_size("array length", &count)) {
        return EXIT_FAILURE;
    }
    if (count > SIZE_MAX / sizeof(*values) || count > SIZE_MAX / sizeof(*length)
        || count > SIZE_MAX / sizeof(*previous)
        || count > SIZE_MAX / sizeof(*sequence)) {
        fprintf(stderr, "Array length is too large.\n");
        return EXIT_FAILURE;
    }
    if (count != 0) {
        values = malloc(count * sizeof(*values));
        length = malloc(count * sizeof(*length));
        previous = malloc(count * sizeof(*previous));
        if (values == NULL || length == NULL || previous == NULL) {
            fprintf(stderr, "Unable to allocate the input or DP arrays.\n");
            free(values);
            free(length);
            free(previous);
            return EXIT_FAILURE;
        }
    }
    for (size_t i = 0; i < count; ++i) {
        if (scanf("%lld", &values[i]) != 1) {
            fprintf(stderr, "Expected an integer at position %zu.\n", i);
            free(values);
            free(length);
            free(previous);
            return EXIT_FAILURE;
        }
    }

    for (size_t i = 0; i < count; ++i) {
        length[i] = 1;
        previous[i] = SIZE_MAX;
        for (size_t j = 0; j < i; ++j) {
            if (values[j] < values[i] && length[j] + 1 > length[i]) {
                length[i] = length[j] + 1;
                previous[i] = j;
            }
        }
        if (length[i] > best_length) {
            best_length = length[i];
            best_end = i;
        }
    }

    if (best_length != 0) {
        sequence = malloc(best_length * sizeof(*sequence));
        if (sequence == NULL) {
            fprintf(stderr, "Unable to allocate the reconstructed subsequence.\n");
            free(values);
            free(length);
            free(previous);
            return EXIT_FAILURE;
        }
        for (size_t position = best_length; position > 0; --position) {
            sequence[position - 1] = values[best_end];
            best_end = previous[best_end];
        }
    }
    printf("LIS length: %zu\n", best_length);
    printf("LIS: ");
    print_sequence(sequence, best_length);

    free(sequence);
    free(values);
    free(length);
    free(previous);
    return EXIT_SUCCESS;
}
