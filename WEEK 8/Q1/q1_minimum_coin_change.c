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
    size_t *minimum = NULL;

    printf("Minimum Coin Change\n");
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
    }
    if (!read_size("target amount", &target)) {
        free(coins);
        return EXIT_FAILURE;
    }
    if (target == SIZE_MAX || target + 1 > SIZE_MAX / sizeof(*minimum)) {
        fprintf(stderr, "Target amount is too large for the dynamic-programming table.\n");
        free(coins);
        return EXIT_FAILURE;
    }

    minimum = malloc((target + 1) * sizeof(*minimum));
    if (minimum == NULL) {
        fprintf(stderr, "Unable to allocate the dynamic-programming table.\n");
        free(coins);
        return EXIT_FAILURE;
    }
    for (size_t amount = 0; amount <= target; ++amount) {
        minimum[amount] = target + 1;
    }
    minimum[0] = 0;

    for (size_t amount = 1; amount <= target; ++amount) {
        for (size_t i = 0; i < count; ++i) {
            if (coins[i] <= amount && minimum[amount - coins[i]] != target + 1) {
                size_t candidate = minimum[amount - coins[i]] + 1;
                if (candidate < minimum[amount]) {
                    minimum[amount] = candidate;
                }
            }
        }
    }

    printf("Target amount: %zu\n", target);
    if (minimum[target] == target + 1) {
        printf("Minimum coins: -1 (the target cannot be formed)\n");
    } else {
        printf("Minimum coins: %zu\n", minimum[target]);
    }

    free(minimum);
    free(coins);
    return EXIT_SUCCESS;
}
