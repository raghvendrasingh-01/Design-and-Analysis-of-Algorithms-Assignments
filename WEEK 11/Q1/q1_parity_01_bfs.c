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
    int weight; // 0 or 1
    struct Edge *next;
} Edge;

/*
 * Graph representation using adjacency lists
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

static void graph_add_edge(Graph *g, int u, int v, int w)
{
    Edge *e = malloc(sizeof(*e));
    if (!e) return;
    e->to = v;
    e->weight = w;
    e->next = g->head[u];
    g->head[u] = e;
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
 * Double-ended queue (Deque) for 0-1 BFS
 * State in deque is represented as (u, p), where:
 * u = vertex index (1..V)
 * p = parity (0 for even number of weight-1 edges, 1 for odd)
 */
typedef struct {
    int u;
    int p;
} State;

typedef struct {
    State *data;
    int head;
    int tail;
    int capacity;
} Deque;

static Deque *deque_create(int capacity)
{
    Deque *dq = malloc(sizeof(*dq));
    if (!dq) return NULL;
    dq->capacity = capacity;
    dq->data = malloc((size_t)capacity * sizeof(State));
    if (!dq->data) {
        free(dq);
        return NULL;
    }
    dq->head = capacity / 2;
    dq->tail = capacity / 2;
    return dq;
}

static void deque_destroy(Deque *dq)
{
    if (dq) {
        free(dq->data);
        free(dq);
    }
}

static inline bool deque_is_empty(const Deque *dq)
{
    return dq->head == dq->tail;
}

static inline void deque_push_front(Deque *dq, State s)
{
    dq->head--;
    dq->data[dq->head] = s;
}

static inline void deque_push_back(Deque *dq, State s)
{
    dq->data[dq->tail] = s;
    dq->tail++;
}

static inline State deque_pop_front(Deque *dq)
{
    State s = dq->data[dq->head];
    dq->head++;
    return s;
}

/*
 * Solve Shortest Path on Parity-Constrained Graph using 0-1 BFS
 * on state graph V' = V x {0, 1}.
 */
static void solve_parity_01_bfs(const Graph *g, int S, int C)
{
    int n = g->num_vertices;
    // dist[u][p] stores min weight W(P) to reach (u, p)
    int (*dist)[2] = malloc((size_t)(n + 1) * sizeof(*dist));
    if (!dist) return;

    for (int i = 0; i <= n; ++i) {
        dist[i][0] = INF;
        dist[i][1] = INF;
    }

    // Capacity: each state (u, p) enters at most once, total 2*V states
    // Allocate generous capacity for double-ended expansion
    int cap = 4 * n + 100;
    Deque *dq = deque_create(cap);
    if (!dq) {
        free(dist);
        return;
    }

    dist[S][0] = 0;
    deque_push_back(dq, (State){S, 0});

    while (!deque_is_empty(dq)) {
        State cur = deque_pop_front(dq);
        int u = cur.u;
        int p = cur.p;
        int d = dist[u][p];

        for (Edge *e = g->head[u]; e != NULL; e = e->next) {
            int v = e->to;
            int w = e->weight;
            int next_p = p ^ w;
            int next_d = d + w;

            if (next_d < dist[v][next_p]) {
                dist[v][next_p] = next_d;
                if (w == 0) {
                    deque_push_front(dq, (State){v, next_p});
                } else {
                    deque_push_back(dq, (State){v, next_p});
                }
            }
        }
    }

    printf("\n=======================================================\n");
    printf(" Shortest Path on Parity-Constrained Graphs via 0-1 BFS\n");
    printf(" Source S = %d, Odd Parity Penalty C = %d\n", S, C);
    printf("=======================================================\n");
    printf("%-8s | %-12s | %-12s | %-12s\n", "Vertex", "Cost(Even)", "Cost(Odd)", "Min Cost");
    printf("---------+--------------+--------------+-------------\n");

    for (int v = 1; v <= n; ++v) {
        int cost_even = (dist[v][0] == INF) ? INF : dist[v][0];
        int cost_odd  = (dist[v][1] == INF) ? INF : dist[v][1] + C;

        if (cost_even == INF && cost_odd == INF) {
            printf("%-8d | %-12s | %-12s | %-12s\n", v, "Unreachable", "Unreachable", "Unreachable");
        } else {
            int min_cost = (cost_even < cost_odd) ? cost_even : cost_odd;
            char buf_even[32], buf_odd[32], buf_min[32];

            if (cost_even == INF) snprintf(buf_even, sizeof(buf_even), "INF");
            else snprintf(buf_even, sizeof(buf_even), "%d (W=%d)", cost_even, dist[v][0]);

            if (cost_odd == INF) snprintf(buf_odd, sizeof(buf_odd), "INF");
            else snprintf(buf_odd, sizeof(buf_odd), "%d (W=%d)", cost_odd, dist[v][1]);

            snprintf(buf_min, sizeof(buf_min), "%d", min_cost);
            printf("%-8d | %-12s | %-12s | %-12s\n", v, buf_even, buf_odd, buf_min);
        }
    }

    printf("\nComplexity Analysis:\n");
    printf("  State Space:       |V'| = 2 * |V| vertices, |E'| = 2 * |E| directed edges\n");
    printf("  Queue Operations:  0-1 BFS using Deque (O(1) push_front for w=0, push_back for w=1)\n");
    printf("  Time Complexity:   O(|V| + |E|)\n");
    printf("  Space Complexity:  O(|V| + |E|) for adjacency list and state distances\n");

    deque_destroy(dq);
    free(dist);
}

int main(void)
{
    int V, E;
    if (scanf("%d %d", &V, &E) != 2) {
        fprintf(stderr, "Error: Invalid input for V and E.\n");
        return 1;
    }

    int S, C;
    if (scanf("%d %d", &S, &C) != 2) {
        fprintf(stderr, "Error: Invalid input for S and C.\n");
        return 1;
    }

    Graph *g = graph_create(V);
    if (!g) {
        fprintf(stderr, "Error: Failed to allocate graph.\n");
        return 1;
    }

    for (int i = 0; i < E; ++i) {
        int u, v, w;
        if (scanf("%d %d %d", &u, &v, &w) != 3) {
            fprintf(stderr, "Error: Invalid edge format.\n");
            graph_destroy(g);
            return 1;
        }
        if (w != 0 && w != 1) {
            fprintf(stderr, "Error: Edge weight must be 0 or 1.\n");
            graph_destroy(g);
            return 1;
        }
        graph_add_edge(g, u, v, w);
    }

    solve_parity_01_bfs(g, S, C);
    graph_destroy(g);

    return 0;
}
