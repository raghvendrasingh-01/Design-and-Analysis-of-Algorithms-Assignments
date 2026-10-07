#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Exact classroom-scale search: the prompt does not define the time unit. */
#define MAX_ITEMS 9U

typedef struct {
    long double weight;
    long double density;
    long double decay;
} Item;

typedef struct {
    size_t index;
    long double density;
} Choice;

typedef struct {
    Item items[MAX_ITEMS];
    size_t count;
    long double capacity;
    size_t order[MAX_ITEMS];
    size_t best_order[MAX_ITEMS];
    long double best_amount[MAX_ITEMS];
    int used[MAX_ITEMS];
    long double best_value;
} Search;

static int read_size(size_t *result)
{
    char token[128];
    char *end;
    uintmax_t number;

    if (scanf("%127s", token) != 1 || token[0] == '-') {
        return 0;
    }
    errno = 0;
    number = strtoumax(token, &end, 10);
    if (errno == ERANGE || *end != '\0' || number > MAX_ITEMS) {
        return 0;
    }
    *result = (size_t)number;
    return 1;
}

static int read_number(long double *result)
{
    char token[128];
    char *end;
    double number;

    if (scanf("%127s", token) != 1) {
        return 0;
    }
    errno = 0;
    number = strtod(token, &end);
    if (errno == ERANGE || *end != '\0' || !isfinite(number)) {
        return 0;
    }
    *result = (long double)number;
    return 1;
}

static int compare_choice(const void *left, const void *right)
{
    const Choice *a = left;
    const Choice *b = right;

    if (a->density != b->density) {
        return a->density > b->density ? -1 : 1;
    }
    return a->index < b->index ? -1 : (a->index > b->index ? 1 : 0);
}

static void evaluate_order(Search *search)
{
    Choice choices[MAX_ITEMS];
    long double amount[MAX_ITEMS] = {0.0L};
    long double remaining = search->capacity;
    long double value = 0.0L;

    for (size_t slot = 0U; slot < search->count; ++slot) {
        size_t index = search->order[slot];
        choices[slot].index = index;
        choices[slot].density = search->items[index].density -
                                search->items[index].decay * (long double)slot;
    }
    qsort(choices, search->count, sizeof(*choices), compare_choice);
    for (size_t i = 0U; i < search->count && remaining > 0.0L; ++i) {
        size_t index = choices[i].index;
        if (choices[i].density <= 0.0L) {
            break;
        }
        amount[index] = search->items[index].weight < remaining
                            ? search->items[index].weight : remaining;
        remaining -= amount[index];
        value += amount[index] * choices[i].density;
    }
    if (value > search->best_value) {
        search->best_value = value;
        for (size_t i = 0U; i < search->count; ++i) {
            search->best_order[i] = search->order[i];
            search->best_amount[i] = amount[i];
        }
    }
}

static void search_orders(Search *search, size_t slot)
{
    if (slot == search->count) {
        evaluate_order(search);
        return;
    }
    for (size_t i = 0U; i < search->count; ++i) {
        if (!search->used[i]) {
            search->used[i] = 1;
            search->order[slot] = i;
            search_orders(search, slot + 1U);
            search->used[i] = 0;
        }
    }
}

int main(void)
{
    Search search = {0};
    long double remaining;

    printf("Fractional Knapsack with Deterioration\n");
    printf("Enter n (0..9), then n lines of value weight decay, then capacity:\n");
    if (!read_size(&search.count)) {
        fprintf(stderr, "Invalid item count; exact scheduling supports 0..9 items.\n");
        return EXIT_FAILURE;
    }
    for (size_t i = 0U; i < search.count; ++i) {
        long double value;
        if (!read_number(&value) || !read_number(&search.items[i].weight) ||
            !read_number(&search.items[i].decay) || value < 0.0L ||
            search.items[i].weight <= 0.0L || search.items[i].decay <= 0.0L) {
            fprintf(stderr, "Use finite non-negative values and positive weights and decay rates.\n");
            return EXIT_FAILURE;
        }
        search.items[i].density = value / search.items[i].weight;
        /* Keep all intermediate arithmetic representable, even if long double
           has the same range as double on another C11 implementation. */
        if (!isfinite(search.items[i].density) ||
            !isfinite(search.items[i].decay * (long double)MAX_ITEMS) ||
            !isfinite(value * (long double)MAX_ITEMS)) {
            fprintf(stderr, "Item magnitude is too large for value arithmetic.\n");
            return EXIT_FAILURE;
        }
    }
    if (!read_number(&search.capacity) || search.capacity < 0.0L) {
        fprintf(stderr, "Capacity must be a non-negative finite number.\n");
        return EXIT_FAILURE;
    }
    search.best_value = -1.0L;
    search_orders(&search, 0U);
    printf("Model: one unit-time opportunity per item; value is sampled at slot start.\n");
    printf("Optimal scheduling order and fractional choices (t starts at 0):\n");
    remaining = search.capacity;
    for (size_t slot = 0U; slot < search.count; ++slot) {
        size_t index = search.best_order[slot];
        long double density = search.items[index].density -
                              search.items[index].decay * (long double)slot;
        long double amount = search.best_amount[index];
        remaining -= amount;
        printf("  t=%zu: item %zu, effective density %.6Lf, weight %.6Lf / %.6Lf, fraction %.6Lf\n",
               slot, index + 1U, density, amount, search.items[index].weight,
               amount / search.items[index].weight);
    }
    printf("Unused capacity: %.6Lf\n", remaining > 0.0L ? remaining : 0.0L);
    printf("Total effective value: %.6Lf\n", search.best_value);
    return EXIT_SUCCESS;
}
