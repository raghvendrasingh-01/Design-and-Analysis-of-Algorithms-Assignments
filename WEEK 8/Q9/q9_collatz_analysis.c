#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>

#define MAX_TRAJECTORY_STEPS ((size_t)10000000U)
#define MAX_INTERVAL_POINTS UINT64_C(1000000)

typedef enum {
    COLLatz_REACHED_ONE,
    COLLatz_OVERFLOW,
    COLLatz_STEP_LIMIT,
    COLLatz_ALLOCATION_FAILURE
} CollatzStatus;

typedef struct {
    uint64_t *values;
    size_t length;
    size_t capacity;
} Trajectory;

typedef struct {
    uint64_t maximum_value;
    uint64_t maximum_start;
    size_t steps;
    CollatzStatus status;
} StartAnalysis;

static int collatz_next(uint64_t current, uint64_t *next)
{
    if (current % UINT64_C(2) == 0U) {
        *next = current / UINT64_C(2);
        return 1;
    }
    if (current > (UINT64_MAX - UINT64_C(1)) / UINT64_C(3)) {
        return 0;
    }
    *next = current * UINT64_C(3) + UINT64_C(1);
    return 1;
}

static int trajectory_append(Trajectory *trajectory, uint64_t value)
{
    if (trajectory->length == trajectory->capacity) {
        size_t new_capacity;
        uint64_t *grown;

        if (trajectory->capacity == 0U) {
            new_capacity = 16U;
        } else {
            if (trajectory->capacity > SIZE_MAX / 2U) {
                return 0;
            }
            new_capacity = trajectory->capacity * 2U;
        }
        if (new_capacity > SIZE_MAX / sizeof(*trajectory->values)) {
            return 0;
        }
        grown = realloc(trajectory->values,
                        new_capacity * sizeof(*trajectory->values));
        if (grown == NULL) {
            return 0;
        }
        trajectory->values = grown;
        trajectory->capacity = new_capacity;
    }
    trajectory->values[trajectory->length++] = value;
    return 1;
}

static CollatzStatus build_trajectory(uint64_t start, Trajectory *trajectory)
{
    uint64_t current = start;
    size_t steps = 0U;

    if (!trajectory_append(trajectory, current)) {
        return COLLatz_ALLOCATION_FAILURE;
    }
    while (current != UINT64_C(1)) {
        if (steps == MAX_TRAJECTORY_STEPS) {
            return COLLatz_STEP_LIMIT;
        }
        if (!collatz_next(current, &current)) {
            return COLLatz_OVERFLOW;
        }
        ++steps;
        if (!trajectory_append(trajectory, current)) {
            return COLLatz_ALLOCATION_FAILURE;
        }
    }
    return COLLatz_REACHED_ONE;
}

static StartAnalysis analyze_start(uint64_t start)
{
    StartAnalysis analysis = {start, start, 0U, COLLatz_REACHED_ONE};
    uint64_t current = start;

    while (current != UINT64_C(1) && analysis.steps < MAX_TRAJECTORY_STEPS) {
        uint64_t next;
        if (!collatz_next(current, &next)) {
            analysis.status = COLLatz_OVERFLOW;
            return analysis;
        }
        current = next;
        ++analysis.steps;
        if (current > analysis.maximum_value) {
            analysis.maximum_value = current;
            analysis.maximum_start = start;
        }
    }
    if (current != UINT64_C(1)) {
        analysis.status = COLLatz_STEP_LIMIT;
    }
    return analysis;
}

static const char *status_name(CollatzStatus status)
{
    switch (status) {
    case COLLatz_REACHED_ONE:
        return "reached 1";
    case COLLatz_OVERFLOW:
        return "stopped before overflow";
    case COLLatz_STEP_LIMIT:
        return "step limit reached";
    case COLLatz_ALLOCATION_FAILURE:
        return "allocation failure";
    }
    return "unknown status";
}

static int read_positive_uint64(uint64_t *result)
{
    char token[64];
    size_t length = 0U;
    uintmax_t parsed;
    char *end;
    int character;

    do {
        character = getchar();
    } while (character != EOF && isspace((unsigned char)character));
    if (character == EOF) {
        return 0;
    }

    while (character != EOF && !isspace((unsigned char)character)) {
        if (character < '0' || character > '9' ||
            length + 1U >= sizeof(token)) {
            do {
                character = getchar();
            } while (character != EOF && !isspace((unsigned char)character));
            return 0;
        }
        token[length++] = (char)character;
        character = getchar();
    }
    token[length] = '\0';
    errno = 0;
    parsed = strtoumax(token, &end, 10);
    if (errno == ERANGE || *end != '\0' || parsed == 0U ||
        parsed > UINT64_MAX) {
        return 0;
    }
    *result = (uint64_t)parsed;
    return 1;
}

int main(void)
{
    uint64_t start;
    uint64_t lower;
    uint64_t upper;
    uint64_t interval_count;
    Trajectory trajectory = {NULL, 0U, 0U};
    CollatzStatus trajectory_status;
    size_t i;
    uint64_t value;
    uint64_t reached = 0U;
    uint64_t overflowed = 0U;
    uint64_t limited = 0U;
    uint64_t interval_maximum = 0U;
    uint64_t interval_maximum_start = 0U;
    size_t longest_steps = 0U;
    uint64_t longest_start = 0U;

    printf("Enter starting value n (positive integer): ");
    if (!read_positive_uint64(&start)) {
        fprintf(stderr, "Error: n must be a positive 64-bit integer.\n");
        return EXIT_FAILURE;
    }
    trajectory_status = build_trajectory(start, &trajectory);
    if (trajectory_status == COLLatz_ALLOCATION_FAILURE) {
        fprintf(stderr, "Error: unable to allocate the trajectory.\n");
        free(trajectory.values);
        return EXIT_FAILURE;
    }
    printf("Trajectory for %" PRIu64 " (%s):\n", start,
           status_name(trajectory_status));
    for (i = 0U; i < trajectory.length; ++i) {
        if (i > 0U) {
            fputs(" -> ", stdout);
        }
        printf("%" PRIu64, trajectory.values[i]);
    }
    putchar('\n');
    printf("Stopping value: %" PRIu64 "\n", trajectory.values[trajectory.length - 1U]);
    printf("Number of steps recorded: %zu\n", trajectory.length - 1U);
    free(trajectory.values);

    printf("Enter interval [a b] (positive integers, at most %" PRIu64
           " starts): ", MAX_INTERVAL_POINTS);
    if (!read_positive_uint64(&lower) || !read_positive_uint64(&upper) ||
        upper < lower || upper - lower == UINT64_MAX) {
        fprintf(stderr, "Error: invalid interval.\n");
        return EXIT_FAILURE;
    }
    interval_count = upper - lower + UINT64_C(1);
    if (interval_count > MAX_INTERVAL_POINTS) {
        fprintf(stderr, "Error: interval contains too many starts.\n");
        return EXIT_FAILURE;
    }
    longest_start = lower;

    for (value = lower;; ++value) {
        StartAnalysis analysis = analyze_start(value);
        if (analysis.status == COLLatz_REACHED_ONE) {
            ++reached;
        } else if (analysis.status == COLLatz_OVERFLOW) {
            ++overflowed;
        } else {
            ++limited;
        }
        if (analysis.maximum_value > interval_maximum) {
            interval_maximum = analysis.maximum_value;
            interval_maximum_start = value;
        }
        if (analysis.steps > longest_steps) {
            longest_steps = analysis.steps;
            longest_start = value;
        }
        if (value == upper) {
            break;
        }
    }

    printf("Interval analysis [%" PRIu64 ", %" PRIu64 "]:\n", lower, upper);
    printf("Starts analysed: %" PRIu64 "\n", interval_count);
    printf("Reached 1: %" PRIu64 "\n", reached);
    printf("Overflow stops: %" PRIu64 "\n", overflowed);
    printf("Step-limit stops: %" PRIu64 "\n", limited);
    printf("Largest value: %" PRIu64 " (from start %" PRIu64 ")\n",
           interval_maximum, interval_maximum_start);
    printf("Longest recorded trajectory: %zu steps (start %" PRIu64 ")\n",
           longest_steps, longest_start);
    return EXIT_SUCCESS;
}
