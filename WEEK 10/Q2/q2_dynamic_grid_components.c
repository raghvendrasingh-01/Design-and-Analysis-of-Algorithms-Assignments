#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*
 * Disjoint Set Union (DSU) structure for grid cells.
 * Coordinate flattening: cell (r, c) -> index = r * C + c.
 */
typedef struct {
    int *parent;
    int *rank;
    int *is_land;
    int R;
    int C;
    int total_cells;
    int island_count;
} GridDSU;

static GridDSU *grid_dsu_create(int R, int C)
{
    GridDSU *g = malloc(sizeof(*g));
    if (!g) return NULL;

    g->R = R;
    g->C = C;
    g->total_cells = R * C;
    g->island_count = 0;

    g->parent  = malloc((size_t)g->total_cells * sizeof(int));
    g->rank    = malloc((size_t)g->total_cells * sizeof(int));
    g->is_land = calloc((size_t)g->total_cells, sizeof(int));

    if (!g->parent || !g->rank || !g->is_land) {
        free(g->parent);
        free(g->rank);
        free(g->is_land);
        free(g);
        return NULL;
    }

    for (int i = 0; i < g->total_cells; ++i) {
        g->parent[i] = i;
        g->rank[i]   = 0;
    }
    return g;
}

static void grid_dsu_destroy(GridDSU *g)
{
    if (g) {
        free(g->parent);
        free(g->rank);
        free(g->is_land);
        free(g);
    }
}

static int grid_find(GridDSU *g, int i)
{
    if (g->parent[i] == i) {
        return i;
    }
    return g->parent[i] = grid_find(g, g->parent[i]);
}

static void grid_union(GridDSU *g, int root1, int root2)
{
    if (g->rank[root1] < g->rank[root2]) {
        g->parent[root1] = root2;
    } else if (g->rank[root1] > g->rank[root2]) {
        g->parent[root2] = root1;
    } else {
        g->parent[root1] = root2;
        g->rank[root2]++;
    }
}

/*
 * Adds land cell at (r, c) (0-indexed).
 * Merges with any of the 4 adjacent orthogonally neighboring land cells.
 * Returns the updated number of islands.
 */
static int add_land(GridDSU *g, int r, int c, bool *was_already_land)
{
    int id = r * g->C + c;

    if (g->is_land[id]) {
        *was_already_land = true;
        return g->island_count;
    }

    *was_already_land = false;
    g->is_land[id] = 1;
    g->island_count++;

    static const int dr[4] = {-1, 1, 0, 0};
    static const int dc[4] = {0, 0, -1, 1};

    for (int d = 0; d < 4; ++d) {
        int nr = r + dr[d];
        int nc = c + dc[d];

        if (nr >= 0 && nr < g->R && nc >= 0 && nc < g->C) {
            int nid = nr * g->C + nc;
            if (g->is_land[nid]) {
                int r1 = grid_find(g, id);
                int r2 = grid_find(g, nid);
                if (r1 != r2) {
                    grid_union(g, r1, r2);
                    g->island_count--;
                }
            }
        }
    }
    return g->island_count;
}

static void print_ascii_grid(const GridDSU *g)
{
    printf("\nFinal Grid Map (. = Water, # = Land):\n");
    for (int r = 0; r < g->R; ++r) {
        printf("  ");
        for (int c = 0; c < g->C; ++c) {
            int id = r * g->C + c;
            printf("%c ", g->is_land[id] ? '#' : '.');
        }
        printf("\n");
    }
}

int main(void)
{
    int R, C, K;
    printf("Dynamic Connected Components in a Grid (Number of Islands II)\n");
    printf("Enter grid dimensions R C and number of land additions K:\n");

    if (scanf("%d %d %d", &R, &C, &K) != 3) {
        fprintf(stderr, "Error: Invalid input for R, C, and K.\n");
        return EXIT_FAILURE;
    }
    if (R <= 0 || C <= 0 || K < 0) {
        fprintf(stderr, "Error: Dimensions must be positive and K >= 0.\n");
        return EXIT_FAILURE;
    }

    GridDSU *g = grid_dsu_create(R, C);
    if (!g) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return EXIT_FAILURE;
    }

    printf("Processing %d land additions (0-indexed coordinates 0..%d, 0..%d):\n", K, R - 1, C - 1);

    for (int step = 1; step <= K; ++step) {
        int r, c;
        if (scanf("%d %d", &r, &c) != 2) {
            fprintf(stderr, "Error: Failed to read land coordinate at step %d.\n", step);
            grid_dsu_destroy(g);
            return EXIT_FAILURE;
        }

        if (r < 0 || r >= R || c < 0 || c >= C) {
            fprintf(stderr, "Error: Coordinates (%d, %d) out of bounds at step %d.\n", r, c, step);
            grid_dsu_destroy(g);
            return EXIT_FAILURE;
        }

        bool already_land = false;
        int current_islands = add_land(g, r, c, &already_land);

        if (already_land) {
            printf("  Step %2d: Add (%d, %d) -> Already land! Current islands = %d\n",
                   step, r, c, current_islands);
        } else {
            printf("  Step %2d: Add (%d, %d) -> Current islands = %d\n",
                   step, r, c, current_islands);
        }
    }

    if (R <= 20 && C <= 30) {
        print_ascii_grid(g);
    }

    printf("\nExecution Summary:\n");
    printf("  Grid dimensions:     %d x %d (%d cells)\n", R, C, R * C);
    printf("  Land cells placed:   %d operations\n", K);
    printf("  Final island count:  %d\n", g->island_count);

    printf("\nComplexity Analysis:\n");
    printf("  Time Complexity:  O(R * C + K * alpha(R * C)) where alpha is the inverse Ackermann function.\n");
    printf("  Space Complexity: O(R * C) for DSU parent, rank, and land arrays.\n");

    grid_dsu_destroy(g);
    return EXIT_SUCCESS;
}
