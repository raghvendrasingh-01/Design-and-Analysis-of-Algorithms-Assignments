# Q5 - Lexicographically Smallest Connected Component Permutation

Given an initial permutation $A = [A_1, A_2, \ldots, A_n]$ of numbers $1 \ldots n$,
and a list of $m$ allowable pairwise swaps $(u, v)$ where elements at indices $u$
and $v$ may be swapped, determine the lexicographically smallest permutation
achievable through any number of valid swaps.

## Graph Representation and Group-Theoretic Formulation

The allowable swap pairs define an undirected graph $G = (V, E)$ where:
- Vertices $V = \{1, 2, \ldots, n\}$ correspond to the array indices.
- Edges $E = \{(u_1, v_1), \ldots, (u_m, v_m)\}$ represent allowable transpositions.

By fundamental permutation group theory:
- Any set of transpositions forming a spanning tree of a connected component of size $k$
  generates the full symmetric group $S_k$ on those $k$ index positions.
- Hence, elements located at indices within the same connected component can be rearranged
  into **any arbitrary permutation** of those elements, without affecting elements at
  indices outside the component.
- The global optimization problem decomposes into independent subproblems for each
  connected component.

## Algorithm and Correctness

1. **Connected Component Decomposition:**
   - Initialize a Disjoint Set Union (DSU) structure over the $n$ indices.
   - For each allowable swap pair $(u, v)$, execute `union(u, v)`.
   - After processing all $m$ swap pairs, partition the indices into disjoint components
     $C_1, C_2, \ldots, C_k$.
2. **Lexicographical Minimization:**
   For each connected component $C$:
   - Collect its index positions in increasing order:
     $$I_C = \{i_1 < i_2 < \ldots < i_{|C|}\}$$
   - Extract the values currently stored at these positions:
     $$V_C = \{A[i_1], A[i_2], \ldots, A[i_{|C|}\}$$
   - Sort the values in ascending numerical order:
     $$v_1 \le v_2 \le \ldots \le v_{|C|}$$
   - Assign the sorted values to the sorted positions:
     $$A[i_j] = v_j \quad \text{for } j = 1, \ldots, |C|$$
   - Since earlier indices in the array receive the strictly smallest available values
     from their connected component, the resulting permutation is guaranteed to be
     globally lexicographically minimal.

## Complexity

- **Time Complexity:**
  - DSU operations: $\mathcal{O}(m \cdot \alpha(n))$ time to union all swap pairs.
  - Index grouping: $\mathcal{O}(n)$ using linked chains across component roots.
  - Sorting within components: $\sum_{i} \mathcal{O}(|C_i| \log |C_i|) \le \mathcal{O}(n \log n)$ total comparison time using quicksort.
  - Total Time: $\mathcal{O}(n \log n + m \cdot \alpha(n))$.
- **Space Complexity:**
  - $\mathcal{O}(n + m)$ auxiliary space for DSU parent and rank arrays, component linked lists, and sorting buffers.

## Build and Run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q5_lexicographically_smallest_swap_permutation.c -o q5
./q5 < sample.txt
```

### Sample Input

```text
7 3
5 3 1 4 7 2 6
1 3
3 6
2 5
```

### Sample Output

```text
Lexicographically Smallest Connected Component Permutation
Enter permutation length n and number of allowable swap pairs m:
Enter the 7 elements of permutation A:
Enter 3 allowable swap pairs (u v):

Original Permutation:
  5 3 1 4 7 2 6

Connected Components Identified: 4
  Component root 3 (size 3): indices { 1 3 6 }
  Component root 4 (size 1): indices { 4 }
  Component root 5 (size 2): indices { 2 5 }
  Component root 7 (size 1): indices { 7 }

Lexicographically Smallest Permutation Achievable:
  1 3 2 4 7 5 6

Complexity Analysis:
  Time Complexity:  O(n log n + m * alpha(n))
                    - DSU on m swap edges: O(m * alpha(n))
                    - Grouping indices: O(n)
                    - Sorting within components: sum(O(|C_i| log |C_i|)) <= O(n log n)
  Space Complexity: O(n + m) for DSU, grouping chains, and index buffers
```
