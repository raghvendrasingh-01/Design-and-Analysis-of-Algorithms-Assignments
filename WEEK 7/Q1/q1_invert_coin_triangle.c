#include <stdio.h>
#include <stdlib.h>

typedef struct { int x, y; } Point;

static int has(const Point a[], int n, int x, int y) {
    for (int i = 0; i < n; i++) if (a[i].x == x && a[i].y == y) return 1;
    return 0;
}

static int formula(int side) {
    int m = side - 1, q = m / 3, r = m % 3;
    return 3 * q * (q + 1) / 2 + r * (q + 1);
}

/* Count coins that must leave the old triangle for one translated inversion. */
static int movedFor(const Point old[], int count, int side, int dx, int dy) {
    int overlap = 0;
    for (int x = 0; x < side; x++)
        for (int y = 0; y <= x; y++)
            if (has(old, count, y + dx, x + dy)) overlap++;
    return count - overlap;
}

static int bruteMinimum(int side, int *bestDx, int *bestDy) {
    int count = side * (side + 1) / 2, best = count;
    Point *old = malloc(sizeof(*old) * (size_t)count);
    if (!old) exit(EXIT_FAILURE);
    int k = 0;
    for (int x = 0; x < side; x++)
        for (int y = 0; y <= x; y++) old[k++] = (Point){x, y};
    for (int dx = -side; dx <= side; dx++)
        for (int dy = -side; dy <= side; dy++) {
            int moved = movedFor(old, count, side, dx, dy);
            if (moved < best) { best = moved; *bestDx = dx; *bestDy = dy; }
        }
    free(old);
    return best;
}

static void printOverlay(int side, int dx, int dy, int finalShape) {
    int count = side * (side + 1) / 2, k = 0;
    Point *old = malloc(sizeof(*old) * (size_t)count);
    Point *target = malloc(sizeof(*target) * (size_t)count);
    if (!old || !target) exit(EXIT_FAILURE);
    for (int x = 0; x < side; x++)
        for (int y = 0; y <= x; y++) {
            old[k] = (Point){x, y};
            target[k++] = (Point){y + dx, x + dy};
        }
    int minX = 0, maxX = side - 1, minY = 0, maxY = side - 1;
    if (dx < minX) minX = dx;
    if (dy < minY) minY = dy;
    if (dx + side - 1 > maxX) maxX = dx + side - 1;
    if (dy + side - 1 > maxY) maxY = dy + side - 1;
    printf("       ");
    for (int y = minY; y <= maxY; y++) printf("%3d", y);
    putchar('\n');
    for (int x = minX; x <= maxX; x++) {
        printf("x=%2d | ", x);
        for (int y = minY; y <= maxY; y++) {
            int inOld = has(old, count, x, y);
            int inTarget = has(target, count, x, y);
            if (finalShape ? inTarget : inOld)
                printf(" %c ", (inOld && inTarget) ? 'O' : 'X');
            else printf("   ");
        }
        putchar('\n');
    }
    free(old); free(target);
}

static void printChangedPositions(int side, int dx, int dy, int finalShape) {
    int count = side * (side + 1) / 2, k = 0;
    Point *old = malloc(sizeof(*old) * (size_t)count);
    Point *target = malloc(sizeof(*target) * (size_t)count);
    if (!old || !target) exit(EXIT_FAILURE);
    for (int x = 0; x < side; x++)
        for (int y = 0; y <= x; y++) {
            old[k] = (Point){x, y};
            target[k++] = (Point){y + dx, x + dy};
        }
    printf(finalShape ? "New positions (X): " : "Vacated positions (X): ");
    const Point *shown = finalShape ? target : old;
    for (int i = 0; i < count; i++) {
        if (!has(finalShape ? old : target, count, shown[i].x, shown[i].y))
            printf("(%d,%d) ", shown[i].x, shown[i].y);
    }
    putchar('\n');
    free(old); free(target);
}

int main(void) {
    int side = 5, dx = 0, dy = 0;
    int minimum = bruteMinimum(side, &dx, &dy);
    printf("Triangle side: %d\n", side);
    printf("Legend: O = unchanged coin, X = coin position changed\n");
    printf("Optimal translation: (dx, dy) = (%d, %d)\n", dx, dy);
    printf("\nOriginal positions on the common lattice:\n");
    printOverlay(side, dx, dy, 0);
    printChangedPositions(side, dx, dy, 0);
    printf("\nFinal inverted positions on the same lattice:\n");
    printOverlay(side, dx, dy, 1);
    printChangedPositions(side, dx, dy, 1);
    printf("Minimum moves (formula): %d\n", formula(side));
    printf("Minimum moves (lattice validation): %d\n", minimum);
    printf("Formula validated: %s\n", formula(side) == minimum ? "yes" : "no");
    printf("For m=n-1=3q+r, moves = 3q(q+1)/2 + r(q+1).\n");
    return 0;
}
