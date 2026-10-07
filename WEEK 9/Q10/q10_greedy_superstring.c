#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static char *copy_string(const char *text)
{
    size_t length = strlen(text);
    char *copy;
    if (length == SIZE_MAX) {
        return NULL;
    }
    copy = malloc(length + 1U);
    if (copy != NULL) {
        memcpy(copy, text, length + 1U);
    }
    return copy;
}

static size_t overlap_length(const char *left, const char *right)
{
    size_t left_length = strlen(left);
    size_t right_length = strlen(right);
    size_t overlap = left_length < right_length ? left_length : right_length;
    while (overlap > 0U) {
        if (memcmp(left + left_length - overlap, right, overlap) == 0) {
            return overlap;
        }
        --overlap;
    }
    return 0U;
}

static char *merge_strings(const char *left, const char *right, size_t overlap)
{
    size_t left_length = strlen(left);
    size_t right_length = strlen(right);
    size_t result_length;
    char *result;

    if (overlap > left_length || overlap > right_length ||
        right_length - overlap > SIZE_MAX - left_length - 1U) {
        return NULL;
    }
    result_length = left_length + right_length - overlap;
    result = malloc(result_length + 1U);
    if (result == NULL) {
        return NULL;
    }
    memcpy(result, left, left_length);
    memcpy(result + left_length, right + overlap, right_length - overlap);
    result[result_length] = '\0';
    return result;
}

static void remove_contained(char **strings, size_t *count)
{
    size_t i = 0U;
    while (i < *count) {
        size_t j;
        int contained = 0;
        size_t length_i = strlen(strings[i]);
        for (j = 0U; j < *count; ++j) {
            if (i != j && length_i <= strlen(strings[j]) &&
                strstr(strings[j], strings[i]) != NULL) {
                contained = 1;
                break;
            }
        }
        if (contained) {
            free(strings[i]);
            for (j = i + 1U; j < *count; ++j) {
                strings[j - 1U] = strings[j];
            }
            --*count;
        } else {
            ++i;
        }
    }
}

static int greedy_superstring(char **input, size_t count, char **result)
{
    char **work = calloc(count, sizeof(*work));
    size_t i;

    if (work == NULL) {
        return 0;
    }
    for (i = 0U; i < count; ++i) {
        work[i] = copy_string(input[i]);
        if (work[i] == NULL) {
            while (i > 0U) {
                free(work[--i]);
            }
            free(work);
            return 0;
        }
    }
    while (count > 1U) {
        size_t best_i = 0U;
        size_t best_j = 1U;
        size_t best_overlap = 0U;
        char *merged;
        for (i = 0U; i < count; ++i) {
            size_t j;
            for (j = i + 1U; j < count; ++j) {
                size_t forward = overlap_length(work[i], work[j]);
                size_t backward = overlap_length(work[j], work[i]);
                if (forward > best_overlap) {
                    best_i = i;
                    best_j = j;
                    best_overlap = forward;
                }
                if (backward > best_overlap) {
                    best_i = j;
                    best_j = i;
                    best_overlap = backward;
                }
            }
        }
        merged = merge_strings(work[best_i], work[best_j], best_overlap);
        if (merged == NULL) {
            for (i = 0U; i < count; ++i) {
                free(work[i]);
            }
            free(work);
            return 0;
        }
        free(work[best_i]);
        free(work[best_j]);
        work[best_i] = merged;
        for (i = best_j + 1U; i < count; ++i) {
            work[i - 1U] = work[i];
        }
        --count;
        remove_contained(work, &count);
    }
    *result = work[0];
    free(work);
    return 1;
}

static int exact_superstring(char **strings, size_t n, char **result,
                             uint64_t *total_overlap)
{
    size_t mask_count = (size_t)1U << n;
    size_t cells = mask_count * n;
    size_t *overlaps = calloc(n * n, sizeof(*overlaps));
    uint64_t *dp = calloc(cells, sizeof(*dp));
    int *parent = malloc(cells * sizeof(*parent));
    size_t total_length = 0U;
    size_t mask;
    size_t last;
    size_t *path;
    size_t current_mask;
    char *built;

    if (overlaps == NULL || dp == NULL || parent == NULL) {
        free(overlaps);
        free(dp);
        free(parent);
        return 0;
    }
    for (last = 0U; last < cells; ++last) {
        parent[last] = -1;
    }
    for (last = 0U; last < n; ++last) {
        size_t next;
        size_t length = strlen(strings[last]);
        if (length > SIZE_MAX - total_length) {
            free(overlaps);
            free(dp);
            free(parent);
            return 0;
        }
        total_length += length;
        for (next = 0U; next < n; ++next) {
            overlaps[last * n + next] = overlap_length(strings[last], strings[next]);
        }
    }
    for (mask = 1U; mask < mask_count; ++mask) {
        for (last = 0U; last < n; ++last) {
            size_t previous_mask;
            size_t previous;
            uint64_t best = 0U;
            int best_parent = -1;
            if ((mask & ((size_t)1U << last)) == 0U) {
                continue;
            }
            previous_mask = mask ^ ((size_t)1U << last);
            if (previous_mask != 0U) {
                for (previous = 0U; previous < n; ++previous) {
                    uint64_t candidate;
                    if ((previous_mask & ((size_t)1U << previous)) == 0U ||
                        dp[previous_mask * n + previous] >
                            UINT64_MAX - overlaps[previous * n + last]) {
                        continue;
                    }
                    candidate = dp[previous_mask * n + previous] +
                                overlaps[previous * n + last];
                    if (best_parent < 0 || candidate > best) {
                        best = candidate;
                        best_parent = (int)previous;
                    }
                }
            }
            dp[mask * n + last] = best;
            parent[mask * n + last] = best_parent;
        }
    }
    last = 0U;
    for (mask = 1U; mask < n; ++mask) {
        if (dp[(mask_count - 1U) * n + mask] >
            dp[(mask_count - 1U) * n + last]) {
            last = mask;
        }
    }
    *total_overlap = dp[(mask_count - 1U) * n + last];
    path = malloc(n * sizeof(*path));
    if (path == NULL) {
        free(overlaps);
        free(dp);
        free(parent);
        return 0;
    }
    current_mask = mask_count - 1U;
    for (mask = n; mask > 0U; --mask) {
        int previous = parent[current_mask * n + last];
        path[mask - 1U] = last;
        current_mask ^= (size_t)1U << last;
        if (previous < 0) {
            break;
        }
        last = (size_t)previous;
    }
    built = copy_string(strings[path[0]]);
    if (built != NULL) {
        for (mask = 1U; mask < n; ++mask) {
            char *next_built = merge_strings(
                built, strings[path[mask]],
                overlaps[path[mask - 1U] * n + path[mask]]);
            free(built);
            built = next_built;
            if (built == NULL) {
                break;
            }
        }
    }
    free(path);
    free(overlaps);
    free(dp);
    free(parent);
    if (built == NULL || strlen(built) != total_length - *total_overlap) {
        free(built);
        return 0;
    }
    *result = built;
    return 1;
}

int main(void)
{
    char *line = read_line("Enter number of strings n: ");
    size_t n;
    char **strings;
    char *greedy;
    size_t i;

    if (line == NULL || !parse_size(line, &n) || n == 0U || n > 100U) {
        fprintf(stderr, "Error: n must be between 1 and 100.\n");
        free(line);
        return EXIT_FAILURE;
    }
    free(line);
    strings = calloc(n, sizeof(*strings));
    if (strings == NULL) {
        fprintf(stderr, "Error: unable to allocate string list.\n");
        return EXIT_FAILURE;
    }
    for (i = 0U; i < n; ++i) {
        char prompt[64];
        (void)snprintf(prompt, sizeof(prompt), "Enter string %zu: ", i + 1U);
        strings[i] = read_line(prompt);
        if (strings[i] == NULL || strings[i][0] == '\0') {
            fprintf(stderr, "Error: strings must be non-empty lines.\n");
            free(strings[i]);
            while (i > 0U) {
                free(strings[--i]);
            }
            free(strings);
            return EXIT_FAILURE;
        }
    }
    remove_contained(strings, &n);
    if (n == 0U) {
        fprintf(stderr, "Error: no strings remain after validation.\n");
        free(strings);
        return EXIT_FAILURE;
    }
    if (!greedy_superstring(strings, n, &greedy)) {
        fprintf(stderr, "Error: unable to construct greedy superstring.\n");
        for (i = 0U; i < n; ++i) {
            free(strings[i]);
        }
        free(strings);
        return EXIT_FAILURE;
    }
    printf("Greedy superstring: %s\n", greedy);
    for (i = 0U; i < n; ++i) {
        if (strstr(greedy, strings[i]) == NULL) {
            fprintf(stderr, "Error: greedy containment validation failed.\n");
            free(greedy);
            for (i = 0U; i < n; ++i) {
                free(strings[i]);
            }
            free(strings);
            return EXIT_FAILURE;
        }
    }
    printf("Greedy length: %zu\n", strlen(greedy));
    if (n <= 12U) {
        char *exact;
        uint64_t overlap;
        if (!exact_superstring(strings, n, &exact, &overlap)) {
            fprintf(stderr, "Error: exact reference could not be allocated.\n");
            free(greedy);
            for (i = 0U; i < n; ++i) {
                free(strings[i]);
            }
            free(strings);
            return EXIT_FAILURE;
        }
        printf("Exact DP superstring: %s\n", exact);
        printf("Exact DP length: %zu\n", strlen(exact));
        printf("Greedy versus exact: %s\n",
               strlen(greedy) == strlen(exact) ? "optimal on this instance"
                                                : "greedy is longer on this instance");
        free(exact);
    } else {
        printf("Exact DP reference: skipped (n > 12; exponential state space).\n");
    }
    free(greedy);
    for (i = 0U; i < n; ++i) {
        free(strings[i]);
    }
    free(strings);
    return EXIT_SUCCESS;
}
