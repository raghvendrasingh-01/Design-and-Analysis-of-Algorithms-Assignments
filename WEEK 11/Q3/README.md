# Q3 - Bidirectional BFS for Shortest Transformation Sequence in a Word Graph

Given:
- A dictionary $D$ containing $N$ unique words, each of length $L \le 10$, consisting solely of lowercase English letters (`'a'` to `'z'`).
- A starting word $A$ and a target word $B$, both of length $L$.
- A valid mutation step consists of changing exactly one character of a word such that the resulting word exists in $D$.
- $B$ must always be in $D$. A transformation sequence is:
$$W_0, W_1, W_2, \ldots, W_k$$
where $W_0 = A$, $W_k = B$, $W_i \in D$ for all $1 \le i \le k$, and each adjacent pair differs by Hamming distance 1.

The objective is to compute the minimum number of mutations $k$ required to transform $A$ into $B$, or return $-1$ if no valid transformation sequence exists.

## Input Representation and Transformation Graph

The problem defines an implicit undirected graph where:
- Vertices are words in $D \cup \{A\}$.
- Edges exist between any two words differing by exactly one character.

Since $N \le 10^5$, pre-building an explicit adjacency matrix ($\mathcal{O}(N^2)$ space/time) or computing pairwise Hamming distances between all pairs ($\mathcal{O}(N^2 \cdot L)$ time) is infeasible.
Instead, we:
1. Store all dictionary words in an efficient hash table with DJB2 hashing and collision chaining.
2. Generate candidate mutated words **on the fly**: for a word of length $L$, there are $25 \times L$ possible single-character mutations.
3. Query the hash table in $\mathcal{O}(L)$ time per mutation to check if the neighbor exists in $D$.

## Algorithm: Bidirectional Breadth-First Search

Standard unidirectional BFS expands outwards from $A$ with branching factor $b \le 25L$, potentially visiting $\mathcal{O}(b^d)$ states where $d$ is the shortest path distance.

**Bidirectional BFS** drastically cuts the search frontier by running two simultaneous searches:
1. **Forward Search:** Initiated from start word $A$.
2. **Backward Search:** Initiated from target word $B$.
3. **Frontier Alternation:** At each iteration, expand the frontier with the **smaller queue size**.
4. **Collision Detection:** If an expansion from one frontier reaches a word already discovered by the opposite frontier, a path is completed:
$$\text{dist}(A, B) = \text{dist}_{\text{fwd}}(v) + \text{dist}_{\text{bwd}}(v)$$
5. **Path Reconstruction:** Using parent pointers recorded in both directions, backtrack to reconstruct the exact optimal transformation ladder: $W_0 \to W_1 \to \cdots \to W_k$.

### Correctness
Since edge weights are uniform ($1$ mutation per edge), expanding level-by-level guarantees that the first meeting between the two frontiers achieves the minimum possible total path length. Expanding the smaller queue optimizes frontier expansion, minimizing the number of explored vertices.

## Complexity Analysis

- **Time Complexity:**
  - Hash table operations: $\mathcal{O}(L)$ per word lookup.
  - Branching factor per state: $25 \times L$ possible single-letter alterations.
  - Generating all neighbors for a state: $\mathcal{O}(25 \cdot L^2)$ time.
  - Bidirectional search visits states up to radius $d/2$ from each side, reducing search space from $\mathcal{O}(b^d)$ to $\mathcal{O}(b^{d/2})$.
  - Worst-Case Time Complexity: Bounded by $\mathcal{O}(N \cdot L^2 \cdot 26)$ when almost all words are explored. For $L \le 10$, this executes in a fraction of a second.
- **Space Complexity:**
  - Hash table and dictionary array: $\mathcal{O}(N \cdot L)$ space.
  - BFS Queues and distance arrays: $\mathcal{O}(N)$ space.
  - Overall Space Complexity: $\mathcal{O}(N \cdot L)$.

## Build and Run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q3_word_ladder_bibfs.c -o q3
./q3 < sample.txt
```

### Sample Input

```text
6
hit cog
hot dot dog lot log cog
```

Format:
- Line 1: $N$ (number of words in dictionary)
- Line 2: `start_word target_word`
- Line 3: space-separated list of $N$ dictionary words

### Sample Output

```text
=======================================================
 Bidirectional BFS: Word Graph Transformation
 Start: 'hit' -> Target: 'cog' (Word Length L = 3)
=======================================================
Result: Valid transformation sequence found!
Minimum mutations required: 4

Optimal Transformation Sequence:
  hit -> hot -> dot -> dog -> cog

Complexity Analysis:
  Dictionary Lookup: Hash table with O(L) hash computation & O(1) average lookup.
  Branching Factor:  At most 25 * L neighbors per word.
  Search Strategy:   Bidirectional BFS expanding the smaller frontier at each step.
  Time Complexity:   O(L * 26 * b^(d/2)) vs unidirectional O(L * 26 * b^d).
                     Worst-case bounded by O(N * L * 26).
  Space Complexity:  O(N * L) for hash table, dictionary storage, and queues.
```
