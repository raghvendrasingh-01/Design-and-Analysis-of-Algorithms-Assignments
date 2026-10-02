#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef enum {
    STEP_MATCH,
    STEP_INSERT,
    STEP_DELETE,
    STEP_SUBSTITUTE
} StepKind;

typedef struct {
    StepKind kind;
    size_t source_position;
    size_t target_position;
    char source_character;
    char target_character;
} TraceStep;

static char *read_line(const char *prompt)
{
    size_t length = 0U;
    size_t capacity = 32U;
    char *text = NULL;
    int character;

    fputs(prompt, stdout);
    text = malloc(capacity);
    if (text == NULL) {
        return NULL;
    }

    while ((character = getchar()) != '\n' && character != EOF) {
        if (length + 1U >= capacity) {
            size_t new_capacity;
            char *grown;

            if (capacity > SIZE_MAX / 2U) {
                free(text);
                return NULL;
            }
            new_capacity = capacity * 2U;
            grown = realloc(text, new_capacity);
            if (grown == NULL) {
                free(text);
                return NULL;
            }
            text = grown;
            capacity = new_capacity;
        }
        text[length++] = (char)character;
    }

    if (length > 0U && text[length - 1U] == '\r') {
        --length;
    }
    text[length] = '\0';
    return text;
}

static int checked_cell_count(size_t rows, size_t columns, size_t *count)
{
    if (rows == 0U || columns == 0U || rows > SIZE_MAX / columns) {
        return 0;
    }
    *count = rows * columns;
    return 1;
}

static void print_trace_step(size_t number, const TraceStep *step)
{
    switch (step->kind) {
    case STEP_MATCH:
        printf("%zu. MATCH '%c' (source %zu -> target %zu)\n", number,
               step->source_character, step->source_position,
               step->target_position);
        break;
    case STEP_INSERT:
        printf("%zu. INSERT '%c' at target position %zu\n", number,
               step->target_character, step->target_position);
        break;
    case STEP_DELETE:
        printf("%zu. DELETE '%c' from source position %zu\n", number,
               step->source_character, step->source_position);
        break;
    case STEP_SUBSTITUTE:
        printf("%zu. SUBSTITUTE '%c' -> '%c' (source %zu, target %zu)\n",
               number, step->source_character, step->target_character,
               step->source_position, step->target_position);
        break;
    }
}

int main(void)
{
    char *source = read_line("Enter source string A: ");
    char *target;
    size_t source_length;
    size_t target_length;
    size_t columns;
    size_t cells;
    size_t *distance;
    TraceStep *trace;
    size_t trace_capacity;
    size_t trace_length = 0U;
    size_t i;
    size_t j;

    if (source == NULL) {
        fprintf(stderr, "Error: could not read the source string.\n");
        return EXIT_FAILURE;
    }
    target = read_line("Enter target string B: ");
    if (target == NULL) {
        fprintf(stderr, "Error: could not read the target string.\n");
        free(source);
        return EXIT_FAILURE;
    }

    source_length = strlen(source);
    target_length = strlen(target);
    if (target_length == SIZE_MAX || source_length == SIZE_MAX) {
        fprintf(stderr, "Error: string is too long.\n");
        free(source);
        free(target);
        return EXIT_FAILURE;
    }
    columns = target_length + 1U;
    if (!checked_cell_count(source_length + 1U, columns, &cells) ||
        cells > SIZE_MAX / sizeof(*distance)) {
        fprintf(stderr, "Error: edit-distance table is too large.\n");
        free(source);
        free(target);
        return EXIT_FAILURE;
    }
    distance = calloc(cells, sizeof(*distance));
    if (distance == NULL) {
        fprintf(stderr, "Error: unable to allocate edit-distance table.\n");
        free(source);
        free(target);
        return EXIT_FAILURE;
    }

    for (i = 1U; i <= source_length; ++i) {
        distance[i * columns] = i;
    }
    for (j = 1U; j <= target_length; ++j) {
        distance[j] = j;
    }
    for (i = 1U; i <= source_length; ++i) {
        for (j = 1U; j <= target_length; ++j) {
            size_t deletion = distance[(i - 1U) * columns + j] + 1U;
            size_t insertion = distance[i * columns + j - 1U] + 1U;
            size_t substitution = distance[(i - 1U) * columns + j - 1U] +
                                  (source[i - 1U] != target[j - 1U]);
            size_t best = deletion < insertion ? deletion : insertion;
            if (substitution < best) {
                best = substitution;
            }
            distance[i * columns + j] = best;
        }
    }

    if (source_length > SIZE_MAX - target_length) {
        fprintf(stderr, "Error: traceback is too large.\n");
        free(distance);
        free(source);
        free(target);
        return EXIT_FAILURE;
    }
    trace_capacity = source_length + target_length;
    if (trace_capacity == SIZE_MAX ||
        (trace_capacity > 0U && trace_capacity > SIZE_MAX / sizeof(*trace))) {
        fprintf(stderr, "Error: traceback is too large.\n");
        free(distance);
        free(source);
        free(target);
        return EXIT_FAILURE;
    }
    trace = malloc((trace_capacity == 0U ? 1U : trace_capacity) *
                   sizeof(*trace));
    if (trace == NULL) {
        fprintf(stderr, "Error: unable to allocate traceback.\n");
        free(distance);
        free(source);
        free(target);
        return EXIT_FAILURE;
    }

    i = source_length;
    j = target_length;
    while (i > 0U || j > 0U) {
        TraceStep step;
        size_t current = distance[i * columns + j];

        if (i > 0U && j > 0U &&
            current == distance[(i - 1U) * columns + j - 1U] +
                           (source[i - 1U] != target[j - 1U])) {
            step.source_position = i;
            step.target_position = j;
            step.source_character = source[i - 1U];
            step.target_character = target[j - 1U];
            step.kind = source[i - 1U] == target[j - 1U]
                            ? STEP_MATCH
                            : STEP_SUBSTITUTE;
            --i;
            --j;
        } else if (i > 0U &&
                   current == distance[(i - 1U) * columns + j] + 1U) {
            step.kind = STEP_DELETE;
            step.source_position = i;
            step.target_position = j;
            step.source_character = source[i - 1U];
            step.target_character = '\0';
            --i;
        } else if (j > 0U &&
                   current == distance[i * columns + j - 1U] + 1U) {
            step.kind = STEP_INSERT;
            step.source_position = i;
            step.target_position = j;
            step.source_character = '\0';
            step.target_character = target[j - 1U];
            --j;
        } else {
            fprintf(stderr, "Error: traceback invariant failed.\n");
            free(trace);
            free(distance);
            free(source);
            free(target);
            return EXIT_FAILURE;
        }
        trace[trace_length++] = step;
    }

    printf("Minimum edit distance: %zu\n", distance[source_length * columns +
                                                    target_length]);
    printf("Traceback (in forward order):\n");
    for (i = trace_length; i > 0U; --i) {
        print_trace_step(trace_length - i + 1U, &trace[i - 1U]);
    }
    printf("Transformed string: %s\n", target);

    free(trace);
    free(distance);
    free(source);
    free(target);
    return EXIT_SUCCESS;
}
