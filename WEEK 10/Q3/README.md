# Q3 - Maximum Weight Component with Size Constraints

Given an undirected graph where each vertex $v$ has an associated weight $w_v$,
edges arrive sequentially with edge weights. At any point (initially and after each
edge addition), we compute the maximum total vertex weight among all connected
components whose vertex count does not exceed a fixed size threshold $K$:
$$\max \left\{ \sum_{v \in C} w_v \;\middle|\; C \text{ is a connected component}, \, |C| \le K \right\}$$

## Formulation and Representation

- **Graph Structure:**
  Vertices are indexed from $1$ to $V$. Component connectivity, component cardinality,
  and total vertex weight sums are tracked using Disjoint Set Union (DSU).
- **Component State:**
  For each root $r$:
  - `comp_size[r]`: number of vertices in component $r$.
  - `weight[r]`: sum of vertex weights $\sum_{v \in C_r} w_v$.
  - `version[r]`: generational version counter incremented whenever component $r$ is merged or re-keyed.
- **Valid Component Maximum:**
  To maintain the running maximum across dynamic merges and size threshold evictions,
  we maintain an augmented **Max-Heap with Lazy Deletion**. The heap stores triples
  `{root, weight, version}`.

## Algorithm and Correctness

1. **Initialization:**
   - Initially, each vertex $v \in \{1, \ldots, V\}$ is in its own component of size $1$ with weight $w_v$.
   - If $|C_v| = 1 \le K$, the initial state is inserted into the Max-Heap.
2. **Sequential Edge Addition $(u, v, \text{edge\_weight})$:**
   - Find component roots $r_u = \text{find}(u)$ and $r_v = \text{find}(v)$.
   - If $r_u == r_v$, $u$ and $v$ are already connected; components remain unchanged.
   - If $r_u \ne r_v$:
     - Increment `version[r_u]` and `version[r_v]`, invalidating any stale entries in the heap for both components.
     - Merge the smaller component into the larger component (union-by-size).
     - Update the merged root's size and weight:
       $$\text{comp\_size}[r_{new}] = \text{comp\_size}[r_u] + \text{comp\_size}[r_v]$$
       $$\text{weight}[r_{new}] = \text{weight}[r_u] + \text{weight}[r_v]$$
     - If $\text{comp\_size}[r_{new}] \le K$, insert $\{r_{new}, \text{weight}[r_{new}], \text{version}[r_{new}]\}$ into the Max-Heap.
3. **Querying Maximum Valid Weight:**
   - While the heap is non-empty, inspect the top element $\{r, W, \text{ver}\}$:
     - If $r$ is no longer a root (`parent[r] != r`), or $\text{ver} \ne \text{version}[r]$, or $\text{comp\_size}[r] > K$, pop and discard the stale element.
     - Otherwise, the top element represents an active, valid connected component. Return $W$.
   - If the heap becomes empty, all components have grown beyond $K$; report that no valid component exists.

## Complexity

- **Time Complexity:**
  - Initialization: $\mathcal{O}(V \log V)$ to push initial singletons.
  - Per Edge Addition:
    - DSU `find` and `union` take amortized $\mathcal{O}(\alpha(V))$ using path compression and union-by-size.
    - At most one new valid component is pushed to the heap in $\mathcal{O}(\log V)$ time.
    - Each heap element is popped at most once across the lifetime of the program.
  - Total Time: $\mathcal{O}(V + E \log V)$.
- **Space Complexity:**
  - $\mathcal{O}(V)$ auxiliary space for DSU state arrays and the Max-Heap.

## Build and Run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q3_max_weight_component_k.c -o q3
./q3 < sample.txt
```

### Sample Input

```text
6 3 5
10 25 15 30 5 20
1 2 5
3 4 8
5 6 3
1 3 12
5 1 7
```

### Sample Output

```text
Maximum Weight Component with Size Constraints
Enter number of vertices V, size threshold K, and number of edges E:
Enter vertex weights w_1 through w_6:

Initial state before adding edges (threshold K = 3):
  Max component weight (size <= 3): 30 (Component root: 4, size: 1)

Processing 5 edge additions sequentially (u v edge_weight):
  Edge  1: (1, 2, wt=5) -> Max valid weight = 35 (Root 1, size 2)
  Edge  2: (3, 4, wt=8) -> Max valid weight = 45 (Root 3, size 2)
  Edge  3: (5, 6, wt=3) -> Max valid weight = 45 (Root 3, size 2)
  Edge  4: (1, 3, wt=12) -> Max valid weight = 25 (Root 5, size 2)
  Edge  5: (5, 1, wt=7) -> All components exceed size threshold K=3

Complexity Analysis:
  Time Complexity:  O(V + E * (alpha(V) + log V)) = O(V + E log V)
                    - DSU operations with path compression & union-by-size: O(alpha(V))
                    - Priority Queue (Max-Heap) with lazy deletion: O(log V) per merge
  Space Complexity: O(V) for DSU parent, size, weight arrays, and lazy Max-Heap
```
