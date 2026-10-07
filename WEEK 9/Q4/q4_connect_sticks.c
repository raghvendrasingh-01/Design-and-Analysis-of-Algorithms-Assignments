#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    uintmax_t *data;
    size_t length;
    size_t capacity;
} MinHeap;

static int read_uint(const char *name, uintmax_t *result)
{
    char token[128];
    char *end;

    if (scanf("%127s", token) != 1 || token[0] == '-') {
        fprintf(stderr, "Invalid %s.\n", name);
        return 0;
    }
    errno = 0;
    *result = strtoumax(token, &end, 10);
    if (errno == ERANGE || *end != '\0') {
        fprintf(stderr, "Invalid %s.\n", name);
        return 0;
    }
    return 1;
}

static void swap_uint(uintmax_t *left, uintmax_t *right)
{
    uintmax_t temporary = *left;
    *left = *right;
    *right = temporary;
}

static void heap_push(MinHeap *heap, uintmax_t value)
{
    size_t index = heap->length++;

    heap->data[index] = value;
    while (index > 0U) {
        size_t parent = (index - 1U) / 2U;
        if (heap->data[parent] <= heap->data[index]) {
            break;
        }
        swap_uint(&heap->data[parent], &heap->data[index]);
        index = parent;
    }
}

static uintmax_t heap_pop(MinHeap *heap)
{
    uintmax_t result = heap->data[0];
    size_t index = 0U;

    heap->data[0] = heap->data[--heap->length];
    while (index * 2U + 1U < heap->length) {
        size_t smallest = index;
        size_t left = index * 2U + 1U;
        size_t right = left + 1U;

        if (heap->data[left] < heap->data[smallest]) {
            smallest = left;
        }
        if (right < heap->length && heap->data[right] < heap->data[smallest]) {
            smallest = right;
        }
        if (smallest == index) {
            break;
        }
        swap_uint(&heap->data[index], &heap->data[smallest]);
        index = smallest;
    }
    return result;
}

int main(void)
{
    uintmax_t count_uint;
    size_t count;
    size_t i;
    uintmax_t total_cost = 0U;
    MinHeap heap;

    heap.data = NULL;
    heap.length = 0U;
    heap.capacity = 0U;
    printf("Minimum Cost to Connect Sticks\n");
    printf("Enter n followed by n non-negative stick lengths:\n");
    if (!read_uint("number of sticks", &count_uint) || count_uint == 0U ||
        count_uint > (uintmax_t)SIZE_MAX) {
        fprintf(stderr, "The number of sticks must be positive and fit in memory.\n");
        return EXIT_FAILURE;
    }
    count = (size_t)count_uint;
    if (count > SIZE_MAX / sizeof(*heap.data)) {
        fprintf(stderr, "Too many sticks.\n");
        return EXIT_FAILURE;
    }
    heap.data = malloc(count * sizeof(*heap.data));
    if (heap.data == NULL) {
        fprintf(stderr, "Unable to allocate the min-heap.\n");
        return EXIT_FAILURE;
    }
    heap.capacity = count;
    for (i = 0U; i < count; ++i) {
        uintmax_t length;
        if (!read_uint("stick length", &length)) {
            free(heap.data);
            return EXIT_FAILURE;
        }
        heap_push(&heap, length);
    }

    printf("Merge sequence:\n");
    while (heap.length > 1U) {
        uintmax_t first = heap_pop(&heap);
        uintmax_t second = heap_pop(&heap);
        uintmax_t merged;

        if (first > UINTMAX_MAX - second ||
            total_cost > UINTMAX_MAX - (first + second)) {
            fprintf(stderr, "Length or total-cost overflow.\n");
            free(heap.data);
            return EXIT_FAILURE;
        }
        merged = first + second;
        total_cost += merged;
        printf("  %" PRIuMAX " + %" PRIuMAX " = %" PRIuMAX " (cost %" PRIuMAX ")\n",
               first, second, merged, merged);
        heap_push(&heap, merged);
    }
    printf("Minimum total cost: %" PRIuMAX "\n", total_cost);
    free(heap.data);
    return EXIT_SUCCESS;
}
