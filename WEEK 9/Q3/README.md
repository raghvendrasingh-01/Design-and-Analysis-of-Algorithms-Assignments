# Q3 - Minimum Refuelling Stops

Input is target distance `D`, initial fuel `F`, station count `n`, and then
`n` pairs `distance fuel`. Distances are measured from the origin; stations
must lie in `0 <= distance < D`, and station fuel is available once on arrival.
All numeric inputs are unsigned integers. The objective is the minimum number
of station refuelling stops, not the minimum amount of fuel purchased.

## Algorithm and correctness

Stations are sorted by distance. While scanning from the previous position,
all stations already passed are placed into a max-heap keyed by their fuel.
Whenever the next gap cannot be traversed, the algorithm refuels at the
largest available passed station, counting one stop, and repeats until the gap
is reachable or the heap is empty.

The greedy choice is safe because at any point every candidate in the heap is
equally usable for the current gap, and choosing the largest fuel leaves at
least as much fuel as any other one-stop choice for every later station. An
exchange argument replaces the first refuelling choice in any feasible
minimum-stop plan with this largest available station without increasing the
number of stops. Repeating this argument proves the reported count is
minimal. An empty heap when a gap is needed proves the route is unreachable.

## Complexity

Sorting takes `O(n log n)` time. Each station is inserted once and each
refuelling candidate is removed at most once, so the heap scan is
`O(n log n)` time. Space is `O(n)`.

## Build and run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q3_minimum_initial_fuel.c -o q3
./q3 < input.txt
```

The parser rejects malformed values, stations outside the route, allocation
overflow, and arithmetic overflow. Unreachable routes are reported rather
than treated as an answer with an infinite number of stops.
