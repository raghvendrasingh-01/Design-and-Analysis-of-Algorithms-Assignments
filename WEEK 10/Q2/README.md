# Q2 - Dynamic Connected Components in a Grid (Number of Islands II)

Given an $R \times C$ grid initially filled entirely with water, land cells are added
sequentially one by one at positions $(r_1, c_1), (r_2, c_2), \ldots, (r_k, c_k)$.
After each addition, we determine the current number of 4-directionally connected
land components (islands).

## Coordinate Flattening and Representation

Each cell in the 2D grid $(r, c)$ where $0 \le r < R$ and $0 \le c < C$ is mapped to a
unique 1D integer index via the coordinate flattening function:
$$\text{id}(r, c) = r \cdot C + c$$
The inverse mapping recovers coordinates when needed:
$$r = \lfloor \text{id} / C \rfloor, \quad c = \text{id} \bmod C$$

This representation allows the $R \times C$ grid to be maintained directly by a standard
1D Disjoint Set Union (DSU) data structure.

## Algorithm and Correctness

1. **State Initialization:**
   - An array `is_land` of size $R \cdot C$ initialized to all zeros (`false`).
   - A DSU structure where `parent[id] = id` and `rank[id] = 0`.
   - `island_count = 0`.
2. **Online Query Processing:**
   When a land cell $(r, c)$ is added:
   - If `is_land[id]` is already 1 (a duplicate addition), the grid topology is unchanged; output the existing `island_count`.
   - Otherwise, mark `is_land[id] = 1` and increment `island_count++`.
   - Inspect the four cardinal orthogonal neighbors:
     $$\{(r - 1, c), (r + 1, c), (r, c - 1), (r, c + 1)\}$$
   - For each valid neighbor $(nr, nc)$ within grid boundaries where `is_land[nid] == 1`:
     - Compute the component representatives: $r_1 = \text{find}(\text{id})$, $r_2 = \text{find}(\text{nid})$.
     - If $r_1 \ne r_2$, the newly added land cell bridges two previously disjoint islands. Merge them using `union(r_1, r_2)` and decrement `island_count--`.
3. Output the updated `island_count`.

## Complexity

- **Time Complexity:**
  - Initialization: $\mathcal{O}(R \cdot C)$ to allocate and clear grid arrays.
  - Per Operation: Inspecting 4 neighbors makes at most 4 DSU `find` and `union` calls. With path compression and union-by-rank, each call takes amortized $\mathcal{O}(\alpha(R \cdot C))$.
  - Total Time: $\mathcal{O}(R \cdot C + K \cdot \alpha(R \cdot C))$.
- **Space Complexity:**
  - $\mathcal{O}(R \cdot C)$ auxiliary space for `parent`, `rank`, and `is_land` arrays.

## Build and Run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q2_dynamic_grid_components.c -o q2
./q2 < sample.txt
```

### Sample Input

```text
4 5 8
0 0
0 1
1 3
2 3
3 4
3 3
1 1
1 2
```

### Sample Output

```text
Dynamic Connected Components in a Grid (Number of Islands II)
Enter grid dimensions R C and number of land additions K:
Processing 8 land additions (0-indexed coordinates 0..3, 0..4):
  Step  1: Add (0, 0) -> Current islands = 1
  Step  2: Add (0, 1) -> Current islands = 1
  Step  3: Add (1, 3) -> Current islands = 2
  Step  4: Add (2, 3) -> Current islands = 2
  Step  5: Add (3, 4) -> Current islands = 3
  Step  6: Add (3, 3) -> Current islands = 2
  Step  7: Add (1, 1) -> Current islands = 2
  Step  8: Add (1, 2) -> Current islands = 1

Final Grid Map (. = Water, # = Land):
  # # . . . 
  . # # # . 
  . . . # . 
  . . . # # 

Execution Summary:
  Grid dimensions:     4 x 5 (20 cells)
  Land cells placed:   8 operations
  Final island count:  1

Complexity Analysis:
  Time Complexity:  O(R * C + K * alpha(R * C)) where alpha is the inverse Ackermann function.
  Space Complexity: O(R * C) for DSU parent, rank, and land arrays.
```
