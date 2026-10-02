#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <float.h>

#define MAX_KEYS ((size_t)2000U)

static size_t cell(size_t stride, size_t row, size_t column)
{
    return row * stride + column;
}

static void print_indent(size_t depth)
{
    size_t i;
    for (i = 0U; i < depth; ++i) {
        fputs("  ", stdout);
    }
}

static void print_tree(size_t first, size_t last, size_t depth,
                       const char *label, size_t stride,
                       const size_t *root, const long long *keys,
                       const long double *q, const long double *p)
{
    print_indent(depth);
    fputs(label, stdout);
    if (first > last) {
        printf("dummy d%zu (q = %.6Lf)\n", first - 1U, q[first - 1U]);
        return;
    }

    {
        size_t selected = root[cell(stride, first, last)];
        printf("root k%zu = %lld (p = %.6Lf)\n", selected,
               keys[selected - 1U], p[selected - 1U]);
        print_tree(first, selected - 1U, depth + 1U, "left: ", stride,
                   root, keys, q, p);
        print_tree(selected + 1U, last, depth + 1U, "right: ", stride,
                   root, keys, q, p);
    }
}

int main(void)
{
    size_t n;
    size_t stride;
    size_t cells;
    long long *keys = NULL;
    long double *p = NULL;
    long double *q = NULL;
    long double *expected = NULL;
    long double *weight = NULL;
    size_t *root = NULL;
    long double probability_total = 0.0L;
    size_t i;

    printf("Enter number of sorted keys n (0 to %zu): ", MAX_KEYS);
    if (scanf("%zu", &n) != 1 || n > MAX_KEYS) {
        fprintf(stderr, "Error: invalid number of keys.\n");
        return EXIT_FAILURE;
    }
    stride = n + 2U;
    if (stride > SIZE_MAX / stride) {
        fprintf(stderr, "Error: OBST table dimensions overflow.\n");
        return EXIT_FAILURE;
    }
    cells = stride * stride;
    if (cells > SIZE_MAX / sizeof(*expected) ||
        cells > SIZE_MAX / sizeof(*root)) {
        fprintf(stderr, "Error: OBST table is too large.\n");
        return EXIT_FAILURE;
    }

    keys = n == 0U ? NULL : malloc(n * sizeof(*keys));
    p = n == 0U ? NULL : malloc(n * sizeof(*p));
    q = malloc((n + 1U) * sizeof(*q));
    expected = calloc(cells, sizeof(*expected));
    weight = calloc(cells, sizeof(*weight));
    root = calloc(cells, sizeof(*root));
    if ((n > 0U && (keys == NULL || p == NULL)) || q == NULL ||
        expected == NULL || weight == NULL || root == NULL) {
        fprintf(stderr, "Error: unable to allocate OBST tables.\n");
        free(keys);
        free(p);
        free(q);
        free(expected);
        free(weight);
        free(root);
        return EXIT_FAILURE;
    }

    if (n > 0U) {
        printf("Enter sorted distinct keys k1 through k%zu:\n", n);
        for (i = 0U; i < n; ++i) {
            if (scanf("%lld", &keys[i]) != 1 ||
                (i > 0U && keys[i] <= keys[i - 1U])) {
                fprintf(stderr, "Error: keys must be strictly increasing integers.\n");
                free(keys);
                free(p);
                free(q);
                free(expected);
                free(weight);
                free(root);
                return EXIT_FAILURE;
            }
        }
        printf("Enter successful probabilities p1 through p%zu:\n", n);
        for (i = 1U; i <= n; ++i) {
            if (scanf("%Lf", &p[i - 1U]) != 1 || !isfinite(p[i - 1U]) ||
                p[i - 1U] < 0.0L) {
                fprintf(stderr, "Error: probabilities must be finite and non-negative.\n");
                free(keys);
                free(p);
                free(q);
                free(expected);
                free(weight);
                free(root);
                return EXIT_FAILURE;
            }
            probability_total += p[i - 1U];
        }
    }

    printf("Enter unsuccessful probabilities q0 through q%zu:\n", n);
    for (i = 0U; i <= n; ++i) {
        if (scanf("%Lf", &q[i]) != 1 || !isfinite(q[i]) || q[i] < 0.0L) {
            fprintf(stderr, "Error: probabilities must be finite and non-negative.\n");
            free(keys);
            free(p);
            free(q);
            free(expected);
            free(weight);
            free(root);
            return EXIT_FAILURE;
        }
        probability_total += q[i];
    }
    if (!isfinite(probability_total) || fabsl(probability_total - 1.0L) > 1.0e-9L) {
        fprintf(stderr,
                "Error: successful and unsuccessful probabilities must sum to 1 (got %.12Lf).\n",
                probability_total);
        free(keys);
        free(p);
        free(q);
        free(expected);
        free(weight);
        free(root);
        return EXIT_FAILURE;
    }

    for (i = 1U; i <= n + 1U; ++i) {
        expected[cell(stride, i, i - 1U)] = q[i - 1U];
        weight[cell(stride, i, i - 1U)] = q[i - 1U];
    }
    {
        size_t interval;
        for (interval = 1U; interval <= n; ++interval) {
            for (i = 1U; i <= n - interval + 1U; ++i) {
                size_t last = i + interval - 1U;
                size_t candidate_root;
                long double best = LDBL_MAX;

                weight[cell(stride, i, last)] =
                    weight[cell(stride, i, last - 1U)] + p[last - 1U] + q[last];
                for (candidate_root = i; candidate_root <= last;
                     ++candidate_root) {
                    long double candidate =
                        expected[cell(stride, i, candidate_root - 1U)] +
                        expected[cell(stride, candidate_root + 1U, last)] +
                        weight[cell(stride, i, last)];
                    if (candidate < best) {
                        best = candidate;
                        root[cell(stride, i, last)] = candidate_root;
                    }
                }
                expected[cell(stride, i, last)] = best;
            }
        }
    }

    printf("Minimum expected search cost: %.10Lf\n",
           expected[cell(stride, 1U, n)]);
    printf("Optimal tree (dummy leaves are unsuccessful searches):\n");
    print_tree(1U, n, 0U, "", stride, root, keys, q, p);

    free(keys);
    free(p);
    free(q);
    free(expected);
    free(weight);
    free(root);
    return EXIT_SUCCESS;
}
