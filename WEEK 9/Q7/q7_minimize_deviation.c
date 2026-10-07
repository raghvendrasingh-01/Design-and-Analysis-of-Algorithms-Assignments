#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint64_t value;
    size_t index;
} HeapNode;

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
    if (*end != '\0' || parsed > UINT64_MAX) {
        return 0;
    }
    *value = (uint64_t)parsed;
    return 1;
}

static int heap_before(HeapNode left, HeapNode right)
{
    return left.value > right.value ||
           (left.value == right.value && left.index < right.index);
}

static void heap_push(HeapNode *heap, size_t *length, HeapNode node)
{
    size_t position = (*length)++;
    heap[position] = node;
    while (position > 0U) {
        size_t parent = (position - 1U) / 2U;
        HeapNode temporary;
        if (heap_before(heap[parent], heap[position])) {
            break;
        }
        temporary = heap[parent];
        heap[parent] = heap[position];
        heap[position] = temporary;
        position = parent;
    }
}

static void heap_down(HeapNode *heap, size_t length, size_t position)
{
    for (;;) {
        size_t left = position * 2U + 1U;
        size_t right = left + 1U;
        size_t best = position;
        HeapNode temporary;
        if (left < length && heap_before(heap[left], heap[best])) {
            best = left;
        }
        if (right < length && heap_before(heap[right], heap[best])) {
            best = right;
        }
        if (best == position) {
            return;
        }
        temporary = heap[position];
        heap[position] = heap[best];
        heap[best] = temporary;
        position = best;
    }
}

int main(void)
{
    char *line = read_line("Enter number of elements n: ");
    size_t n;
    uint64_t *values;
    uint64_t *best_values;
    HeapNode *heap;
    size_t heap_length = 0U;
    uint64_t minimum = UINT64_MAX;
    uint64_t best_deviation = UINT64_MAX;
    size_t i;

    if (line == NULL || !parse_size(line, &n) || n == 0U) {
        fprintf(stderr, "Error: n must be a positive integer.\n");
        free(line);
        return EXIT_FAILURE;
    }
    free(line);
    if (n > SIZE_MAX / sizeof(*values) || n > SIZE_MAX / sizeof(*best_values) ||
        n > SIZE_MAX / sizeof(*heap)) {
        fprintf(stderr, "Error: array is too large.\n");
        return EXIT_FAILURE;
    }
    values = malloc(n * sizeof(*values));
    best_values = malloc(n * sizeof(*best_values));
    heap = malloc(n * sizeof(*heap));
    if (values == NULL || best_values == NULL || heap == NULL) {
        fprintf(stderr, "Error: unable to allocate array buffers.\n");
        free(values);
        free(best_values);
        free(heap);
        return EXIT_FAILURE;
    }
    for (i = 0U; i < n; ++i) {
        uint64_t value;
        line = read_line("Enter a positive integer: ");
        if (line == NULL || !parse_u64(line, &value) || value == 0U) {
            fprintf(stderr, "Error: every element must be a positive integer.\n");
            free(line);
            free(values);
            free(best_values);
            free(heap);
            return EXIT_FAILURE;
        }
        free(line);
        if ((value & UINT64_C(1)) != 0U) {
            if (value > UINT64_MAX / 2U) {
                fprintf(stderr, "Error: doubling an odd element would overflow.\n");
                free(values);
                free(best_values);
                free(heap);
                return EXIT_FAILURE;
            }
            value *= 2U;
        }
        values[i] = value;
        if (value < minimum) {
            minimum = value;
        }
        heap_push(heap, &heap_length, (HeapNode){value, i});
    }

    for (;;) {
        uint64_t maximum = heap[0].value;
        uint64_t deviation = maximum - minimum;
        if (deviation < best_deviation) {
            best_deviation = deviation;
            memcpy(best_values, values, n * sizeof(*best_values));
        }
        if ((maximum & UINT64_C(1)) != 0U) {
            break;
        }
        {
            size_t maximum_index = heap[0].index;
            uint64_t reduced = maximum / 2U;
            if (reduced < minimum) {
                minimum = reduced;
            }
            values[maximum_index] = reduced;
            heap[0].value = reduced;
            heap_down(heap, heap_length, 0U);
        }
    }

    printf("Transformed array: ");
    for (i = 0U; i < n; ++i) {
        printf("%" PRIu64 "%s", best_values[i], i + 1U == n ? "\n" : " ");
    }
    printf("Minimum deviation: %" PRIu64 "\n", best_deviation);
    free(values);
    free(best_values);
    free(heap);
    return EXIT_SUCCESS;
}
