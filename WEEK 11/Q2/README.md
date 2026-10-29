# Q2 - Multi-Source BFS for Radius-Constrained Voronoi Cell Partitioning

Given an unweighted, undirected graph $G = (V, E)$, a set of $K$ designated service hub vertices $H = \{h_1, h_2, \ldots, h_K\} \subseteq V$ indexed from $1$ to $K$, and an integer radius threshold $R \ge 0$.

For every vertex $v \in V$, assign an ownership label $\text{owner}[v] \in \{1, 2, \ldots, K\} \cup \{-1\}$ according to:
1. **Distance Metric:** $d_i = \text{dist}(v, h_i)$ for all $i \in \{1, \ldots, K\}$, with $d_{\min}(v) = \min_{1 \le i \le K} d_i$.
2. **Reachability Threshold ($R$):** If $d_{\min}(v) > R$, vertex $v$ lies outside service coverage; assign $\text{owner}[v] = -1$.
3. **Closest Hub Assignment:** If $d_{\min}(v) \le R$, assign $v$ to the hub $h_i$ that achieves $d_i = d_{\min}(v)$.
4. **Deterministic Tie-Breaking:** If multiple hubs achieve the exact same minimum distance $d_{\min}(v)$, select the hub with the smallest numerical index $i$.

The objective is to compute the array $\text{owner}[1 \dots |V|]$ and the corresponding shortest distance to that hub for every vertex.

## Graph Representation and Multi-Source BFS Formulation

Running $K$ independent single-source BFS traversals would require $\mathcal{O}(K(|V| + |E|))$ time, which is inefficient for large $K$. Instead, we model the problem as a **Multi-Source Breadth-First Search (Multi-Source BFS)**.

By initializing the BFS FIFO queue with all $K$ service hubs at distance $0$ (ordered by their hub index $1 \dots K$), the wavefront expands concurrently from all hubs layer by layer:
- The first time a vertex $v$ is discovered by edge $(u, v)$ from hub $h_i$, the shortest distance $d_{\min}(v) = \text{dist}[u] + 1$ is established.
- If $v$ is simultaneously reached via another neighbor at the exact same distance $\text{dist}[u] + 1$, the tie-breaking condition $\text{owner}[u] < \text{owner}[v]$ updates the assignment to the smaller numerical hub index without increasing distance.

## Algorithm and Correctness

1. **Initialization:**
   - Allocate `dist[1..V]` initialized to $\infty$.
   - Allocate `owner[1..V]` initialized to $-1$.
   - Create a FIFO queue $Q$.
   - For each hub $i \in \{1, \ldots, K\}$ with vertex $h_i$:
     - If $\text{dist}[h_i] > 0$: set $\text{dist}[h_i] = 0$, $\text{owner}[h_i] = i$, and enqueue $h_i$.
2. **Concurrent Expansion:**
   - While $Q$ is not empty, dequeue vertex $u$.
   - For each neighbor $v$ of $u$:
     - If $\text{dist}[u] + 1 < \text{dist}[v]$:
       - Update $\text{dist}[v] = \text{dist}[u] + 1$.
       - Update $\text{owner}[v] = \text{owner}[u]$.
       - Enqueue $v$.
     - Else if $\text{dist}[u] + 1 == \text{dist}[v]$:
       - If $\text{owner}[u] < \text{owner}[v]$:
         - Update $\text{owner}[v] = \text{owner}[u]$ (tie-break rule).
3. **Radius Filtering:**
   - Iterate through all vertices $v \in \{1, \ldots, V\}$:
     - If $\text{dist}[v] > R$, set $\text{owner}[v] = -1$.

### Correctness
By the properties of BFS on unweighted graphs, exploring in level-order guarantees that vertices are visited in non-decreasing order of their shortest path distance to the nearest hub. The tie-breaking update ensures that whenever multiple hubs are equidistant, the strictly minimum hub index is retained. Filtering with $R$ correctly invalidates nodes outside the coverage radius.

## Complexity Analysis

- **Time Complexity:**
  - Queue initialization with $K$ hubs takes $\mathcal{O}(K)$ time.
  - Multi-source BFS visits each vertex at most once and scans each undirected edge twice (once from each endpoint).
  - Tie-breaking takes $\mathcal{O}(1)$ time per edge check.
  - Overall Time Complexity: $\mathcal{O}(|V| + |E|)$.
- **Space Complexity:**
  - Graph representation: Adjacency list storing $|V|$ vertices and $2|E|$ directed half-edges $\implies \mathcal{O}(|V| + |E|)$.
  - BFS Queue, `dist`, and `owner` arrays: $\mathcal{O}(|V|)$ space.
  - Overall Space Complexity: $\mathcal{O}(|V| + |E|)$.

## Build and Run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q2_voronoi_partitioning.c -o q2
./q2 < sample.txt
```

### Sample Input

```text
9 9
2 2
1 8
1 2
2 3
3 4
4 5
5 8
2 6
6 7
7 8
3 7
```

Format:
- Line 1: `V E` (number of vertices and undirected edges)
- Line 2: `K R` (number of hubs and radius threshold)
- Line 3: `h_1 h_2 ... h_K` (vertex indices of the $K$ hubs)
- Next `E` lines: `u v` (undirected edge between $u$ and $v$)

### Sample Output

```text
=======================================================
 Multi-Source BFS: Radius-Constrained Voronoi Partitioning
 Hubs K = 2, Radius Threshold R = 2
=======================================================
Vertex   | Shortest Dist | Assigned Hub | Status          
---------+--------------+--------------+-----------------
1        | 0            | Hub 1        | Assigned        
2        | 1            | Hub 1        | Assigned        
3        | 2            | Hub 1        | Assigned        
4        | 2            | Hub 2        | Assigned        
5        | 1            | Hub 2        | Assigned        
6        | 2            | Hub 1        | Assigned        
7        | 1            | Hub 2        | Assigned        
8        | 0            | Hub 2        | Assigned        
9        | INF          | -1           | Disconnected    

Assigned Voronoi Cells (owner array [1..|V|]):
  owner = [1, 1, 1, 2, 2, 1, 2, 2, -1]

Complexity Analysis:
  Multi-Source Queue: All K hubs enqueued initially in O(K) time.
  BFS Traversal:      Each vertex and undirected edge visited O(1) times.
  Tie-Breaking:       O(1) comparison on equidistant arrivals.
  Time Complexity:    O(|V| + |E|)
  Space Complexity:   O(|V| + |E|) for adjacency list, queue, and owner arrays.
```
