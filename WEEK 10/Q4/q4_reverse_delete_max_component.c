#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>

/*
 * Adjacency list for undirected graph.
 */
typedef struct EdgeNode {
    int to;
    struct EdgeNode *next;
} EdgeNode;

typedef struct {
    int V;
    EdgeNode **head;
} Graph;

static Graph *graph_create(int V)
{
    Graph *g = malloc(sizeof(*g));
    if (!g) return NULL;
    g->V = V;
    g->head = calloc((size_t)(V + 1), sizeof(EdgeNode *));
    if (!g->head) {
        free(g);
        return NULL;
    }
    return g;
}

static void graph_add_edge(Graph *g, int u, int v)
{
    EdgeNode *e1 = malloc(sizeof(*e1));
    if (!e1) return;
    e1->to = v;
    e1->next = g->head[u];
    g->head[u] = e1;

    EdgeNode *e2 = malloc(sizeof(*e2));
    if (!e2) return;
    e2->to = u;
    e2->next = g->head[v];
    g->head[v] = e2;
}

static void graph_destroy(Graph *g)
{
    if (g) {
        for (int i = 1; i <= g->V; ++i) {
            EdgeNode *curr = g->head[i];
            while (curr) {
                EdgeNode *next = curr->next;
                free(curr);
                curr = next;
            }
        }
        free(g->head);
        free(g);
    }
}

/*
 * Disjoint Set Union (DSU) tracking component vertex sum.
 */
typedef struct {
    int *parent;
    int *rank;
    int64_t *comp_weight;
} DSU;

static DSU *dsu_create(int V)
{
    DSU *dsu = malloc(sizeof(*dsu));
    if (!dsu) return NULL;
    dsu->parent = malloc((size_t)(V + 1) * sizeof(int));
    dsu->rank = calloc((size_t)(V + 1), sizeof(int));
    dsu->comp_weight = calloc((size_t)(V + 1), sizeof(int64_t));

    if (!dsu->parent || !dsu->rank || !dsu->comp_weight) {
        free(dsu->parent);
        free(dsu->rank);
        free(dsu->comp_weight);
        free(dsu);
        return NULL;
    }

    for (int i = 0; i <= V; ++i) {
        dsu->parent[i] = i;
    }
    return dsu;
}

static void dsu_destroy(DSU *dsu)
{
    if (dsu) {
        free(dsu->parent);
        free(dsu->rank);
        free(dsu->comp_weight);
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

static int dsu_union(DSU *dsu, int root1, int root2)
{
    if (root1 == root2) return root1;

    if (dsu->rank[root1] < dsu->rank[root2]) {
        dsu->parent[root1] = root2;
        dsu->comp_weight[root2] += dsu->comp_weight[root1];
        return root2;
    } else if (dsu->rank[root1] > dsu->rank[root2]) {
        dsu->parent[root2] = root1;
        dsu->comp_weight[root1] += dsu->comp_weight[root2];
        return root1;
    } else {
        dsu->parent[root1] = root2;
        dsu->comp_weight[root2] += dsu->comp_weight[root1];
        dsu->rank[root2]++;
        return root2;
    }
}

int main(void)
{
    int V, M;
    printf("Reverse Time-Step DSU Application (Sequential Vertex Deletions)\n");
    printf("Enter number of vertices V and number of edges M:\n");

    if (scanf("%d %d", &V, &M) != 2) {
        fprintf(stderr, "Error: Invalid input for V and M.\n");
        return EXIT_FAILURE;
    }
    if (V < 1 || M < 0) {
        fprintf(stderr, "Error: V must be >= 1 and M >= 0.\n");
        return EXIT_FAILURE;
    }

    int64_t *vertex_weights = malloc((size_t)(V + 1) * sizeof(int64_t));
    if (!vertex_weights) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return EXIT_FAILURE;
    }

    printf("Enter vertex weights w_1 through w_%d:\n", V);
    for (int i = 1; i <= V; ++i) {
        if (scanf("%" SCNd64, &vertex_weights[i]) != 1) {
            fprintf(stderr, "Error: Failed to read vertex weight %d.\n", i);
            free(vertex_weights);
            return EXIT_FAILURE;
        }
    }

    Graph *g = graph_create(V);
    if (!g) {
        fprintf(stderr, "Error: Failed to allocate graph.\n");
        free(vertex_weights);
        return EXIT_FAILURE;
    }

    printf("Enter %d undirected edges (u v):\n", M);
    for (int i = 0; i < M; ++i) {
        int u, v;
        if (scanf("%d %d", &u, &v) != 2) {
            fprintf(stderr, "Error: Failed to read edge %d.\n", i + 1);
            graph_destroy(g);
            free(vertex_weights);
            return EXIT_FAILURE;
        }
        if (u < 1 || u > V || v < 1 || v > V) {
            fprintf(stderr, "Error: Edge %d vertices out of bounds [1, %d].\n", i + 1, V);
            graph_destroy(g);
            free(vertex_weights);
            return EXIT_FAILURE;
        }
        graph_add_edge(g, u, v);
    }

    int *P = malloc((size_t)(V + 1) * sizeof(int));
    if (!P) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        graph_destroy(g);
        free(vertex_weights);
        return EXIT_FAILURE;
    }

    printf("Enter deletion permutation order P = [p_1, ..., p_%d]:\n", V);
    for (int i = 1; i <= V; ++i) {
        if (scanf("%d", &P[i]) != 1) {
            fprintf(stderr, "Error: Failed to read deletion order at index %d.\n", i);
            free(P);
            graph_destroy(g);
            free(vertex_weights);
            return EXIT_FAILURE;
        }
        if (P[i] < 1 || P[i] > V) {
            fprintf(stderr, "Error: Invalid vertex %d in deletion sequence.\n", P[i]);
            free(P);
            graph_destroy(g);
            free(vertex_weights);
            return EXIT_FAILURE;
        }
    }

    /*
     * Offline Reverse Time Processing:
     * Deletions in forward order: P[1], P[2], ..., P[V].
     * In reverse time, start from an empty graph at step V (answer is 0).
     * At step t (from V down to 2), "add" vertex P[t], connect with already active neighbors,
     * and update running_max. The max component sum after deleting P[t - 1] is running_max.
     */
    DSU *dsu = dsu_create(V);
    bool *is_active = calloc((size_t)(V + 1), sizeof(bool));
    int64_t *ans = calloc((size_t)(V + 1), sizeof(int64_t));

    if (!dsu || !is_active || !ans) {
        fprintf(stderr, "Error: Memory allocation failed for simulation.\n");
        free(P);
        graph_destroy(g);
        free(vertex_weights);
        dsu_destroy(dsu);
        free(is_active);
        free(ans);
        return EXIT_FAILURE;
    }

    int64_t running_max = 0;
    ans[V] = 0; /* All vertices deleted -> 0 */

    for (int t = V; t >= 2; --t) {
        int u = P[t];
        is_active[u] = true;
        dsu->comp_weight[u] = vertex_weights[u];
        if (dsu->comp_weight[u] > running_max) {
            running_max = dsu->comp_weight[u];
        }

        for (EdgeNode *e = g->head[u]; e; e = e->next) {
            int v = e->to;
            if (is_active[v]) {
                int ru = dsu_find(dsu, u);
                int rv = dsu_find(dsu, v);
                if (ru != rv) {
                    int r_new = dsu_union(dsu, ru, rv);
                    if (dsu->comp_weight[r_new] > running_max) {
                        running_max = dsu->comp_weight[r_new];
                    }
                }
            }
        }
        ans[t - 1] = running_max;
    }

    printf("\nDeletions and Maximum Remaining Component Weight:\n");
    printf("%-6s | %-14s | %-20s\n", "Step", "Deleted Vertex", "Max Component Weight");
    printf("-------+----------------+---------------------\n");
    for (int i = 1; i <= V; ++i) {
        printf(" %-5d | Vertex %-7d | %" PRId64 "\n", i, P[i], ans[i]);
    }

    printf("\nComplexity Analysis:\n");
    printf("  Time Complexity:  O(V + M * alpha(V)) using offline reverse time DSU with path compression and union-by-rank.\n");
    printf("  Space Complexity: O(V + M) for adjacency list, DSU, and query buffers.\n");

    free(P);
    graph_destroy(g);
    free(vertex_weights);
    dsu_destroy(dsu);
    free(is_active);
    free(ans);
    return EXIT_SUCCESS;
}
