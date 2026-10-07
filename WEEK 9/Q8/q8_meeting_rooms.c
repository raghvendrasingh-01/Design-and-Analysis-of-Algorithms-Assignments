#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int64_t start;
    int64_t end;
    size_t original_index;
} Meeting;

typedef struct {
    int64_t end;
    size_t room;
} EndNode;

static char *read_line(const char *prompt)
{
    size_t length = 0U;
    size_t capacity = 32U;
    char *line = malloc(capacity);
    int character;

    fputs(prompt, stdout);
    if (line == NULL) {
        return NULL;
    }
    while ((character = getchar()) != '\n' && character != EOF) {
        if (character == 0) {
            free(line);
            return NULL;
        }
        if (length + 1U >= capacity) {
            char *grown;
            if (capacity > SIZE_MAX / 2U) {
                free(line);
                return NULL;
            }
            capacity *= 2U;
            grown = realloc(line, capacity);
            if (grown == NULL) {
                free(line);
                return NULL;
            }
            line = grown;
        }
        line[length++] = (char)character;
    }
    if (character == EOF && length == 0U) {
        free(line);
        return NULL;
    }
    if (length > 0U && line[length - 1U] == '\r') {
        --length;
    }
    line[length] = '\0';
    return line;
}

static int parse_size(const char *text, size_t *value)
{
    char *end;
    unsigned long long parsed;
    while (*text == ' ' || *text == '\t') {
        ++text;
    }
    if (*text == '-' || *text == '\0') {
        return 0;
    }
    errno = 0;
    parsed = strtoull(text, &end, 10);
    if (errno == ERANGE || end == text) {
        return 0;
    }
    while (*end == ' ' || *end == '\t') {
        ++end;
    }
    if (*end != '\0' || parsed > (unsigned long long)SIZE_MAX) {
        return 0;
    }
    *value = (size_t)parsed;
    return 1;
}

static int parse_two_i64(const char *text, int64_t *first, int64_t *second)
{
    char *end;
    long long left;
    long long right;
    while (*text == ' ' || *text == '\t') {
        ++text;
    }
    errno = 0;
    left = strtoll(text, &end, 10);
    if (errno == ERANGE || end == text) {
        return 0;
    }
    text = end;
    if (*text != ' ' && *text != '\t') {
        return 0;
    }
    while (*text == ' ' || *text == '\t') {
        ++text;
    }
    errno = 0;
    right = strtoll(text, &end, 10);
    if (errno == ERANGE || end == text) {
        return 0;
    }
    while (*end == ' ' || *end == '\t') {
        ++end;
    }
    if (*end != '\0') {
        return 0;
    }
    *first = (int64_t)left;
    *second = (int64_t)right;
    return 1;
}

static int meeting_before(Meeting left, Meeting right)
{
    if (left.start != right.start) {
        return left.start < right.start;
    }
    if (left.end != right.end) {
        return left.end < right.end;
    }
    return left.original_index < right.original_index;
}

static void sift_meetings(Meeting *meetings, size_t n, size_t position)
{
    while (position < n / 2U) {
        size_t child = position * 2U + 1U;
        Meeting temporary;
        if (child + 1U < n && meeting_before(meetings[child], meetings[child + 1U])) {
            ++child;
        }
        if (!meeting_before(meetings[position], meetings[child])) {
            break;
        }
        temporary = meetings[position];
        meetings[position] = meetings[child];
        meetings[child] = temporary;
        position = child;
    }
}

static void sort_meetings(Meeting *meetings, size_t n)
{
    size_t i;
    for (i = n / 2U; i > 0U; --i) {
        sift_meetings(meetings, n, i - 1U);
    }
    for (i = n; i > 1U; --i) {
        Meeting temporary = meetings[0];
        meetings[0] = meetings[i - 1U];
        meetings[i - 1U] = temporary;
        sift_meetings(meetings, i - 1U, 0U);
    }
}

static int end_before(EndNode left, EndNode right)
{
    return left.end < right.end ||
           (left.end == right.end && left.room < right.room);
}

static void end_push(EndNode *heap, size_t *length, EndNode node)
{
    size_t position = (*length)++;
    heap[position] = node;
    while (position > 0U) {
        size_t parent = (position - 1U) / 2U;
        EndNode temporary;
        if (end_before(heap[parent], heap[position])) {
            break;
        }
        temporary = heap[parent];
        heap[parent] = heap[position];
        heap[position] = temporary;
        position = parent;
    }
}

static EndNode end_pop(EndNode *heap, size_t *length)
{
    EndNode result = heap[0];
    size_t position = 0U;
    --*length;
    if (*length == 0U) {
        return result;
    }
    heap[0] = heap[*length];
    for (;;) {
        size_t left = position * 2U + 1U;
        size_t right = left + 1U;
        size_t best = position;
        EndNode temporary;
        if (left < *length && end_before(heap[left], heap[best])) {
            best = left;
        }
        if (right < *length && end_before(heap[right], heap[best])) {
            best = right;
        }
        if (best == position) {
            break;
        }
        temporary = heap[position];
        heap[position] = heap[best];
        heap[best] = temporary;
        position = best;
    }
    return result;
}

static int room_before(size_t left, size_t right)
{
    return left < right;
}

static void room_push(size_t *heap, size_t *length, size_t room)
{
    size_t position = (*length)++;
    heap[position] = room;
    while (position > 0U) {
        size_t parent = (position - 1U) / 2U;
        size_t temporary;
        if (room_before(heap[parent], heap[position])) {
            break;
        }
        temporary = heap[parent];
        heap[parent] = heap[position];
        heap[position] = temporary;
        position = parent;
    }
}

static size_t room_pop(size_t *heap, size_t *length)
{
    size_t result = heap[0];
    size_t position = 0U;
    --*length;
    if (*length == 0U) {
        return result;
    }
    heap[0] = heap[*length];
    for (;;) {
        size_t left = position * 2U + 1U;
        size_t right = left + 1U;
        size_t best = position;
        size_t temporary;
        if (left < *length && room_before(heap[left], heap[best])) {
            best = left;
        }
        if (right < *length && room_before(heap[right], heap[best])) {
            best = right;
        }
        if (best == position) {
            break;
        }
        temporary = heap[position];
        heap[position] = heap[best];
        heap[best] = temporary;
        position = best;
    }
    return result;
}

int main(void)
{
    char *line = read_line("Enter number of meetings n: ");
    size_t n;
    Meeting *meetings;
    size_t *assignments;
    EndNode *active;
    size_t *available;
    size_t active_length = 0U;
    size_t available_length = 0U;
    size_t room_count = 0U;
    size_t i;

    if (line == NULL || !parse_size(line, &n)) {
        fprintf(stderr, "Error: n must be a non-negative integer.\n");
        free(line);
        return EXIT_FAILURE;
    }
    free(line);
    if (n > SIZE_MAX / sizeof(*meetings) || n > SIZE_MAX / sizeof(*active) ||
        n > SIZE_MAX / sizeof(*assignments)) {
        fprintf(stderr, "Error: meeting list is too large.\n");
        return EXIT_FAILURE;
    }
    meetings = n == 0U ? NULL : malloc(n * sizeof(*meetings));
    assignments = n == 0U ? NULL : malloc(n * sizeof(*assignments));
    active = n == 0U ? NULL : malloc(n * sizeof(*active));
    available = n == 0U ? NULL : malloc(n * sizeof(*available));
    if (n != 0U && (meetings == NULL || assignments == NULL || active == NULL ||
                    available == NULL)) {
        fprintf(stderr, "Error: unable to allocate meeting buffers.\n");
        free(meetings);
        free(assignments);
        free(active);
        free(available);
        return EXIT_FAILURE;
    }
    for (i = 0U; i < n; ++i) {
        int64_t start;
        int64_t end;
        char prompt[64];
        (void)snprintf(prompt, sizeof(prompt),
                       "Enter start and end for meeting %zu: ", i + 1U);
        line = read_line(prompt);
        if (line == NULL || !parse_two_i64(line, &start, &end) || start >= end) {
            fprintf(stderr, "Error: each interval must contain start < end.\n");
            free(line);
            free(meetings);
            free(assignments);
            free(active);
            free(available);
            return EXIT_FAILURE;
        }
        free(line);
        meetings[i] = (Meeting){start, end, i};
    }
    sort_meetings(meetings, n);
    for (i = 0U; i < n; ++i) {
        while (active_length != 0U && active[0].end <= meetings[i].start) {
            EndNode finished = end_pop(active, &active_length);
            room_push(available, &available_length, finished.room);
        }
        {
            size_t room;
            if (available_length != 0U) {
                room = room_pop(available, &available_length);
            } else {
                room = room_count++;
            }
            assignments[meetings[i].original_index] = room;
            end_push(active, &active_length, (EndNode){meetings[i].end, room});
        }
    }
    for (i = 0U; i < room_count; ++i) {
        active[i].room = SIZE_MAX;
    }
    for (i = 0U; i < n; ++i) {
        size_t room = assignments[meetings[i].original_index];
        if (active[room].room != SIZE_MAX && active[room].end > meetings[i].start) {
            fprintf(stderr, "Error: assignment validation failed.\n");
            free(meetings);
            free(assignments);
            free(active);
            free(available);
            return EXIT_FAILURE;
        }
        active[room] = (EndNode){meetings[i].end, room};
    }
    printf("Minimum rooms required: %zu\n", room_count);
    printf("Assignments (input order, room numbers start at 1):\n");
    for (i = 0U; i < n; ++i) {
        printf("Meeting %zu -> room %zu\n", i + 1U, assignments[i] + 1U);
    }
    printf("Validation: intervals sharing a room do not overlap (end == start is reusable).\n");

    free(meetings);
    free(assignments);
    free(active);
    free(available);
    return EXIT_SUCCESS;
}
