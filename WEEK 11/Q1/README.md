# Q1 - Shortest Path on Parity-Constrained Graphs via 0-1 BFS

Given a directed graph $G = (V, E)$, a designated source vertex $S \in V$, and an extra penalty cost $C \ge 0$. Every directed edge $(u, v) \in E$ has weight $w(u, v) \in \{0, 1\}$.
For any path $P$ from $S$ to $v$:
1. $W(P)$ denotes the sum of all edge weights along $P$.
2. $k_1(P)$ denotes the total count of weight-1 edges traversed in $P$. Notice that since each edge has weight 0 or 1, $W(P) = k_1(P)$.
3. The effective cost of path $P$ is:
$$\text{Cost}(P) = W(P) + \begin{cases} 0 & \text{if } k_1(P) \text{ is even} \\ C & \text{if } k_1(P) \text{ is odd} \end{cases}$$

The objective is to compute for every vertex $v \in V$ the minimum possible $\text{Cost}(P)$ among all valid paths from $S$ to $v$, or report $v$ as unreachable.

## Graph Representation and State-Space Expansion

Since the parity of weight-1 edges traversed directly dictates whether the additive penalty $C$ applies, we lift the graph to an expanded state-space graph $G' = (V', E')$:
- Each state is a tuple $(u, p)$, where $u \in V$ is the physical vertex and $p \in \{0, 1\}$ represents the current parity of $k_1(P) \pmod 2$.
- The total number of states is $|V'| = 2|V|$.
- For every directed edge $e = (u, v) \in E$ with weight $w \in \{0, 1\}$:
  - Transition: $(u, p) \xrightarrow{w} (v, p \oplus w)$.
  - The weight of this transition in $G'$ is identical to $w \in \{0, 1\}$.

Because all transition weights are in $\{0, 1\}$, finding the shortest path $W(P)$ to reach every state $(v, p)$ from the source state $(S, 0)$ is solvable via **0-1 Breadth-First Search (0-1 BFS)** using a double-ended queue (deque).

## Algorithm and Correctness

1. **State Distances:**
   - Initialize $\text{dist}[v][0] = \infty$ and $\text{dist}[v][1] = \infty$ for all $v \in V$.
   - Set $\text{dist}[S][0] = 0$.
2. **0-1 BFS Execution:**
   - Push $(S, 0)$ to the deque.
   - Pop front state $(u, p)$ with distance $d = \text{dist}[u][p]$.
   - For each outgoing edge $(u, v)$ with weight $w \in \{0, 1\}$:
     - Next parity: $p' = p \oplus w$.
     - Next weight: $d' = d + w$.
     - If $d' < \text{dist}[v][p']$:
       - Update $\text{dist}[v][p'] = d'$.
       - If $w == 0$, insert $(v, p')$ at the **front** of the deque (distance $d$).
       - If $w == 1$, insert $(v, p')$ at the **back** of the deque (distance $d + 1$).
3. **Effective Cost Calculation:**
   - For every vertex $v \in V$:
     $$\text{Cost}_{\text{even}}(v) = \text{dist}[v][0]$$
     $$\text{Cost}_{\text{odd}}(v) = \text{dist}[v][1] + C$$
     $$\text{MinCost}(v) = \min(\text{Cost}_{\text{even}}(v), \text{Cost}_{\text{odd}}(v))$$
   - If both $\text{dist}[v][0] = \infty$ and $\text{dist}[v][1] = \infty$, vertex $v$ is unreachable from $S$.

### Proof of 0-1 BFS Optimality
At any point during 0-1 BFS, the elements in the deque have distances that differ by at most 1 ($d$ or $d + 1$). Inserting 0-weight edges at the front and 1-weight edges at the back maintains the monotonic non-decreasing order of distances across deque extractions. Hence, every state is settled with its true shortest distance when extracted, guaranteeing optimality in linear time without the logarithmic factor of Dijkstra's algorithm.

## Complexity Analysis

- **Time Complexity:**
  - State space size: $|V'| = 2|V|$.
  - Total edges in expanded graph: $|E'| = 2|E|$.
  - Each state is pushed and popped at most once in 0-1 BFS with $O(1)$ deque operations.
  - Overall Time Complexity: $\mathcal{O}(|V| + |E|)$.
- **Space Complexity:**
  - Graph representation: Adjacency list storing $|V|$ vertices and $|E|$ edges $\implies \mathcal{O}(|V| + |E|)$.
  - Distance table and deque: $\mathcal{O}(|V|)$ auxiliary space.
  - Overall Space Complexity: $\mathcal{O}(|V| + |E|)$.

## Build and Run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q1_parity_01_bfs.c -o q1
./q1 < sample.txt
```

### Sample Input

```text
6 7
1 5
1 2 1
1 3 0
2 4 1
3 4 1
4 5 0
3 5 1
2 3 0
```

Format:
- Line 1: `V E` (number of vertices and directed edges)
- Line 2: `S C` (source vertex and odd parity penalty)
- Next `E` lines: `u v w` (directed edge from $u$ to $v$ with weight $w \in \{0, 1\}$)

### Sample Output

```text
=======================================================
 Shortest Path on Parity-Constrained Graphs via 0-1 BFS
 Source S = 1, Odd Parity Penalty C = 5
=======================================================
Vertex   | Cost(Even)   | Cost(Odd)    | Min Cost    
---------+--------------+--------------+-------------
1        | 0 (W=0)      | INF          | 0           
2        | INF          | 6 (W=1)      | 6           
3        | 0 (W=0)      | 6 (W=1)      | 0           
4        | 2 (W=2)      | 6 (W=1)      | 2           
5        | 2 (W=2)      | 6 (W=1)      | 2           
6        | Unreachable  | Unreachable  | Unreachable 

Complexity Analysis:
  State Space:       |V'| = 2 * |V| vertices, |E'| = 2 * |E| directed edges
  Queue Operations:  0-1 BFS using Deque (O(1) push_front for w=0, push_back for w=1)
  Time Complexity:   O(|V| + |E|)
  Space Complexity:  O(|V| + |E|) for adjacency list and state distances
```
