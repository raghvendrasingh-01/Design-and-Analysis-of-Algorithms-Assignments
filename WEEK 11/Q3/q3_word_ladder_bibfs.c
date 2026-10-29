#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_WORD_LEN 16
#define HASH_TABLE_SIZE 262144 // 2^18 buckets for fast open-addressing / chaining

/*
 * Hash table node for dictionary lookup
 */
typedef struct DictNode {
    char word[MAX_WORD_LEN];
    int id;
    struct DictNode *next;
} DictNode;

typedef struct {
    DictNode **buckets;
    int size;
} HashTable;

static unsigned int hash_string(const char *str)
{
    unsigned int hash = 5381;
    int c;
    while ((c = (unsigned char)*str++)) {
        hash = ((hash << 5) + hash) + (unsigned int)c; /* hash * 33 + c */
    }
    return hash;
}

static HashTable *ht_create(int size)
{
    HashTable *ht = malloc(sizeof(*ht));
    if (!ht) return NULL;
    ht->size = size;
    ht->buckets = calloc((size_t)size, sizeof(DictNode *));
    if (!ht->buckets) {
        free(ht);
        return NULL;
    }
    return ht;
}

static void ht_destroy(HashTable *ht)
{
    if (!ht) return;
    for (int i = 0; i < ht->size; ++i) {
        DictNode *curr = ht->buckets[i];
        while (curr) {
            DictNode *tmp = curr->next;
            free(curr);
            curr = tmp;
        }
    }
    free(ht->buckets);
    free(ht);
}

static int ht_insert(HashTable *ht, const char *word, int id)
{
    unsigned int h = hash_string(word) % (unsigned int)ht->size;
    for (DictNode *node = ht->buckets[h]; node != NULL; node = node->next) {
        if (strcmp(node->word, word) == 0) {
            return node->id; // Already present
        }
    }
    DictNode *new_node = malloc(sizeof(*new_node));
    if (!new_node) return -1;
    strncpy(new_node->word, word, MAX_WORD_LEN - 1);
    new_node->word[MAX_WORD_LEN - 1] = '\0';
    new_node->id = id;
    new_node->next = ht->buckets[h];
    ht->buckets[h] = new_node;
    return id;
}

static int ht_find(const HashTable *ht, const char *word)
{
    unsigned int h = hash_string(word) % (unsigned int)ht->size;
    for (DictNode *node = ht->buckets[h]; node != NULL; node = node->next) {
        if (strcmp(node->word, word) == 0) {
            return node->id;
        }
    }
    return -1;
}

/*
 * Word dictionary representation
 */
typedef struct {
    char (*words)[MAX_WORD_LEN];
    int count;
    int capacity;
    HashTable *ht;
} WordDict;

static WordDict *dict_create(int max_words)
{
    WordDict *d = malloc(sizeof(*d));
    if (!d) return NULL;
    d->capacity = max_words;
    d->count = 0;
    d->words = malloc((size_t)max_words * sizeof(*d->words));
    d->ht = ht_create(HASH_TABLE_SIZE);
    if (!d->words || !d->ht) {
        if (d->words) free(d->words);
        if (d->ht) ht_destroy(d->ht);
        free(d);
        return NULL;
    }
    return d;
}

static void dict_destroy(WordDict *d)
{
    if (d) {
        free(d->words);
        ht_destroy(d->ht);
        free(d);
    }
}

static int dict_add(WordDict *d, const char *word)
{
    int existing = ht_find(d->ht, word);
    if (existing != -1) return existing;

    if (d->count >= d->capacity) return -1;
    int id = d->count++;
    strncpy(d->words[id], word, MAX_WORD_LEN - 1);
    d->words[id][MAX_WORD_LEN - 1] = '\0';
    ht_insert(d->ht, word, id);
    return id;
}

/*
 * Simple FIFO Queue
 */
typedef struct {
    int *data;
    int head;
    int tail;
    int capacity;
} Queue;

static Queue *queue_create(int cap)
{
    Queue *q = malloc(sizeof(*q));
    if (!q) return NULL;
    q->capacity = cap;
    q->data = malloc((size_t)cap * sizeof(int));
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

static inline int queue_size(const Queue *q)
{
    return q->tail - q->head;
}

static inline void queue_push(Queue *q, int v)
{
    q->data[q->tail++] = v;
}

static inline int queue_pop(Queue *q)
{
    return q->data[q->head++];
}

/*
 * Bidirectional Breadth-First Search (BFS)
 */
static int bidirectional_bfs(WordDict *dict, int start_id, int target_id, int word_len, int *parent_fwd, int *parent_bwd, int *meet_node)
{
    if (start_id == target_id) {
        *meet_node = start_id;
        return 0;
    }

    int n = dict->count;
    int *dist_fwd = malloc((size_t)n * sizeof(int));
    int *dist_bwd = malloc((size_t)n * sizeof(int));
    if (!dist_fwd || !dist_bwd) {
        free(dist_fwd);
        free(dist_bwd);
        return -1;
    }

    for (int i = 0; i < n; ++i) {
        dist_fwd[i] = -1;
        dist_bwd[i] = -1;
        parent_fwd[i] = -1;
        parent_bwd[i] = -1;
    }

    Queue *q_fwd = queue_create(n + 10);
    Queue *q_bwd = queue_create(n + 10);
    if (!q_fwd || !q_bwd) {
        free(dist_fwd);
        free(dist_bwd);
        queue_destroy(q_fwd);
        queue_destroy(q_bwd);
        return -1;
    }

    dist_fwd[start_id] = 0;
    queue_push(q_fwd, start_id);

    dist_bwd[target_id] = 0;
    queue_push(q_bwd, target_id);

    char mutated[MAX_WORD_LEN];
    int min_total_dist = -1;
    int best_meeting_vertex = -1;

    // Expand layer by layer from the smaller frontier
    while (!queue_is_empty(q_fwd) && !queue_is_empty(q_bwd)) {
        Queue *curr_q;
        int *curr_dist;
        int *other_dist;
        int *curr_parent;
        bool is_forward;

        if (queue_size(q_fwd) <= queue_size(q_bwd)) {
            curr_q = q_fwd;
            curr_dist = dist_fwd;
            other_dist = dist_bwd;
            curr_parent = parent_fwd;
            is_forward = true;
        } else {
            curr_q = q_bwd;
            curr_dist = dist_bwd;
            other_dist = dist_fwd;
            curr_parent = parent_bwd;
            is_forward = false;
        }

        int level_count = queue_size(curr_q);
        for (int step = 0; step < level_count; ++step) {
            int u = queue_pop(curr_q);
            const char *u_word = dict->words[u];
            int d = curr_dist[u];

            // Mutate each character position from 'a' to 'z'
            for (int pos = 0; pos < word_len; ++pos) {
                memcpy(mutated, u_word, (size_t)word_len + 1);
                char original_char = mutated[pos];

                for (char c = 'a'; c <= 'z'; ++c) {
                    if (c == original_char) continue;
                    mutated[pos] = c;

                    int v = ht_find(dict->ht, mutated);
                    if (v == -1) continue;

                    // Check if opposite frontier has reached v
                    if (other_dist[v] != -1) {
                        int total_path = d + 1 + other_dist[v];
                        if (min_total_dist == -1 || total_path < min_total_dist) {
                            min_total_dist = total_path;
                            best_meeting_vertex = v;
                            curr_parent[v] = u;
                        }
                    }

                    // Standard BFS step in current frontier
                    if (curr_dist[v] == -1) {
                        curr_dist[v] = d + 1;
                        curr_parent[v] = u;
                        queue_push(curr_q, v);
                    }
                }
            }
        }

        // If meeting occurred in this layer, we found the optimal shortest path
        if (min_total_dist != -1) {
            (void)is_forward;
            break;
        }
    }

    *meet_node = best_meeting_vertex;

    queue_destroy(q_fwd);
    queue_destroy(q_bwd);
    free(dist_fwd);
    free(dist_bwd);
    return min_total_dist;
}

static void print_transformation_path(const WordDict *dict, int start_id, int target_id, int meet_node,
                                      const int *parent_fwd, const int *parent_bwd)
{
    if (meet_node == -1) return;

    // Trace path from start to meet_node
    int fwd_path[1000];
    int fwd_len = 0;
    int curr = meet_node;
    while (curr != -1) {
        fwd_path[fwd_len++] = curr;
        if (curr == start_id) break;
        curr = parent_fwd[curr];
    }

    // Trace path from meet_node to target
    int bwd_path[1000];
    int bwd_len = 0;
    curr = meet_node;
    while (curr != -1) {
        if (curr != meet_node) {
            bwd_path[bwd_len++] = curr;
        }
        if (curr == target_id) break;
        curr = parent_bwd[curr];
    }

    printf("\nOptimal Transformation Sequence:\n  ");
    // Print forward in reverse (from start to meet_node)
    for (int i = fwd_len - 1; i >= 0; --i) {
        printf("%s", dict->words[fwd_path[i]]);
        if (i > 0 || bwd_len > 0) printf(" -> ");
    }
    // Print backward (from meet_node's child to target)
    for (int i = 0; i < bwd_len; ++i) {
        printf("%s", dict->words[bwd_path[i]]);
        if (i < bwd_len - 1) printf(" -> ");
    }
    printf("\n");
}

int main(void)
{
    int N;
    if (scanf("%d", &N) != 1) {
        fprintf(stderr, "Error: Invalid dictionary size N.\n");
        return 1;
    }

    char start_word[MAX_WORD_LEN];
    char target_word[MAX_WORD_LEN];
    if (scanf("%15s %15s", start_word, target_word) != 2) {
        fprintf(stderr, "Error: Invalid start or target word.\n");
        return 1;
    }

    int L = (int)strlen(start_word);
    if ((int)strlen(target_word) != L) {
        fprintf(stderr, "Error: Word lengths of start and target must match.\n");
        return 1;
    }

    // Capacity: N dictionary words + possibly start and target words
    WordDict *dict = dict_create(N + 10);
    if (!dict) {
        fprintf(stderr, "Error: Failed to allocate dictionary.\n");
        return 1;
    }

    for (int i = 0; i < N; ++i) {
        char buf[MAX_WORD_LEN];
        if (scanf("%15s", buf) != 1) {
            fprintf(stderr, "Error: Reading dictionary word.\n");
            dict_destroy(dict);
            return 1;
        }
        if ((int)strlen(buf) == L) {
            dict_add(dict, buf);
        }
    }

    // Target B must be in dictionary D
    int target_id = ht_find(dict->ht, target_word);
    if (target_id == -1) {
        printf("\n=======================================================\n");
        printf(" Bidirectional BFS: Word Graph Transformation\n");
        printf(" Start: '%s' -> Target: '%s' (Word Length L = %d)\n", start_word, target_word, L);
        printf("=======================================================\n");
        printf("Error: Target word '%s' is not in the dictionary D.\n", target_word);
        printf("Minimum mutations required: -1\n");
        dict_destroy(dict);
        return 0;
    }

    // Start A added to dictionary if not present
    int start_id = dict_add(dict, start_word);

    int *parent_fwd = malloc((size_t)dict->count * sizeof(int));
    int *parent_bwd = malloc((size_t)dict->count * sizeof(int));
    int meet_node = -1;

    int min_mutations = bidirectional_bfs(dict, start_id, target_id, L, parent_fwd, parent_bwd, &meet_node);

    printf("\n=======================================================\n");
    printf(" Bidirectional BFS: Word Graph Transformation\n");
    printf(" Start: '%s' -> Target: '%s' (Word Length L = %d)\n", start_word, target_word, L);
    printf("=======================================================\n");

    if (min_mutations == -1) {
        printf("Result: No valid transformation sequence exists.\n");
        printf("Minimum mutations required: -1\n");
    } else {
        printf("Result: Valid transformation sequence found!\n");
        printf("Minimum mutations required: %d\n", min_mutations);
        print_transformation_path(dict, start_id, target_id, meet_node, parent_fwd, parent_bwd);
    }

    printf("\nComplexity Analysis:\n");
    printf("  Dictionary Lookup: Hash table with O(L) hash computation & O(1) average lookup.\n");
    printf("  Branching Factor:  At most 25 * L neighbors per word.\n");
    printf("  Search Strategy:   Bidirectional BFS expanding the smaller frontier at each step.\n");
    printf("  Time Complexity:   O(L * 26 * b^(d/2)) vs unidirectional O(L * 26 * b^d).\n");
    printf("                     Worst-case bounded by O(N * L * 26).\n");
    printf("  Space Complexity:  O(N * L) for hash table, dictionary storage, and queues.\n");

    free(parent_fwd);
    free(parent_bwd);
    dict_destroy(dict);

    return 0;
}
