# Q8 - Minimum Number of Meeting Rooms

The input consists of half-open meeting intervals `[start, end)`. Meetings
whose end equals another meeting's start may use the same room. Intervals are
sorted by start time. A min-heap of active room end times releases every room
that is already free; a second min-heap reuses the smallest available room.
The program prints a room assignment for every original input interval.

## Correctness

Before assigning a meeting, the active heap contains exactly the rooms whose
meetings overlap its start. Releasing all end times `<= start` is therefore
precisely the set of reusable rooms. If one is available, reusing it cannot
increase the number of rooms; otherwise every existing room overlaps the new
meeting, so a new room is necessary. Thus the largest active count is both a
valid assignment and a lower bound forced by an overlapping set of meetings.
The printed assignment consequently uses the minimum possible number of rooms.

## Complexity

Sorting the `n` intervals uses in-place heapsort in `O(n log n)` time.
Each meeting enters and leaves the end-time heap once, and each available-room
operation costs `O(log n)`. Total time is `O(n log n)` and extra space is
`O(n)`. A final linear pass checks that the printed assignments do not overlap.

## Build and run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q8_meeting_rooms.c -o q8
./q8
```

Malformed lines and non-positive intervals are rejected. `sample.txt` is
captured from an actual run.
