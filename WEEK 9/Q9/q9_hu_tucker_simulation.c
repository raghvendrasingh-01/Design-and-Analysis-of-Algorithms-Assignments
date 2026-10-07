#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static char *read_line(const char *prompt)
{
    size_t length = 0U;
    size_t capacity = 32U;
    char *line = malloc(capacity);
    int character;

    fputs(prompt, stdout);
    if (line == NULL) {
        return NULL;
    }
    while ((character = getchar()) != '\n' && character != EOF) {
        if (character == 0) {
            free(line);
            return NULL;
        }
        if (length + 1U >= capacity) {
            char *grown;
            if (capacity > SIZE_MAX / 2U) {
                free(line);
                return NULL;
            }
            capacity *= 2U;
            grown = realloc(line, capacity);
            if (grown == NULL) {
                free(line);
                return NULL;
            }
            line = grown;
        }
        line[length++] = (char)character;
    }
    if (character == EOF && length == 0U) {
        free(line);
        return NULL;
    }
    if (length > 0U && line[length - 1U] == '\r') {
        --length;
    }
    line[length] = '\0';
    return line;
}

static int parse_size(const char *text, size_t *value)
{
    char *end;
    unsigned long long parsed;
    while (*text == ' ' || *text == '\t') {
        ++text;
    }
    if (*text == '-' || *text == '\0') {
        return 0;
    }
    errno = 0;
    parsed = strtoull(text, &end, 10);
    if (errno == ERANGE || end == text) {
        return 0;
    }
    while (*end == ' ' || *end == '\t') {
        ++end;
    }
    if (*end != '\0' || parsed > (unsigned long long)SIZE_MAX) {
        return 0;
    }
    *value = (size_t)parsed;
    return 1;
}

static int parse_u64(const char *text, uint64_t *value)
{
    char *end;
    unsigned long long parsed;
    while (*text == ' ' || *text == '\t') {
        ++text;
    }
    if (*text == '-' || *text == '\0') {
        return 0;
    }
    errno = 0;
    parsed = strtoull(text, &end, 10);
    if (errno == ERANGE || end == text) {
        return 0;
    }
    while (*end == ' ' || *end == '\t') {
        ++end;
    }
    if (*end != '\0' || parsed == 0U || parsed > UINT64_MAX) {
        return 0;
    }
    *value = (uint64_t)parsed;
    return 1;
}

static int add_u64(uint64_t left, uint64_t right, uint64_t *result)
{
    if (left > UINT64_MAX - right) {
        return 0;
    }
    *result = left + right;
    return 1;
}

static void print_tree(const size_t *split, size_t n, size_t first, size_t last)
{
    if (first == last) {
        printf("%zu", first + 1U);
        return;
    }
    printf("(");
    print_tree(split, n, first, split[first * n + last]);
    printf(" + ");
    print_tree(split, n, split[first * n + last] + 1U, last);
    printf(")");
}

int main(void)
{
    char *line = read_line("Enter number of ordered weights n: ");
    size_t n;
    uint64_t *weights;
    uint64_t *prefix;
    uint64_t *cost;
    size_t *split;
    size_t cells;
    size_t i;

    if (line == NULL || !parse_size(line, &n) || n == 0U || n > 250U) {
        fprintf(stderr, "Error: n must be between 1 and 250.\n");
        free(line);
        return EXIT_FAILURE;
    }
    free(line);
    if (n > SIZE_MAX / n || n * n > SIZE_MAX / sizeof(*cost) ||
        n * n > SIZE_MAX / sizeof(*split)) {
        fprintf(stderr, "Error: dynamic-programming table is too large.\n");
        return EXIT_FAILURE;
    }
    cells = n * n;
    weights = malloc(n * sizeof(*weights));
    prefix = malloc((n + 1U) * sizeof(*prefix));
    cost = malloc(cells * sizeof(*cost));
    split = malloc(cells * sizeof(*split));
    if (weights == NULL || prefix == NULL || cost == NULL || split == NULL) {
        fprintf(stderr, "Error: unable to allocate DP buffers.\n");
        free(weights);
        free(prefix);
        free(cost);
        free(split);
        return EXIT_FAILURE;
    }
    prefix[0] = 0U;
    for (i = 0U; i < n; ++i) {
        line = read_line("Enter a positive weight: ");
        if (line == NULL || !parse_u64(line, &weights[i]) ||
            !add_u64(prefix[i], weights[i], &prefix[i + 1U])) {
            fprintf(stderr, "Error: weights must be positive and their sum must fit uint64_t.\n");
            free(line);
            free(weights);
            free(prefix);
            free(cost);
            free(split);
            return EXIT_FAILURE;
        }
        free(line);
    }

    for (i = 0U; i < n; ++i) {
        cost[i * n + i] = 0U;
        split[i * n + i] = i;
    }
    for (i = 2U; i <= n; ++i) {
        size_t first;
        for (first = 0U; first + i <= n; ++first) {
            size_t last = first + i - 1U;
            uint64_t interval_sum = prefix[last + 1U] - prefix[first];
            uint64_t best = UINT64_MAX;
            int found = 0;
            size_t best_split = first;
            size_t middle;
            for (middle = first; middle < last; ++middle) {
                uint64_t subtotal;
                uint64_t candidate;
                if (!add_u64(cost[first * n + middle],
                             cost[(middle + 1U) * n + last], &subtotal) ||
                    !add_u64(subtotal, interval_sum, &candidate)) {
                    continue;
                }
                if (!found || candidate < best) {
                    best = candidate;
                    best_split = middle;
                    found = 1;
                }
            }
            if (!found) {
                fprintf(stderr, "Error: optimal cost overflows uint64_t.\n");
                free(weights);
                free(prefix);
                free(cost);
                free(split);
                return EXIT_FAILURE;
            }
            cost[first * n + last] = best;
            split[first * n + last] = best_split;
        }
    }

    printf("Optimal alphabetic weighted path cost: %" PRIu64 "\n",
           cost[n - 1U]);
    printf("Optimal ordered merge tree: ");
    print_tree(split, n, 0U, n - 1U);
    printf("\n");
    printf("Interpretation: leaf i has weight wi and contributes wi * depth(i).\n");

    free(weights);
    free(prefix);
    free(cost);
    free(split);
    return EXIT_SUCCESS;
}
