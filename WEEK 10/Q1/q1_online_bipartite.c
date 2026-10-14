#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * Disjoint Set Union (DSU) with 2-coloring parity tracking.
 * Each node maintains:
 * - parent[i]: parent pointer in the forest
 * - parity[i]: relative color difference between node i and parent[i] (0 or 1)
 * - rank[i]: rank for union-by-rank heuristic
 */
typedef struct {
    int *parent;
    int *parity;
    int *rank;
    int n;
} BipartiteDSU;

static BipartiteDSU *dsu_create(int n)
{
    BipartiteDSU *dsu = malloc(sizeof(*dsu));
    if (!dsu) return NULL;

    dsu->n = n;
    dsu->parent = malloc((size_t)(n + 1) * sizeof(int));
    dsu->parity = malloc((size_t)(n + 1) * sizeof(int));
    dsu->rank   = malloc((size_t)(n + 1) * sizeof(int));

    if (!dsu->parent || !dsu->parity || !dsu->rank) {
        free(dsu->parent);
        free(dsu->parity);
        free(dsu->rank);
        free(dsu);
        return NULL;
    }

    for (int i = 0; i <= n; ++i) {
        dsu->parent[i] = i;
        dsu->parity[i] = 0;
        dsu->rank[i]   = 0;
    }
    return dsu;
}

static void dsu_destroy(BipartiteDSU *dsu)
{
    if (dsu) {
        free(dsu->parent);
        free(dsu->parity);
        free(dsu->rank);
        free(dsu);
    }
}

/*
 * Find root with path compression while maintaining parity
 * parity[u] becomes the color difference between u and root(u).
 */
static int dsu_find(BipartiteDSU *dsu, int u, int *parity_out)
{
    if (dsu->parent[u] == u) {
        *parity_out = 0;
        return u;
    }
    int p_par;
    int root = dsu_find(dsu, dsu->parent[u], &p_par);
    dsu->parity[u] ^= p_par;
    dsu->parent[u] = root;
    *parity_out = dsu->parity[u];
    return root;
}

/*
 * Attempt to add edge (u, v).
 * Returns:
 *   1 if edge connects two components and is accepted
 *   2 if edge creates an even cycle within the same component (accepted)
 *   0 if edge would form an odd cycle and violates bipartiteness (rejected)
 */
static int dsu_add_edge(BipartiteDSU *dsu, int u, int v)
{
    if (u == v) {
        /* Self-loop is an odd cycle of length 1 */
        return 0;
    }

    int pu, pv;
    int ru = dsu_find(dsu, u, &pu);
    int rv = dsu_find(dsu, v, &pv);

    if (ru == rv) {
        /* Same connected component */
        if (pu == pv) {
            /* Same color parity -> odd cycle formed! */
            return 0;
        }
        /* Different color parity -> even cycle, graph remains bipartite */
        return 2;
    }

    /* Merge two different components */
    /* Required: color(ru) ^ color(rv) = pu ^ pv ^ 1 */
    int rel_parity = pu ^ pv ^ 1;

    if (dsu->rank[ru] < dsu->rank[rv]) {
        dsu->parent[ru] = rv;
        dsu->parity[ru] = rel_parity;
    } else if (dsu->rank[ru] > dsu->rank[rv]) {
        dsu->parent[rv] = ru;
        dsu->parity[rv] = rel_parity;
    } else {
        dsu->parent[ru] = rv;
        dsu->parity[ru] = rel_parity;
        dsu->rank[rv]++;
    }

    return 1;
}

int main(void)
{
    int V, E;
    printf("Online Graph-Bipartiteness Testing\n");
    printf("Enter number of vertices V and number of edge operations E:\n");

    if (scanf("%d %d", &V, &E) != 2) {
        fprintf(stderr, "Error: Invalid input for V and E.\n");
        return EXIT_FAILURE;
    }
    if (V < 1 || E < 0) {
        fprintf(stderr, "Error: V must be >= 1 and E >= 0.\n");
        return EXIT_FAILURE;
    }

    BipartiteDSU *dsu = dsu_create(V);
    if (!dsu) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return EXIT_FAILURE;
    }

    printf("Processing %d edge operations online (vertices 1 to %d):\n", E, V);
    int accepted_count = 0;
    int rejected_count = 0;

    for (int i = 1; i <= E; ++i) {
        int u, v;
        if (scanf("%d %d", &u, &v) != 2) {
            fprintf(stderr, "Error: Failed to read edge %d.\n", i);
            dsu_destroy(dsu);
            return EXIT_FAILURE;
        }

        if (u < 1 || u > V || v < 1 || v > V) {
            fprintf(stderr, "Error: Vertex index out of bounds [1, %d] on edge %d.\n", V, i);
            dsu_destroy(dsu);
            return EXIT_FAILURE;
        }

        int status = dsu_add_edge(dsu, u, v);
        if (status == 1) {
            printf("  Edge %d (%d, %d): ACCEPTED (tree edge, components merged)\n", i, u, v);
            accepted_count++;
        } else if (status == 2) {
            printf("  Edge %d (%d, %d): ACCEPTED (even cycle formed, bipartiteness preserved)\n", i, u, v);
            accepted_count++;
        } else {
            printf("  Edge %d (%d, %d): REJECTED (odd cycle detected, violates bipartiteness!)\n", i, u, v);
            rejected_count++;
        }
    }

    printf("\nExecution Summary:\n");
    printf("  Total edges streamed: %d\n", E);
    printf("  Accepted edges:       %d\n", accepted_count);
    printf("  Rejected edges:       %d\n", rejected_count);
    printf("  Bipartite invariant:  MAINTAINED\n");

    printf("\nVertex 2-Coloring Assignment:\n");
    for (int i = 1; i <= V; ++i) {
        int parity;
        int root = dsu_find(dsu, i, &parity);
        printf("  Vertex %d: Component Root = %d, Color = %d\n", i, root, parity);
    }

    printf("\nComplexity Analysis:\n");
    printf("  Time Complexity:  O(V + E * alpha(V)) where alpha is the inverse Ackermann function.\n");
    printf("  Space Complexity: O(V) for DSU parent, rank, and parity arrays.\n");

    dsu_destroy(dsu);
    return EXIT_SUCCESS;
}
