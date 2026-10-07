#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static int read_size(const char *name, size_t *result)
{
    char token[128];
    char *end;
    uintmax_t number;

    if (scanf("%127s", token) != 1 || token[0] == '-') {
        fprintf(stderr, "Invalid %s.\n", name);
        return 0;
    }
    errno = 0;
    number = strtoumax(token, &end, 10);
    if (errno == ERANGE || *end != '\0' || number > (uintmax_t)SIZE_MAX) {
        fprintf(stderr, "Invalid %s.\n", name);
        return 0;
    }
    *result = (size_t)number;
    return 1;
}

static int read_rating(intmax_t *result)
{
    char token[128];
    char *end;

    if (scanf("%127s", token) != 1) {
        return 0;
    }
    errno = 0;
    *result = strtoimax(token, &end, 10);
    if (errno == ERANGE || *end != '\0') {
        return 0;
    }
    return 1;
}

int main(void)
{
    size_t count;
    size_t i;
    intmax_t *ratings = NULL;
    size_t *left = NULL;
    size_t *candies = NULL;
    uintmax_t total = 0U;

    printf("Candy Distribution\n");
    printf("Enter n followed by n child ratings:\n");
    if (!read_size("number of children", &count)) {
        return EXIT_FAILURE;
    }
    if (count == 0U) {
        printf("Candy vector: (empty)\n");
        printf("Minimum total candies: 0\n");
        return EXIT_SUCCESS;
    }
    if (count > SIZE_MAX / sizeof(*ratings) || count > SIZE_MAX / sizeof(*left) ||
        count > SIZE_MAX / sizeof(*candies)) {
        fprintf(stderr, "Too many children.\n");
        return EXIT_FAILURE;
    }
    ratings = malloc(count * sizeof(*ratings));
    left = malloc(count * sizeof(*left));
    candies = malloc(count * sizeof(*candies));
    if (ratings == NULL || left == NULL || candies == NULL) {
        fprintf(stderr, "Unable to allocate candy arrays.\n");
        free(ratings);
        free(left);
        free(candies);
        return EXIT_FAILURE;
    }
    for (i = 0U; i < count; ++i) {
        if (!read_rating(&ratings[i])) {
            fprintf(stderr, "Ratings must be valid signed integers.\n");
            free(ratings);
            free(left);
            free(candies);
            return EXIT_FAILURE;
        }
    }

    left[0] = 1U;
    for (i = 1U; i < count; ++i) {
        if (ratings[i] > ratings[i - 1U]) {
            if (left[i - 1U] == SIZE_MAX) {
                fprintf(stderr, "Candy count overflow.\n");
                free(ratings);
                free(left);
                free(candies);
                return EXIT_FAILURE;
            }
            left[i] = left[i - 1U] + 1U;
        } else {
            left[i] = 1U;
        }
    }
    for (i = count; i > 0U; --i) {
        size_t index = i - 1U;
        size_t right_requirement = 1U;

        if (index + 1U < count && ratings[index] > ratings[index + 1U]) {
            if (candies[index + 1U] == SIZE_MAX) {
                fprintf(stderr, "Candy count overflow.\n");
                free(ratings);
                free(left);
                free(candies);
                return EXIT_FAILURE;
            }
            right_requirement = candies[index + 1U] + 1U;
        }
        candies[index] = left[index] > right_requirement ? left[index] : right_requirement;
        if (total > UINTMAX_MAX - (uintmax_t)candies[index]) {
            fprintf(stderr, "Total candy count overflow.\n");
            free(ratings);
            free(left);
            free(candies);
            return EXIT_FAILURE;
        }
        total += (uintmax_t)candies[index];
    }

    printf("Candy vector:");
    for (i = 0U; i < count; ++i) {
        printf(" %zu", candies[i]);
    }
    printf("\nMinimum total candies: %" PRIuMAX "\n", total);
    free(ratings);
    free(left);
    free(candies);
    return EXIT_SUCCESS;
}
