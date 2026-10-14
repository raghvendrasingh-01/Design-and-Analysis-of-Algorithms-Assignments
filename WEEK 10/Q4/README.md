# Q4 - Reverse Time-Step DSU Application (Sequential Vertex Deletions)

Given a vertex-weighted undirected graph $G = (V, E)$ with vertex weights
$w_1, \ldots, w_V$, vertices are deleted sequentially one by one in a specified
permutation order $P = [p_1, p_2, \ldots, p_V]$. Deleting a vertex removes that
vertex and all incident edges. After each vertex deletion, we compute the
maximum total vertex weight among all remaining connected components in the graph.

## Algorithmic Insight: Reverse Time-Step DSU

Online deletion of vertices and edges (dynamic graph decremental connectivity)
requires complex split-supporting structures that take polynomial recomputation
time. However, because the entire deletion sequence $P$ is known in advance,
the process can be viewed **in reverse time**:
- Removing vertices from step $1$ to $V$ in forward time is mathematically
  equivalent to starting with an empty graph at time $t = V$ and **inserting**
  vertices from $p_V$ backwards to $p_1$.
- DSU naturally supports incremental edge and vertex additions with near-constant
  amortized overhead per operation.

## Algorithm and Correctness

1. **Adjacency Representation:**
   The undirected graph is stored in an adjacency list using an array of linked list edge nodes.
2. **Reverse Simulation:**
   - Initialize an empty graph where all vertices are marked inactive (`is_active[v] = false`).
   - At step $V$ (after all vertices have been deleted in forward time), no vertices remain; the maximum component weight is $0$:
     $$\text{ans}[V] = 0$$
   - For step $t$ running backwards from $V$ down to $2$:
     - Activate vertex $u = p_t$ by setting $\text{is_active}[u] = \text{true}$.
     - Initialize DSU for $u$: $\text{parent}[u] = u$, $\text{rank}[u] = 0$, $\text{comp\_weight}[u] = w_u$.
     - Update $\text{running\_max} = \max(\text{running\_max}, w_u)$.
     - Iterate through all edges $(u, v)$ incident to $u$:
       - If neighbor $v$ is already active ($\text{is_active}[v] == \text{true}$):
         - Find roots $r_u = \text{find}(u)$ and $r_v = \text{find}(v)$.
         - If $r_u \ne r_v$, merge components via union-by-rank:
           $$\text{comp\_weight}[r_{new}] = \text{comp\_weight}[r_u] + \text{comp\_weight}[r_v]$$
           $$\text{running\_max} = \max(\text{running\_max}, \text{comp\_weight}[r_{new}])$$
     - The state after adding $p_t$ in reverse time corresponds precisely to the graph state immediately after deleting $p_{t-1}$ in forward time.
       Record:
       $$\text{ans}[t - 1] = \text{running\_max}$$
3. **Output:**
   Emit the answers $\text{ans}[1], \text{ans}[2], \ldots, \text{ans}[V]$ in forward order.

## Complexity

- **Time Complexity:**
  - Adjacency list construction: $\mathcal{O}(V + M)$ time.
  - Reverse DSU insertions: Exactly $V$ vertex additions and at most $2M$ edge checks.
  - Each DSU operation with path compression and union-by-rank takes amortized $\mathcal{O}(\alpha(V))$.
  - Total Time: $\mathcal{O}(V + M \cdot \alpha(V))$.
- **Space Complexity:**
  - $\mathcal{O}(V + M)$ auxiliary space for the adjacency list, DSU arrays, activity flags, and answer array.

## Build and Run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q4_reverse_delete_max_component.c -o q4
./q4 < sample.txt
```

### Sample Input

```text
5 4
10 20 30 40 50
1 2
2 3
3 4
4 5
3 4 2 5 1
```

### Sample Output

```text
Reverse Time-Step DSU Application (Sequential Vertex Deletions)
Enter number of vertices V and number of edges M:
Enter vertex weights w_1 through w_5:
Enter 4 undirected edges (u v):
Enter deletion permutation order P = [p_1, ..., p_5]:

Deletions and Maximum Remaining Component Weight:
Step   | Deleted Vertex | Max Component Weight
-------+----------------+---------------------
 1     | Vertex 3       | 90
 2     | Vertex 4       | 50
 3     | Vertex 2       | 50
 4     | Vertex 5       | 10
 5     | Vertex 1       | 0

Complexity Analysis:
  Time Complexity:  O(V + M * alpha(V)) using offline reverse time DSU with path compression and union-by-rank.
  Space Complexity: O(V + M) for adjacency list, DSU, and query buffers.
```
