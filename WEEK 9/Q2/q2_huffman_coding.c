#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    uintmax_t frequency;
    size_t symbol;
    size_t minimum_symbol;
    struct Node *left;
    struct Node *right;
} Node;

typedef struct {
    Node **data;
    size_t length;
    size_t capacity;
} MinHeap;

typedef struct {
    unsigned char symbol;
    uintmax_t frequency;
    size_t length;
    char *code;
} CodeEntry;

static int read_size(const char *name, size_t *result)
{
    char token[128];
    char *end;
    uintmax_t number;

    if (scanf("%127s", token) != 1 || token[0] == '-') {
        fprintf(stderr, "Invalid %s.\n", name);
        return 0;
    }
    errno = 0;
    number = strtoumax(token, &end, 10);
    if (errno == ERANGE || *end != '\0' || number > (uintmax_t)SIZE_MAX) {
        fprintf(stderr, "Invalid %s.\n", name);
        return 0;
    }
    *result = (size_t)number;
    return 1;
}

static int read_frequency(uintmax_t *result)
{
    char token[128];
    char *end;
    uintmax_t number;

    if (scanf("%127s", token) != 1 || token[0] == '-') {
        return 0;
    }
    errno = 0;
    number = strtoumax(token, &end, 10);
    if (errno == ERANGE || *end != '\0' || number == 0U) {
        return 0;
    }
    *result = number;
    return 1;
}

static int node_less(const Node *left, const Node *right)
{
    if (left->frequency != right->frequency) {
        return left->frequency < right->frequency;
    }
    return left->minimum_symbol < right->minimum_symbol;
}

static void heap_swap(Node **left, Node **right)
{
    Node *temporary = *left;
    *left = *right;
    *right = temporary;
}

static void heap_up(MinHeap *heap, size_t index)
{
    while (index > 0U) {
        size_t parent = (index - 1U) / 2U;
        if (!node_less(heap->data[index], heap->data[parent])) {
            break;
        }
        heap_swap(&heap->data[index], &heap->data[parent]);
        index = parent;
    }
}

static void heap_down(MinHeap *heap, size_t index)
{
    for (;;) {
        size_t smallest = index;
        size_t left = index * 2U + 1U;
        size_t right = left + 1U;

        if (left < heap->length && node_less(heap->data[left], heap->data[smallest])) {
            smallest = left;
        }
        if (right < heap->length && node_less(heap->data[right], heap->data[smallest])) {
            smallest = right;
        }
        if (smallest == index) {
            break;
        }
        heap_swap(&heap->data[index], &heap->data[smallest]);
        index = smallest;
    }
}

static void free_tree(Node *node)
{
    if (node != NULL) {
        free_tree(node->left);
        free_tree(node->right);
        free(node);
    }
}

static int assign_lengths(const Node *node, size_t depth, size_t *lengths)
{
    if (node->left == NULL && node->right == NULL) {
        lengths[node->symbol] = depth == 0U ? 1U : depth;
        return 1;
    }
    if (node->left == NULL || node->right == NULL || depth == SIZE_MAX) {
        return 0;
    }
    return assign_lengths(node->left, depth + 1U, lengths) &&
           assign_lengths(node->right, depth + 1U, lengths);
}

static int compare_codes(const void *left, const void *right)
{
    const CodeEntry *a = left;
    const CodeEntry *b = right;

    if (a->length != b->length) {
        return a->length < b->length ? -1 : 1;
    }
    return a->symbol < b->symbol ? -1 : (a->symbol > b->symbol ? 1 : 0);
}

static int increment_bits(char *bits, size_t length)
{
    size_t position = length;

    while (position > 0U && bits[position - 1U] == '1') {
        bits[position - 1U] = '0';
        --position;
    }
    if (position == 0U) {
        return 0;
    }
    bits[position - 1U] = '1';
    return 1;
}

int main(void)
{
    size_t count;
    size_t i;
    size_t *lengths = NULL;
    Node **nodes = NULL;
    CodeEntry *codes = NULL;
    MinHeap heap;
    Node *root = NULL;
    char *bits = NULL;
    size_t current_length = 0U;
    long double weighted_length = 0.0L;
    uintmax_t total_frequency = 0U;
    int success = 0;

    heap.data = NULL;
    heap.length = 0U;
    heap.capacity = 0U;
    printf("Huffman Coding\n");
    printf("Enter n, then n pairs of one-character symbol and positive frequency:\n");
    if (!read_size("number of symbols", &count) || count == 0U) {
        fprintf(stderr, "The number of symbols must be positive.\n");
        return EXIT_FAILURE;
    }
    if (count > SIZE_MAX / sizeof(*nodes) || count > SIZE_MAX / sizeof(*lengths) ||
        count > SIZE_MAX / sizeof(*codes)) {
        fprintf(stderr, "Too many symbols.\n");
        return EXIT_FAILURE;
    }
    nodes = calloc(count, sizeof(*nodes));
    lengths = calloc(count, sizeof(*lengths));
    codes = calloc(count, sizeof(*codes));
    heap.data = malloc(count * sizeof(*heap.data));
    if (nodes == NULL || lengths == NULL || codes == NULL || heap.data == NULL) {
        fprintf(stderr, "Unable to allocate Huffman structures.\n");
        goto cleanup;
    }
    heap.capacity = count;

    for (i = 0U; i < count; ++i) {
        char token[16];
        uintmax_t frequency;
        size_t j;

        if (scanf("%15s", token) != 1 || strlen(token) != 1U ||
            !read_frequency(&frequency)) {
            fprintf(stderr, "Each symbol must be one character followed by a positive frequency.\n");
            goto cleanup;
        }
        for (j = 0U; j < i; ++j) {
            if (codes[j].symbol == (unsigned char)token[0]) {
                fprintf(stderr, "Duplicate symbol '%c'.\n", token[0]);
                goto cleanup;
            }
        }
        codes[i].symbol = (unsigned char)token[0];
        codes[i].frequency = frequency;
        if (total_frequency > UINTMAX_MAX - frequency) {
            fprintf(stderr, "Total frequency overflow.\n");
            goto cleanup;
        }
        total_frequency += frequency;
        nodes[i] = malloc(sizeof(*nodes[i]));
        if (nodes[i] == NULL) {
            fprintf(stderr, "Unable to allocate a Huffman node.\n");
            goto cleanup;
        }
        nodes[i]->frequency = frequency;
        nodes[i]->symbol = i;
        nodes[i]->minimum_symbol = (unsigned char)token[0];
        nodes[i]->left = NULL;
        nodes[i]->right = NULL;
        heap.data[heap.length++] = nodes[i];
        heap_up(&heap, heap.length - 1U);
    }

    while (heap.length > 1U) {
        Node *left;
        Node *right;
        Node *parent;

        left = heap.data[0];
        heap.data[0] = heap.data[--heap.length];
        heap_down(&heap, 0U);
        right = heap.data[0];
        heap.data[0] = heap.data[--heap.length];
        if (heap.length > 0U) {
            heap_down(&heap, 0U);
        }
        if (left->frequency > UINTMAX_MAX - right->frequency) {
            fprintf(stderr, "Huffman frequency overflow.\n");
            goto cleanup;
        }
        parent = malloc(sizeof(*parent));
        if (parent == NULL) {
            fprintf(stderr, "Unable to allocate an internal Huffman node.\n");
            goto cleanup;
        }
        parent->frequency = left->frequency + right->frequency;
        parent->symbol = 0U;
        parent->minimum_symbol = left->minimum_symbol < right->minimum_symbol
                                     ? left->minimum_symbol : right->minimum_symbol;
        parent->left = left;
        parent->right = right;
        heap.data[heap.length++] = parent;
        heap_up(&heap, heap.length - 1U);
    }
    root = heap.data[0];
    if (!assign_lengths(root, 0U, lengths)) {
        fprintf(stderr, "Unable to assign Huffman code lengths.\n");
        goto cleanup;
    }
    for (i = 0U; i < count; ++i) {
        codes[i].length = lengths[i];
    }
    qsort(codes, count, sizeof(*codes), compare_codes);
    if (codes[0].length == 0U || codes[0].length == SIZE_MAX) {
        fprintf(stderr, "Code length is too large.\n");
        goto cleanup;
    }
    bits = calloc(codes[0].length + 1U, sizeof(*bits));
    if (bits == NULL) {
        fprintf(stderr, "Unable to allocate canonical code.\n");
        goto cleanup;
    }
    memset(bits, '0', codes[0].length);
    current_length = codes[0].length;
    for (i = 0U; i < count; ++i) {
        if (i > 0U) {
            size_t new_length = codes[i].length;
            char *grown;

            if (new_length < current_length || !increment_bits(bits, current_length)) {
                fprintf(stderr, "Invalid Huffman length set.\n");
                goto cleanup;
            }
            if (new_length == SIZE_MAX || new_length + 1U > SIZE_MAX) {
                fprintf(stderr, "Code length is too large.\n");
                goto cleanup;
            }
            grown = realloc(bits, new_length + 1U);
            if (grown == NULL) {
                fprintf(stderr, "Unable to grow canonical code.\n");
                goto cleanup;
            }
            bits = grown;
            memset(bits + current_length, '0', new_length - current_length);
            bits[new_length] = '\0';
            current_length = new_length;
        }
        codes[i].code = malloc(codes[i].length + 1U);
        if (codes[i].code == NULL) {
            fprintf(stderr, "Unable to allocate a code string.\n");
            goto cleanup;
        }
        memcpy(codes[i].code, bits, codes[i].length + 1U);
        weighted_length += (long double)codes[i].frequency * (long double)codes[i].length;
    }

    printf("Canonical codebook (length, then symbol order):\n");
    for (i = 0U; i < count; ++i) {
        printf("  '%c': frequency=%" PRIuMAX ", length=%zu, code=%s\n",
               codes[i].symbol, codes[i].frequency, codes[i].length, codes[i].code);
    }
    printf("Expected code length: %.6Lf bits/symbol\n",
           weighted_length / (long double)total_frequency);
    success = 1;

cleanup:
    if (!success && root == NULL) {
        for (i = 0U; i < heap.length; ++i) {
            free_tree(heap.data[i]);
        }
    } else {
        free_tree(root);
    }
    if (codes != NULL) {
        for (i = 0U; i < count; ++i) {
            free(codes[i].code);
        }
    }
    free(bits);
    free(heap.data);
    free(nodes);
    free(lengths);
    free(codes);
    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
