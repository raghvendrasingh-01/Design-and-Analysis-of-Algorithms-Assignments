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

int main(void)
{
    size_t count;
    size_t target;
    size_t *coins = NULL;
    uint64_t *ways = NULL;
    int overflow = 0;

    printf("Coin Change: Number of Combinations\n");
    printf("Enter the number of denominations, the denominations, and the target amount:\n");
    if (!read_size("number of denominations", &count)) {
        return EXIT_FAILURE;
    }
    if (count > SIZE_MAX / sizeof(*coins)) {
        fprintf(stderr, "Too many denominations.\n");
        return EXIT_FAILURE;
    }
    if (count != 0) {
        coins = malloc(count * sizeof(*coins));
        if (coins == NULL) {
            fprintf(stderr, "Unable to allocate the denomination array.\n");
            return EXIT_FAILURE;
        }
    }
    for (size_t i = 0; i < count; ++i) {
        if (!read_size("coin denomination", &coins[i]) || coins[i] == 0) {
            fprintf(stderr, "Coin denominations must be positive.\n");
            free(coins);
            return EXIT_FAILURE;
        }
        for (size_t previous = 0; previous < i; ++previous) {
            if (coins[previous] == coins[i]) {
                fprintf(stderr, "Denominations must be distinct.\n");
                free(coins);
                return EXIT_FAILURE;
            }
        }
    }
    if (!read_size("target amount", &target)) {
        free(coins);
        return EXIT_FAILURE;
    }
    if (target == SIZE_MAX || target + 1 > SIZE_MAX / sizeof(*ways)) {
        fprintf(stderr, "Target amount is too large for the dynamic-programming table.\n");
        free(coins);
        return EXIT_FAILURE;
    }

    ways = calloc(target + 1, sizeof(*ways));
    if (ways == NULL) {
        fprintf(stderr, "Unable to allocate the dynamic-programming table.\n");
        free(coins);
        return EXIT_FAILURE;
    }
    ways[0] = 1;
    for (size_t i = 0; i < count && !overflow; ++i) {
        for (size_t amount = coins[i]; amount <= target; ++amount) {
            if (UINT64_MAX - ways[amount] < ways[amount - coins[i]]) {
                overflow = 1;
                break;
            }
            ways[amount] += ways[amount - coins[i]];
        }
    }

    if (overflow) {
        fprintf(stderr, "The number of combinations exceeds UINT64_MAX; result cannot be represented.\n");
        free(ways);
        free(coins);
        return EXIT_FAILURE;
    }
    printf("Target amount: %zu\n", target);
    printf("Number of combinations: %llu\n", (unsigned long long)ways[target]);

    free(ways);
    free(coins);
    return EXIT_SUCCESS;
}
