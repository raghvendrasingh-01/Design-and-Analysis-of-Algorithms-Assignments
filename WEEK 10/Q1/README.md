# Q1 - Online Graph-Bipartiteness Testing

An undirected graph with $V$ vertices receives edges $(u, v)$ online one by one.
A graph is bipartite if and only if it contains no odd-length cycles. If adding
an incoming edge $(u, v)$ preserves bipartiteness (i.e. does not introduce an
odd cycle), the edge is accepted into the graph; otherwise, it is reported as a
bipartiteness violation and skipped.

## Graph Representation and Formulation

Rather than recomputing 2-colorability using full Breadth-First Search (BFS) or
Depth-First Search (DFS) on every incoming edge—which would incur $O(E(V + E))$
time—we maintain the bipartite spanning forest using an augmented
**Disjoint Set Union (DSU)** data structure.

Each element $i \in \{1, \ldots, V\}$ maintains:
- `parent[i]`: pointer to its parent in the forest (or itself if it is a root).
- `parity[i]`: relative parity (color difference $\in \{0, 1\}$) between vertex $i$ and `parent[i]`.
- `rank[i]`: subtree rank used for union-by-rank balancing.

During `find(u)` queries, path compression updates `parity[u]` via the bitwise XOR sum
of relative parities along the path to the root. Consequently, `parity[u]` directly yields
the 2-coloring of $u$ relative to its component root:
$$\text{color}(u) \equiv \text{color}(\text{root}(u)) \oplus \text{parity}[u] \pmod 2$$

## Algorithm and Correctness

For each incoming edge $(u, v)$:
1. Query component roots and parities: $r_u = \text{find}(u, \&p_u)$, $r_v = \text{find}(v, \&p_v)$.
2. **If $u$ and $v$ belong to the same component ($r_u == r_v$):**
   - The cycle length formed by adding $(u, v)$ is determined by the tree path between $u$ and $v$:
     $$\text{dist}(u, v) \equiv p_u \oplus p_v \pmod 2$$
   - If $p_u == p_v$, the tree path between $u$ and $v$ is even, so closing the cycle with edge $(u, v)$ forms an **odd cycle** of length $2k + 1$. The edge is rejected and skipped.
   - If $p_u \ne p_v$, the tree path between $u$ and $v$ is odd, so closing the cycle forms an **even cycle** of length $2k + 2$. The graph remains bipartite, and the edge is accepted.
3. **If $u$ and $v$ belong to different components ($r_u \ne r_v$):**
   - The edge connects two disjoint bipartite components into a single bipartite component.
   - We require $\text{color}(u) \oplus \text{color}(v) = 1$.
   - Linking $r_u$ to $r_v$ sets the relative root parity to:
     $$\text{parity}[r_u] = p_u \oplus p_v \oplus 1$$
   - The components are merged using union-by-rank, preserving the bipartite invariant.

## Complexity

- **Time Complexity:**
  - Initialization: $O(V)$ time to set up DSU arrays.
  - Edge Processing: Each edge performs $O(1)$ DSU operations with path compression and union-by-rank, running in amortized $O(\alpha(V))$ time, where $\alpha$ is the extremely slow-growing inverse Ackermann function ($\alpha(V) \le 4$ for all practical $V$).
  - Total Time: $\mathcal{O}(V + E \cdot \alpha(V))$.
- **Space Complexity:**
  - $\mathcal{O}(V)$ auxiliary space for `parent`, `parity`, and `rank` arrays.

## Build and Run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q1_online_bipartite.c -o q1
./q1 < sample.txt
```

### Sample Input

```text
6 7
1 2
2 3
3 4
4 1
1 3
4 5
5 6
```

### Sample Output

```text
Online Graph-Bipartiteness Testing
Enter number of vertices V and number of edge operations E:
Processing 7 edge operations online (vertices 1 to 6):
  Edge 1 (1, 2): ACCEPTED (tree edge, components merged)
  Edge 2 (2, 3): ACCEPTED (tree edge, components merged)
  Edge 3 (3, 4): ACCEPTED (tree edge, components merged)
  Edge 4 (4, 1): ACCEPTED (even cycle formed, bipartiteness preserved)
  Edge 5 (1, 3): REJECTED (odd cycle detected, violates bipartiteness!)
  Edge 6 (4, 5): ACCEPTED (tree edge, components merged)
  Edge 7 (5, 6): ACCEPTED (tree edge, components merged)

Execution Summary:
  Total edges streamed: 7
  Accepted edges:       6
  Rejected edges:       1
  Bipartite invariant:  MAINTAINED

Vertex 2-Coloring Assignment:
  Vertex 1: Component Root = 2, Color = 1
  Vertex 2: Component Root = 2, Color = 0
  Vertex 3: Component Root = 2, Color = 1
  Vertex 4: Component Root = 2, Color = 0
  Vertex 5: Component Root = 2, Color = 1
  Vertex 6: Component Root = 2, Color = 0

Complexity Analysis:
  Time Complexity:  O(V + E * alpha(V)) where alpha is the inverse Ackermann function.
  Space Complexity: O(V) for DSU parent, rank, and parity arrays.
```
