#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *read_line(void)
{
    size_t length = 0;
    size_t capacity = 16;
    char *line = malloc(capacity);
    int character;

    if (line == NULL) {
        return NULL;
    }
    while ((character = getchar()) != '\n' && character != EOF) {
        if (length + 1 >= capacity) {
            size_t new_capacity;
            char *grown;

            if (capacity > SIZE_MAX / 2) {
                free(line);
                return NULL;
            }
            new_capacity = capacity * 2;
            grown = realloc(line, new_capacity);
            if (grown == NULL) {
                free(line);
                return NULL;
            }
            line = grown;
            capacity = new_capacity;
        }
        line[length++] = (char)character;
    }
    if (character == EOF && length == 0) {
        free(line);
        return NULL;
    }
    if (length > 0 && line[length - 1] == '\r') {
        --length;
    }
    line[length] = '\0';
    return line;
}

int main(void)
{
    char *first;
    char *second;
    size_t first_length;
    size_t second_length;
    size_t columns;
    size_t rows;
    size_t *table;
    size_t lcs_length;
    char *answer;
    size_t i;
    size_t j;
    size_t write_at;

    printf("Longest Common Subsequence\n");
    printf("Enter the first sequence on one line and the second sequence on the next:\n");
    first = read_line();
    second = read_line();
    if (first == NULL || second == NULL) {
        fprintf(stderr, "Two input lines are required.\n");
        free(first);
        free(second);
        return EXIT_FAILURE;
    }
    first_length = strlen(first);
    second_length = strlen(second);
    if (second_length == SIZE_MAX || first_length == SIZE_MAX) {
        fprintf(stderr, "Input is too large.\n");
        free(first);
        free(second);
        return EXIT_FAILURE;
    }
    columns = second_length + 1;
    rows = first_length + 1;
    if (rows > SIZE_MAX / columns || rows * columns > SIZE_MAX / sizeof(*table)) {
        fprintf(stderr, "The dynamic-programming table is too large.\n");
        free(first);
        free(second);
        return EXIT_FAILURE;
    }
    table = calloc(rows * columns, sizeof(*table));
    if (table == NULL) {
        fprintf(stderr, "Unable to allocate the dynamic-programming table.\n");
        free(first);
        free(second);
        return EXIT_FAILURE;
    }

    for (i = 1; i <= first_length; ++i) {
        for (j = 1; j <= second_length; ++j) {
            if (first[i - 1] == second[j - 1]) {
                table[i * columns + j] = table[(i - 1) * columns + j - 1] + 1;
            } else {
                size_t above = table[(i - 1) * columns + j];
                size_t left = table[i * columns + j - 1];
                table[i * columns + j] = above > left ? above : left;
            }
        }
    }

    lcs_length = table[first_length * columns + second_length];
    if (lcs_length == SIZE_MAX) {
        fprintf(stderr, "The reconstructed subsequence is too large.\n");
        free(table);
        free(first);
        free(second);
        return EXIT_FAILURE;
    }
    answer = malloc(lcs_length + 1);
    if (answer == NULL) {
        fprintf(stderr, "Unable to allocate the reconstructed subsequence.\n");
        free(table);
        free(first);
        free(second);
        return EXIT_FAILURE;
    }
    answer[lcs_length] = '\0';
    i = first_length;
    j = second_length;
    write_at = lcs_length;
    while (i > 0 && j > 0) {
        if (first[i - 1] == second[j - 1]) {
            answer[--write_at] = first[i - 1];
            --i;
            --j;
        } else if (table[(i - 1) * columns + j] >= table[i * columns + j - 1]) {
            --i;
        } else {
            --j;
        }
    }

    printf("LCS length: %zu\n", lcs_length);
    printf("LCS: %s\n", answer);
    free(answer);
    free(table);
    free(first);
    free(second);
    return EXIT_SUCCESS;
}
