#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned char character;
    size_t count;
} HeapNode;

typedef struct {
    unsigned char character;
    size_t release_position;
} CooldownEntry;

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

    while (isspace((unsigned char)*text) != 0) {
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
    while (isspace((unsigned char)*end) != 0) {
        ++end;
    }
    if (*end != '\0' || parsed > (unsigned long long)SIZE_MAX) {
        return 0;
    }
    *value = (size_t)parsed;
    return 1;
}

static int higher_priority(HeapNode left, HeapNode right)
{
    if (left.count != right.count) {
        return left.count > right.count;
    }
    return left.character < right.character;
}

static void heap_push(HeapNode *heap, size_t *length, HeapNode node)
{
    size_t position = (*length)++;
    heap[position] = node;
    while (position > 0U) {
        size_t parent = (position - 1U) / 2U;
        if (higher_priority(heap[parent], heap[position])) {
            break;
        }
        {
            HeapNode temporary = heap[parent];
            heap[parent] = heap[position];
            heap[position] = temporary;
        }
        position = parent;
    }
}

static HeapNode heap_pop(HeapNode *heap, size_t *length)
{
    HeapNode result = heap[0];
    size_t position = 0U;

    --*length;
    if (*length == 0U) {
        return result;
    }
    heap[0] = heap[*length];
    for (;;) {
        size_t left = position * 2U + 1U;
        size_t right = left + 1U;
        size_t best = position;
        if (left < *length && higher_priority(heap[left], heap[best])) {
            best = left;
        }
        if (right < *length && higher_priority(heap[right], heap[best])) {
            best = right;
        }
        if (best == position) {
            break;
        }
        {
            HeapNode temporary = heap[position];
            heap[position] = heap[best];
            heap[best] = temporary;
        }
        position = best;
    }
    return result;
}

int main(void)
{
    char *input = read_line("Enter string S: ");
    char *k_line;
    size_t k;
    size_t length;
    size_t counts[UCHAR_MAX + 1U] = {0U};
    HeapNode heap[UCHAR_MAX + 1U];
    CooldownEntry *cooldown;
    size_t heap_length = 0U;
    size_t cooldown_length = 0U;
    size_t cooldown_front = 0U;
    size_t position;
    char *answer;

    if (input == NULL) {
        fprintf(stderr, "Error: unable to read S.\n");
        return EXIT_FAILURE;
    }
    k_line = read_line("Enter K (non-negative): ");
    if (k_line == NULL || !parse_size(k_line, &k)) {
        fprintf(stderr, "Error: K must be a non-negative integer.\n");
        free(input);
        free(k_line);
        return EXIT_FAILURE;
    }
    free(k_line);
    length = strlen(input);
    if (length == SIZE_MAX || length > SIZE_MAX / sizeof(*cooldown)) {
        fprintf(stderr, "Error: input is too long.\n");
        free(input);
        return EXIT_FAILURE;
    }
    answer = malloc(length + 1U);
    cooldown = length == 0U ? NULL : malloc(length * sizeof(*cooldown));
    if (answer == NULL || (length != 0U && cooldown == NULL)) {
        fprintf(stderr, "Error: unable to allocate rearrangement buffers.\n");
        free(answer);
        free(cooldown);
        free(input);
        return EXIT_FAILURE;
    }

    for (position = 0U; position < length; ++position) {
        ++counts[(unsigned char)input[position]];
    }
    for (position = 0U; position <= UCHAR_MAX; ++position) {
        if (counts[position] != 0U) {
            HeapNode node = {(unsigned char)position, counts[position]};
            heap_push(heap, &heap_length, node);
        }
    }

    for (position = 0U; position < length; ++position) {
        while (cooldown_front < cooldown_length &&
               cooldown[cooldown_front].release_position <= position) {
            HeapNode node = {cooldown[cooldown_front].character,
                             counts[cooldown[cooldown_front].character]};
            heap_push(heap, &heap_length, node);
            ++cooldown_front;
        }
        if (heap_length == 0U) {
            printf("Rearranged string: (empty)\n");
            printf("Impossible: no valid arrangement exists for K = %zu.\n", k);
            free(answer);
            free(cooldown);
            free(input);
            return EXIT_SUCCESS;
        }
        {
            HeapNode chosen = heap_pop(heap, &heap_length);
            answer[position] = (char)chosen.character;
            --counts[chosen.character];
            if (counts[chosen.character] != 0U) {
                size_t release_position;
                if (k > SIZE_MAX - position) {
                    release_position = SIZE_MAX;
                } else {
                    release_position = position + k;
                }
                cooldown[cooldown_length].character = chosen.character;
                cooldown[cooldown_length].release_position = release_position;
                ++cooldown_length;
            }
        }
    }
    answer[length] = '\0';
    {
        size_t last[UCHAR_MAX + 1U];
        size_t original_counts[UCHAR_MAX + 1U] = {0U};
        size_t answer_counts[UCHAR_MAX + 1U] = {0U};
        for (position = 0U; position <= UCHAR_MAX; ++position) {
            last[position] = SIZE_MAX;
        }
        for (position = 0U; position < length; ++position) {
            unsigned char character = (unsigned char)answer[position];
            ++original_counts[(unsigned char)input[position]];
            ++answer_counts[character];
            if (last[character] != SIZE_MAX && position - last[character] < k) {
                fprintf(stderr, "Error: cooldown validation failed.\n");
                free(answer);
                free(cooldown);
                free(input);
                return EXIT_FAILURE;
            }
            last[character] = position;
        }
        if (memcmp(original_counts, answer_counts, sizeof(original_counts)) != 0) {
            fprintf(stderr, "Error: character-count validation failed.\n");
            free(answer);
            free(cooldown);
            free(input);
            return EXIT_FAILURE;
        }
    }
    printf("Rearranged string: %s\n", answer);
    printf("Verified: every equal-character pair is at least %zu position(s) apart.\n", k);

    free(answer);
    free(cooldown);
    free(input);
    return EXIT_SUCCESS;
}
