#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define INF 1000000000

/*
 * Adjacency list edge
 */
typedef struct Edge {
    int to;
    struct Edge *next;
} Edge;

/*
 * Undirected Graph representation
 */
typedef struct {
    int num_vertices;
    Edge **head;
} Graph;

static Graph *graph_create(int n)
{
    Graph *g = malloc(sizeof(*g));
    if (!g) return NULL;
    g->num_vertices = n;
    g->head = calloc((size_t)(n + 1), sizeof(Edge *));
    if (!g->head) {
        free(g);
        return NULL;
    }
    return g;
}

static void graph_add_edge_undirected(Graph *g, int u, int v)
{
    Edge *e1 = malloc(sizeof(*e1));
    if (!e1) return;
    e1->to = v;
    e1->next = g->head[u];
    g->head[u] = e1;

    Edge *e2 = malloc(sizeof(*e2));
    if (!e2) return;
    e2->to = u;
    e2->next = g->head[v];
    g->head[v] = e2;
}

static void graph_destroy(Graph *g)
{
    if (!g) return;
    for (int i = 0; i <= g->num_vertices; ++i) {
        Edge *curr = g->head[i];
        while (curr) {
            Edge *tmp = curr->next;
            free(curr);
            curr = tmp;
        }
    }
    free(g->head);
    free(g);
}

/*
 * Standard FIFO Queue for BFS
 */
typedef struct {
    int *data;
    int head;
    int tail;
    int capacity;
} Queue;

static Queue *queue_create(int capacity)
{
    Queue *q = malloc(sizeof(*q));
    if (!q) return NULL;
    q->capacity = capacity;
    q->data = malloc((size_t)capacity * sizeof(int));
    if (!q->data) {
        free(q);
        return NULL;
    }
    q->head = 0;
    q->tail = 0;
    return q;
}

static void queue_destroy(Queue *q)
{
    if (q) {
        free(q->data);
        free(q);
    }
}

static inline bool queue_is_empty(const Queue *q)
{
    return q->head == q->tail;
}

static inline void queue_push(Queue *q, int val)
{
    q->data[q->tail++] = val;
}

static inline int queue_pop(Queue *q)
{
    return q->data[q->head++];
}

/*
 * Multi-Source BFS for Radius-Constrained Voronoi Cell Partitioning
 */
static void solve_voronoi_partitioning(const Graph *g, int K, const int *hubs, int R)
{
    int n = g->num_vertices;
    int *dist = malloc((size_t)(n + 1) * sizeof(int));
    int *owner = malloc((size_t)(n + 1) * sizeof(int));
    if (!dist || !owner) {
        free(dist);
        free(owner);
        return;
    }

    for (int i = 0; i <= n; ++i) {
        dist[i] = INF;
        owner[i] = -1;
    }

    Queue *q = queue_create(n + 10);
    if (!q) {
        free(dist);
        free(owner);
        return;
    }

    // Initialize multi-source queue with hubs in increasing order of index 1..K
    // Hubs are 1-indexed in problem: hub index i in 1..K corresponds to vertex hubs[i]
    for (int i = 1; i <= K; ++i) {
        int h_vertex = hubs[i];
        // If multiple hub indices correspond to the same vertex, smallest index wins
        if (dist[h_vertex] > 0) {
            dist[h_vertex] = 0;
            owner[h_vertex] = i;
            queue_push(q, h_vertex);
        }
    }

    while (!queue_is_empty(q)) {
        int u = queue_pop(q);
        int d_u = dist[u];
        int owner_u = owner[u];

        for (Edge *e = g->head[u]; e != NULL; e = e->next) {
            int v = e->to;
            int new_d = d_u + 1;

            if (new_d < dist[v]) {
                dist[v] = new_d;
                owner[v] = owner_u;
                queue_push(q, v);
            } else if (new_d == dist[v]) {
                // Tie-breaking: smaller numerical hub index wins
                if (owner_u < owner[v]) {
                    owner[v] = owner_u;
                }
            }
        }
    }

    // Apply Radius Threshold R: if dist[v] > R, vertex is outside coverage area
    for (int v = 1; v <= n; ++v) {
        if (dist[v] > R) {
            owner[v] = -1;
        }
    }

    printf("\n=======================================================\n");
    printf(" Multi-Source BFS: Radius-Constrained Voronoi Partitioning\n");
    printf(" Hubs K = %d, Radius Threshold R = %d\n", K, R);
    printf("=======================================================\n");
    printf("%-8s | %-12s | %-12s | %-16s\n", "Vertex", "Shortest Dist", "Assigned Hub", "Status");
    printf("---------+--------------+--------------+-----------------\n");

    for (int v = 1; v <= n; ++v) {
        if (owner[v] == -1) {
            if (dist[v] == INF) {
                printf("%-8d | %-12s | %-12d | %-16s\n", v, "INF", -1, "Disconnected");
            } else {
                printf("%-8d | %-12d | %-12d | %-16s\n", v, dist[v], -1, "Out of Radius (>R)");
            }
        } else {
            printf("%-8d | %-12d | Hub %-8d | %-16s\n", v, dist[v], owner[v], "Assigned");
        }
    }

    printf("\nAssigned Voronoi Cells (owner array [1..|V|]):\n  owner = [");
    for (int v = 1; v <= n; ++v) {
        printf("%d%s", owner[v], (v == n) ? "]\n" : ", ");
    }

    printf("\nComplexity Analysis:\n");
    printf("  Multi-Source Queue: All K hubs enqueued initially in O(K) time.\n");
    printf("  BFS Traversal:      Each vertex and undirected edge visited O(1) times.\n");
    printf("  Tie-Breaking:       O(1) comparison on equidistant arrivals.\n");
    printf("  Time Complexity:    O(|V| + |E|)\n");
    printf("  Space Complexity:   O(|V| + |E|) for adjacency list, queue, and owner arrays.\n");

    queue_destroy(q);
    free(dist);
    free(owner);
}

int main(void)
{
    int V, E;
    if (scanf("%d %d", &V, &E) != 2) {
        fprintf(stderr, "Error: Invalid input for V and E.\n");
        return 1;
    }

    int K, R;
    if (scanf("%d %d", &K, &R) != 2) {
        fprintf(stderr, "Error: Invalid input for K and R.\n");
        return 1;
    }

    int *hubs = malloc((size_t)(K + 1) * sizeof(int));
    if (!hubs) {
        fprintf(stderr, "Error: Memory allocation failed for hubs.\n");
        return 1;
    }

    for (int i = 1; i <= K; ++i) {
        if (scanf("%d", &hubs[i]) != 1) {
            fprintf(stderr, "Error: Invalid hub vertex input.\n");
            free(hubs);
            return 1;
        }
    }

    Graph *g = graph_create(V);
    if (!g) {
        fprintf(stderr, "Error: Failed to create graph.\n");
        free(hubs);
        return 1;
    }

    for (int i = 0; i < E; ++i) {
        int u, v;
        if (scanf("%d %d", &u, &v) != 2) {
            fprintf(stderr, "Error: Invalid edge format.\n");
            free(hubs);
            graph_destroy(g);
            return 1;
        }
        graph_add_edge_undirected(g, u, v);
    }

    solve_voronoi_partitioning(g, K, hubs, R);

    free(hubs);
    graph_destroy(g);
    return 0;
}
