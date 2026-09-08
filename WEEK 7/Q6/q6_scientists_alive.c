#include <stdio.h>
#include <stdlib.h>

typedef struct { char name[32]; int birth, death; } Scientist;
typedef struct { int year, delta; } Event;

static int compareEvents(const void *a, const void *b) {
    const Event *x = a, *y = b;
    if (x->year != y->year) return x->year - y->year;
    return x->delta - y->delta; /* death (-1) precedes birth (+1) */
}

int main(void) {
    Scientist people[] = {
        {"Ada Lovelace", 1815, 1852}, {"Charles Darwin", 1809, 1882},
        {"Marie Curie", 1867, 1934}, {"Albert Einstein", 1879, 1955},
        {"Nikola Tesla", 1856, 1943}, {"Alan Turing", 1912, 1954}
    };
    int n = (int)(sizeof(people) / sizeof(people[0]));
    Event *events = malloc((size_t)(2 * n) * sizeof(*events));
    if (!events) return EXIT_FAILURE;
    for (int i = 0; i < n; i++) {
        events[2 * i] = (Event){people[i].birth, 1};
        events[2 * i + 1] = (Event){people[i].death, -1};
    }
    qsort(events, (size_t)(2 * n), sizeof(*events), compareEvents);
    int alive = 0, best = 0, bestYear = 0;
    for (int i = 0; i < 2 * n; i++) {
        alive += events[i].delta;
        if (alive > best) { best = alive; bestYear = events[i].year; }
    }
    printf("Scientists: %d\nLargest number alive: %d\nFirst year of maximum: %d\n", n, best, bestYear);
    printf("Events are sorted by year; deaths are processed before births in a tie.\n");
    printf("Time: O(n log n), space: O(n).\n");
    free(events); return 0;
}
