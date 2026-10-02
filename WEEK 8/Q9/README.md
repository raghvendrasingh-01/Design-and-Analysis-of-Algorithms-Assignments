# Q9 - Collatz Trajectory and Interval Analysis

The program analyses a user-provided positive 64-bit starting value and every
start in a bounded interval `[a,b]`. For the individual start it prints the
trajectory. For the interval it reports how many starts reached 1, stopped
because the next odd step would overflow, or reached a safety step limit; it
also reports the largest value and longest trajectory.

## Algorithm and overflow safety

The next value is `x/2` for even `x`. For odd `x`, the program first checks

```text
x <= (UINT64_MAX - 1) / 3
```

before evaluating `3*x+1`, so unsigned wraparound is never used as a Collatz
value. Each trajectory grows a heap-backed vector as needed. Interval starts
are analysed one at a time, avoiding storage proportional to all trajectories.
The conjecture is not assumed proved; a ten-million-step guard prevents an
unexpected non-terminating run from hanging the program.

## Correctness and complexity

`collatz_next` implements exactly the even and checked odd recurrence. The
trajectory loop appends each valid next value until 1, overflow, or the safety
limit. For an interval of `r` starts and maximum trajectory length `L`, the
time is `O(rL)` and interval-summary extra space is `O(1)`; the printed
individual trajectory uses `O(L)` dynamic space.

## Build and run

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic q9_collatz_analysis.c -o q9
./q9
```

The interval is limited to one million starting values to keep an accidental
request computationally bounded. `sample.txt` contains output from an actual
run.

## Sample behavior

For start `13` and interval `[10,15]`, the individual path reaches 1 and the
interval summary counts all six starts while reporting their largest value and
longest step count.
