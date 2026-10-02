#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

#define MAX_ROD_LENGTH ((size_t)10000U)
static int checked_add(long long left, long long right, long long *result)
{
    if ((right > 0 && left > LLONG_MAX - right) ||
        (right < 0 && left < LLONG_MIN - right)) {
        return 0;
    }
    *result = left + right;
    return 1;
}

int main(void)
{
    size_t length;
    long long *price;
    long long *revenue;
    size_t *first_piece;
    size_t i;

    printf("Enter rod length n (positive integer): ");
    if (scanf("%zu", &length) != 1 || length == 0U ||
        length > MAX_ROD_LENGTH) {
        fprintf(stderr, "Error: n must be between 1 and %zu.\n",
                MAX_ROD_LENGTH);
        return EXIT_FAILURE;
    }

    price = malloc(length * sizeof(*price));
    revenue = malloc((length + 1U) * sizeof(*revenue));
    first_piece = malloc((length + 1U) * sizeof(*first_piece));
    if (price == NULL || revenue == NULL || first_piece == NULL) {
        fprintf(stderr, "Error: unable to allocate rod-cutting tables.\n");
        free(price);
        free(revenue);
        free(first_piece);
        return EXIT_FAILURE;
    }

    printf("Enter prices p1 through p%zu:\n", length);
    for (i = 1U; i <= length; ++i) {
        if (scanf("%lld", &price[i - 1U]) != 1) {
            fprintf(stderr, "Error: price %zu is not a valid integer.\n", i);
            free(price);
            free(revenue);
            free(first_piece);
            return EXIT_FAILURE;
        }
    }

    revenue[0] = 0;
    first_piece[0] = 0U;
    for (i = 1U; i <= length; ++i) {
        size_t piece;
        long long best = price[i - 1U];
        size_t best_piece = i;

        for (piece = 1U; piece < i; ++piece) {
            long long candidate;
            if (!checked_add(revenue[i - piece], price[piece - 1U],
                             &candidate)) {
                fprintf(stderr,
                        "Error: revenue overflow while considering length %zu.\n",
                        i);
                free(price);
                free(revenue);
                free(first_piece);
                return EXIT_FAILURE;
            }
            if (candidate > best) {
                best = candidate;
                best_piece = piece;
            }
        }
        revenue[i] = best;
        first_piece[i] = best_piece;
    }

    printf("Maximum revenue: %lld\n", revenue[length]);
    printf("Optimal piece lengths: ");
    {
        size_t remaining = length;
        int first = 1;
        while (remaining > 0U) {
            size_t piece = first_piece[remaining];
            if (piece == 0U || piece > remaining) {
                fprintf(stderr, "Error: reconstruction invariant failed.\n");
                free(price);
                free(revenue);
                free(first_piece);
                return EXIT_FAILURE;
            }
            if (!first) {
                printf(" + ");
            }
            printf("%zu", piece);
            first = 0;
            remaining -= piece;
        }
    }
    printf(" (sum = %zu)\n", length);

    free(price);
    free(revenue);
    free(first_piece);
    return EXIT_SUCCESS;
}
