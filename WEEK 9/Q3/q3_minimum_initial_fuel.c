#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    uintmax_t distance;
    uintmax_t fuel;
} Station;

typedef struct {
    uintmax_t *data;
    size_t length;
    size_t capacity;
} MaxHeap;

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

static int compare_station(const void *left, const void *right)
{
    const Station *a = left;
    const Station *b = right;

    if (a->distance != b->distance) {
        return a->distance < b->distance ? -1 : 1;
    }
    return a->fuel < b->fuel ? -1 : (a->fuel > b->fuel ? 1 : 0);
}

static void heap_swap(uintmax_t *left, uintmax_t *right)
{
    uintmax_t temporary = *left;
    *left = *right;
    *right = temporary;
}

static void heap_push(MaxHeap *heap, uintmax_t value)
{
    size_t index = heap->length++;

    heap->data[index] = value;
    while (index > 0U) {
        size_t parent = (index - 1U) / 2U;
        if (heap->data[parent] >= heap->data[index]) {
            break;
        }
        heap_swap(&heap->data[parent], &heap->data[index]);
        index = parent;
    }
}

static uintmax_t heap_pop(MaxHeap *heap)
{
    uintmax_t result = heap->data[0];
    size_t index = 0U;

    heap->data[0] = heap->data[--heap->length];
    while (index * 2U + 1U < heap->length) {
        size_t largest = index;
        size_t left = index * 2U + 1U;
        size_t right = left + 1U;

        if (heap->data[left] > heap->data[largest]) {
            largest = left;
        }
        if (right < heap->length && heap->data[right] > heap->data[largest]) {
            largest = right;
        }
        if (largest == index) {
            break;
        }
        heap_swap(&heap->data[index], &heap->data[largest]);
        index = largest;
    }
    return result;
}

int main(void)
{
    uintmax_t target;
    uintmax_t fuel;
    uintmax_t station_count_uint;
    size_t station_count;
    size_t i;
    uintmax_t previous = 0U;
    size_t stops = 0U;
    Station *stations = NULL;
    MaxHeap heap;
    int reachable = 1;

    heap.data = NULL;
    heap.length = 0U;
    heap.capacity = 0U;
    printf("Minimum Refuelling Stops\n");
    printf("Enter target distance D, initial fuel F, station count, then distance fuel pairs:\n");
    if (!read_uint("target distance", &target) || !read_uint("initial fuel", &fuel) ||
        !read_uint("station count", &station_count_uint) ||
        station_count_uint > (uintmax_t)SIZE_MAX) {
        return EXIT_FAILURE;
    }
    station_count = (size_t)station_count_uint;
    if (station_count > SIZE_MAX / sizeof(*stations) ||
        station_count > SIZE_MAX / sizeof(*heap.data)) {
        fprintf(stderr, "Too many stations.\n");
        return EXIT_FAILURE;
    }
    if (station_count > 0U) {
        stations = malloc(station_count * sizeof(*stations));
        heap.data = malloc(station_count * sizeof(*heap.data));
        if (stations == NULL || heap.data == NULL) {
            fprintf(stderr, "Unable to allocate station structures.\n");
            free(stations);
            free(heap.data);
            return EXIT_FAILURE;
        }
    }
    heap.capacity = station_count;
    for (i = 0U; i < station_count; ++i) {
        if (!read_uint("station distance", &stations[i].distance) ||
            !read_uint("station fuel", &stations[i].fuel) ||
            stations[i].distance >= target) {
            fprintf(stderr, "Station distances must satisfy 0 <= distance < D.\n");
            free(stations);
            free(heap.data);
            return EXIT_FAILURE;
        }
    }
    qsort(stations, station_count, sizeof(*stations), compare_station);

    for (i = 0U; i <= station_count; ++i) {
        uintmax_t destination = i < station_count ? stations[i].distance : target;
        uintmax_t gap = destination - previous;

        while (fuel < gap) {
            if (heap.length == 0U) {
                reachable = 0;
                break;
            }
            if (fuel > UINTMAX_MAX - heap.data[0]) {
                fprintf(stderr, "Fuel quantity overflow.\n");
                free(stations);
                free(heap.data);
                return EXIT_FAILURE;
            }
            fuel += heap_pop(&heap);
            ++stops;
        }
        if (!reachable) {
            break;
        }
        fuel -= gap;
        previous = destination;
        if (i < station_count) {
            heap_push(&heap, stations[i].fuel);
        }
    }

    printf("Target distance: %" PRIuMAX "\n", target);
    if (!reachable) {
        printf("Result: unreachable with the supplied initial fuel and stations\n");
    } else {
        printf("Minimum refuelling stops: %zu\n", stops);
        printf("Fuel remaining at target: %" PRIuMAX "\n", fuel);
    }
    free(stations);
    free(heap.data);
    return EXIT_SUCCESS;
}
