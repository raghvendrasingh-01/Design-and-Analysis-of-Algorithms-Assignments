#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>

/*
 * Priority Queue (Max-Heap) entry with lazy deletion versioning.
 */
typedef struct {
    int root;
    int64_t weight;
    uint32_t version;
} HeapNode;

typedef struct {
    HeapNode *data;
    size_t size;
    size_t capacity;
} MaxHeap;

static MaxHeap *heap_create(size_t capacity)
{
    MaxHeap *h = malloc(sizeof(*h));
    if (!h) return NULL;
    h->data = malloc(capacity * sizeof(HeapNode));
    if (!h->data) {
        free(h);
        return NULL;
    }
    h->size = 0;
    h->capacity = capacity;
    return h;
}

static void heap_destroy(MaxHeap *h)
{
    if (h) {
        free(h->data);
        free(h);
    }
}

static void heap_push(MaxHeap *h, int root, int64_t weight, uint32_t version)
{
    if (h->size >= h->capacity) {
        size_t new_cap = h->capacity * 2 + 16;
        HeapNode *new_data = realloc(h->data, new_cap * sizeof(HeapNode));
        if (!new_data) return;
        h->data = new_data;
        h->capacity = new_cap;
    }

    size_t i = h->size++;
    while (i > 0) {
        size_t parent = (i - 1) / 2;
        if (h->data[parent].weight >= weight) {
            break;
        }
        h->data[i] = h->data[parent];
        i = parent;
    }
    h->data[i] = (HeapNode){root, weight, version};
}

static void heap_pop(MaxHeap *h)
{
    if (h->size == 0) return;
    HeapNode last = h->data[--h->size];
    if (h->size == 0) return;

    size_t i = 0;
    while (i * 2 + 1 < h->size) {
        size_t left = i * 2 + 1;
        size_t right = left + 1;
        size_t best = left;

        if (right < h->size && h->data[right].weight > h->data[left].weight) {
            best = right;
        }
        if (last.weight >= h->data[best].weight) {
            break;
        }
        h->data[i] = h->data[best];
        i = best;
    }
    h->data[i] = last;
}

/*
 * DSU data structure tracking component sizes and vertex weight sums.
 */
typedef struct {
    int *parent;
    int *comp_size;
    int64_t *weight;
    uint32_t *version;
    int V;
    int K;
    MaxHeap *heap;
} GraphDSU;

static GraphDSU *graph_dsu_create(int V, int K, const int64_t *vertex_weights)
{
    GraphDSU *g = malloc(sizeof(*g));
    if (!g) return NULL;

    g->V = V;
    g->K = K;
    g->parent = malloc((size_t)(V + 1) * sizeof(int));
    g->comp_size = malloc((size_t)(V + 1) * sizeof(int));
    g->weight = malloc((size_t)(V + 1) * sizeof(int64_t));
    g->version = malloc((size_t)(V + 1) * sizeof(uint32_t));
    g->heap = heap_create((size_t)V * 2 + 16);

    if (!g->parent || !g->comp_size || !g->weight || !g->version || !g->heap) {
        free(g->parent);
        free(g->comp_size);
        free(g->weight);
        free(g->version);
        heap_destroy(g->heap);
        free(g);
        return NULL;
    }

    for (int i = 1; i <= V; ++i) {
        g->parent[i] = i;
        g->comp_size[i] = 1;
        g->weight[i] = vertex_weights[i];
        g->version[i] = 0;
        if (g->comp_size[i] <= K) {
            heap_push(g->heap, i, g->weight[i], g->version[i]);
        }
    }

    return g;
}

static void graph_dsu_destroy(GraphDSU *g)
{
    if (g) {
        free(g->parent);
        free(g->comp_size);
        free(g->weight);
        free(g->version);
        heap_destroy(g->heap);
        free(g);
    }
}

static int dsu_find(GraphDSU *g, int i)
{
    if (g->parent[i] == i) {
        return i;
    }
    return g->parent[i] = dsu_find(g, g->parent[i]);
}

static void dsu_add_edge(GraphDSU *g, int u, int v)
{
    int ru = dsu_find(g, u);
    int rv = dsu_find(g, v);

    if (ru == rv) {
        /* Already in the same component */
        return;
    }

    /* Merge smaller component into larger component */
    if (g->comp_size[ru] < g->comp_size[rv]) {
        int temp = ru;
        ru = rv;
        rv = temp;
    }

    /* Invalidate former heap entries */
    g->version[ru]++;
    g->version[rv]++;

    g->parent[rv] = ru;
    g->comp_size[ru] += g->comp_size[rv];
    g->weight[ru] += g->weight[rv];

    /* If new merged component satisfies size <= K, push to heap */
    if (g->comp_size[ru] <= g->K) {
        heap_push(g->heap, ru, g->weight[ru], g->version[ru]);
    }
}

/*
 * Returns true if a valid component with size <= K exists, setting *max_w.
 * Returns false if no component satisfies size <= K.
 */
static bool get_max_weight_le_k(GraphDSU *g, int64_t *max_w, int *best_root, int *best_size)
{
    while (g->heap->size > 0) {
        HeapNode top = g->heap->data[0];
        int r = top.root;

        /* Check if heap node is valid */
        if (g->parent[r] == r && top.version == g->version[r] && g->comp_size[r] <= g->K) {
            *max_w = top.weight;
            *best_root = r;
            *best_size = g->comp_size[r];
            return true;
        }
        /* Stale entry; prune */
        heap_pop(g->heap);
    }
    return false;
}

int main(void)
{
    int V, K, E;
    printf("Maximum Weight Component with Size Constraints\n");
    printf("Enter number of vertices V, size threshold K, and number of edges E:\n");

    if (scanf("%d %d %d", &V, &K, &E) != 3) {
        fprintf(stderr, "Error: Invalid input for V, K, and E.\n");
        return EXIT_FAILURE;
    }
    if (V < 1 || K < 1 || E < 0) {
        fprintf(stderr, "Error: V must be >= 1, K >= 1, and E >= 0.\n");
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
            fprintf(stderr, "Error: Failed to read weight for vertex %d.\n", i);
            free(vertex_weights);
            return EXIT_FAILURE;
        }
    }

    GraphDSU *g = graph_dsu_create(V, K, vertex_weights);
    if (!g) {
        fprintf(stderr, "Error: DSU initialization failed.\n");
        free(vertex_weights);
        return EXIT_FAILURE;
    }

    int64_t max_w;
    int best_root, best_size;

    printf("\nInitial state before adding edges (threshold K = %d):\n", K);
    if (get_max_weight_le_k(g, &max_w, &best_root, &best_size)) {
        printf("  Max component weight (size <= %d): %" PRId64 " (Component root: %d, size: %d)\n",
               K, max_w, best_root, best_size);
    } else {
        printf("  No connected component satisfies size <= %d\n", K);
    }

    printf("\nProcessing %d edge additions sequentially (u v edge_weight):\n", E);
    for (int step = 1; step <= E; ++step) {
        int u, v;
        int64_t edge_w;
        if (scanf("%d %d %" SCNd64, &u, &v, &edge_w) != 3) {
            fprintf(stderr, "Error: Failed to read edge %d.\n", step);
            graph_dsu_destroy(g);
            free(vertex_weights);
            return EXIT_FAILURE;
        }

        if (u < 1 || u > V || v < 1 || v > V) {
            fprintf(stderr, "Error: Edge %d vertices out of bounds [1, %d].\n", step, V);
            graph_dsu_destroy(g);
            free(vertex_weights);
            return EXIT_FAILURE;
        }

        dsu_add_edge(g, u, v);

        if (get_max_weight_le_k(g, &max_w, &best_root, &best_size)) {
            printf("  Edge %2d: (%d, %d, wt=%" PRId64 ") -> Max valid weight = %" PRId64 " (Root %d, size %d)\n",
                   step, u, v, edge_w, max_w, best_root, best_size);
        } else {
            printf("  Edge %2d: (%d, %d, wt=%" PRId64 ") -> All components exceed size threshold K=%d\n",
                   step, u, v, edge_w, K);
        }
    }

    printf("\nComplexity Analysis:\n");
    printf("  Time Complexity:  O(V + E * (alpha(V) + log V)) = O(V + E log V)\n");
    printf("                    - DSU operations with path compression & union-by-size: O(alpha(V))\n");
    printf("                    - Priority Queue (Max-Heap) with lazy deletion: O(log V) per merge\n");
    printf("  Space Complexity: O(V) for DSU parent, size, weight arrays, and lazy Max-Heap\n");

    graph_dsu_destroy(g);
    free(vertex_weights);
    return EXIT_SUCCESS;
}
