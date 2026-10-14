#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * Disjoint Set Union (DSU) on permutation indices 1..n.
 */
typedef struct {
    int *parent;
    int *rank;
    int n;
} DSU;

static DSU *dsu_create(int n)
{
    DSU *dsu = malloc(sizeof(*dsu));
    if (!dsu) return NULL;
    dsu->n = n;
    dsu->parent = malloc((size_t)(n + 1) * sizeof(int));
    dsu->rank   = calloc((size_t)(n + 1), sizeof(int));

    if (!dsu->parent || !dsu->rank) {
        free(dsu->parent);
        free(dsu->rank);
        free(dsu);
        return NULL;
    }

    for (int i = 0; i <= n; ++i) {
        dsu->parent[i] = i;
    }
    return dsu;
}

static void dsu_destroy(DSU *dsu)
{
    if (dsu) {
        free(dsu->parent);
        free(dsu->rank);
        free(dsu);
    }
}

static int dsu_find(DSU *dsu, int i)
{
    if (dsu->parent[i] == i) {
        return i;
    }
    return dsu->parent[i] = dsu_find(dsu, dsu->parent[i]);
}

static void dsu_union(DSU *dsu, int u, int v)
{
    int ru = dsu_find(dsu, u);
    int rv = dsu_find(dsu, v);
    if (ru == rv) return;

    if (dsu->rank[ru] < dsu->rank[rv]) {
        dsu->parent[ru] = rv;
    } else if (dsu->rank[ru] > dsu->rank[rv]) {
        dsu->parent[rv] = ru;
    } else {
        dsu->parent[ru] = rv;
        dsu->rank[rv]++;
    }
}

static int compare_int(const void *a, const void *b)
{
    int val_a = *(const int *)a;
    int val_b = *(const int *)b;
    return (val_a > val_b) - (val_a < val_b);
}

int main(void)
{
    int n, m;
    printf("Lexicographically Smallest Connected Component Permutation\n");
    printf("Enter permutation length n and number of allowable swap pairs m:\n");

    if (scanf("%d %d", &n, &m) != 2) {
        fprintf(stderr, "Error: Invalid input for n and m.\n");
        return EXIT_FAILURE;
    }
    if (n < 1 || m < 0) {
        fprintf(stderr, "Error: n must be >= 1 and m >= 0.\n");
        return EXIT_FAILURE;
    }

    int *A = malloc((size_t)(n + 1) * sizeof(int));
    int *orig_A = malloc((size_t)(n + 1) * sizeof(int));
    if (!A || !orig_A) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        free(A);
        free(orig_A);
        return EXIT_FAILURE;
    }

    printf("Enter the %d elements of permutation A:\n", n);
    for (int i = 1; i <= n; ++i) {
        if (scanf("%d", &A[i]) != 1) {
            fprintf(stderr, "Error: Failed to read permutation element at index %d.\n", i);
            free(A);
            free(orig_A);
            return EXIT_FAILURE;
        }
        orig_A[i] = A[i];
    }

    DSU *dsu = dsu_create(n);
    if (!dsu) {
        fprintf(stderr, "Error: DSU allocation failed.\n");
        free(A);
        free(orig_A);
        return EXIT_FAILURE;
    }

    printf("Enter %d allowable swap pairs (u v):\n", m);
    for (int i = 1; i <= m; ++i) {
        int u, v;
        if (scanf("%d %d", &u, &v) != 2) {
            fprintf(stderr, "Error: Failed to read swap pair %d.\n", i);
            dsu_destroy(dsu);
            free(A);
            free(orig_A);
            return EXIT_FAILURE;
        }
        if (u < 1 || u > n || v < 1 || v > n) {
            fprintf(stderr, "Error: Swap indices out of bounds [1, %d] on pair %d.\n", n, i);
            dsu_destroy(dsu);
            free(A);
            free(orig_A);
            return EXIT_FAILURE;
        }
        dsu_union(dsu, u, v);
    }

    /*
     * Group indices and their values by connected component root.
     * We use a head-next index chain for each root.
     */
    int *head = malloc((size_t)(n + 1) * sizeof(int));
    int *next_idx = malloc((size_t)(n + 1) * sizeof(int));
    int *comp_size = calloc((size_t)(n + 1), sizeof(int));

    if (!head || !next_idx || !comp_size) {
        fprintf(stderr, "Error: Memory allocation failed for component grouping.\n");
        free(head);
        free(next_idx);
        free(comp_size);
        dsu_destroy(dsu);
        free(A);
        free(orig_A);
        return EXIT_FAILURE;
    }

    for (int i = 0; i <= n; ++i) {
        head[i] = 0;
        next_idx[i] = 0;
    }

    /* Build component linked lists preserving increasing index order */
    int *tail = calloc((size_t)(n + 1), sizeof(int));
    if (!tail) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        free(head);
        free(next_idx);
        free(comp_size);
        dsu_destroy(dsu);
        free(A);
        free(orig_A);
        return EXIT_FAILURE;
    }

    for (int i = 1; i <= n; ++i) {
        int root = dsu_find(dsu, i);
        comp_size[root]++;
        if (head[root] == 0) {
            head[root] = i;
            tail[root] = i;
        } else {
            next_idx[tail[root]] = i;
            tail[root] = i;
        }
    }
    free(tail);

    /* Allocate reusable buffers for sorting */
    int *idx_buf = malloc((size_t)(n + 1) * sizeof(int));
    int *val_buf = malloc((size_t)(n + 1) * sizeof(int));
    if (!idx_buf || !val_buf) {
        fprintf(stderr, "Error: Memory allocation failed for sorting buffers.\n");
        free(idx_buf);
        free(val_buf);
        free(head);
        free(next_idx);
        free(comp_size);
        dsu_destroy(dsu);
        free(A);
        free(orig_A);
        return EXIT_FAILURE;
    }

    int component_count = 0;
    for (int r = 1; r <= n; ++r) {
        if (head[r] != 0) {
            component_count++;
            int count = 0;
            int curr = head[r];
            while (curr != 0) {
                idx_buf[count] = curr;
                val_buf[count] = A[curr];
                count++;
                curr = next_idx[curr];
            }

            /* Sort values in ascending order to obtain lexicographically smallest assignment */
            qsort(val_buf, (size_t)count, sizeof(int), compare_int);

            /* Assign sorted values back into sorted indices */
            for (int k = 0; k < count; ++k) {
                A[idx_buf[k]] = val_buf[k];
            }
        }
    }

    printf("\nOriginal Permutation:\n  ");
    for (int i = 1; i <= n; ++i) {
        printf("%d%c", orig_A[i], (i == n ? '\n' : ' '));
    }

    printf("\nConnected Components Identified: %d\n", component_count);
    for (int r = 1; r <= n; ++r) {
        if (comp_size[r] > 0) {
            printf("  Component root %d (size %d): indices { ", r, comp_size[r]);
            int curr = head[r];
            while (curr != 0) {
                printf("%d ", curr);
                curr = next_idx[curr];
            }
            printf("}\n");
        }
    }

    printf("\nLexicographically Smallest Permutation Achievable:\n  ");
    for (int i = 1; i <= n; ++i) {
        printf("%d%c", A[i], (i == n ? '\n' : ' '));
    }

    printf("\nComplexity Analysis:\n");
    printf("  Time Complexity:  O(n log n + m * alpha(n))\n");
    printf("                    - DSU on m swap edges: O(m * alpha(n))\n");
    printf("                    - Grouping indices: O(n)\n");
    printf("                    - Sorting within components: sum(O(|C_i| log |C_i|)) <= O(n log n)\n");
    printf("  Space Complexity: O(n + m) for DSU, grouping chains, and index buffers\n");

    free(idx_buf);
    free(val_buf);
    free(head);
    free(next_idx);
    free(comp_size);
    dsu_destroy(dsu);
    free(A);
    free(orig_A);
    return EXIT_SUCCESS;
}
