# Q2 — Fair vs Biased Coin

> Show by simulation that P(HEAD) for a fair coin is about 0.5, then compare a
> fair coin against biased ones.

| | |
|---|---|
| **Source** | [`q2_coin_toss.c`](q2_coin_toss.c) |
| **Sample file** | [`sample.txt`](sample.txt) |
| **Input** | One integer — the number of tosses per coin |
| **Build** | `gcc -Wall -Wextra q2_coin_toss.c -o q2 -lm` |
| **Writes** | `coin_toss_analysis.csv` in the current directory |

---

## Problem Statement

Using simulation in C, show that the probability of getting a HEAD by tossing a
fair coin is about `0.5`. Extend the simulation to compare fair vs biased
coin-tossing experiments.

---

## Approach

A single toss is `(rand() % 100) < percent_heads`, which yields HEAD with
probability `percent_heads / 100`. The generator is seeded from `time(NULL)`, so
every run produces different numbers.

The program then runs two experiments:

**1. Convergence of a fair coin.** A 50 % coin is tossed `10`, `100`, `1,000`,
`10,000`, `100,000` and `1,000,000` times. Each round reports the observed head
fraction and the absolute error against the expected `0.5`. The error shrinks as
the sample grows — the law of large numbers, observed rather than asserted.

**2. Fair vs biased comparison.** Five coins with biases of **50, 60, 70, 90 and
30 %** are each tossed the number of times entered by the user. For every coin the
program records heads, tails, observed frequency, expected frequency and the
absolute difference, writes the table to CSV for plotting, and draws an ASCII bar
chart of the observed head probabilities.

---

## Time Complexity

**Θ(n)** in the number of tosses. Each toss is a single `rand()` call and one
comparison, so cost is linear in the total tosses performed:
`Θ(Σ 10^i)` for the convergence run plus `Θ(C · n)` for `C = 5` coins.

## Space Complexity

**Θ(1)** auxiliary. Counters are accumulated in place; the only storage is four
fixed-size arrays of length `N_COINS = 5`, which does not grow with the number of
tosses.

---

## Sample Input

```text
100000
```

## Sample Output

Values come from a real run. Because the generator is time-seeded, your numbers
will differ slightly — the *convergence* is what reproduces, not the digits.

```text
=============================================
      FAIR vs BIASED COIN TOSS SIMULATION
=============================================

Enter number of tosses for comparison: 100000

========== FAIR COIN CONVERGENCE ==========
---------------------------------------------------------------
Tosses          Heads   Tails   Fraction        Error
---------------------------------------------------------------
      10            5       5   0.500000        0.000000
     100           46      54   0.460000        0.040000
    1000          529     471   0.529000        0.029000
   10000         4886    5114   0.488600        0.011400
  100000        50058   49942   0.500580        0.000580
 1000000       500558  499442   0.500558        0.000558

Observation:
As the number of tosses increases, the observed probability
approaches 0.5 (Law of Large Numbers).

========== FAIR vs BIASED COINS ==========
(100000 tosses each)

--------------------------------------------------------------------------
Bias    Heads           Tails           Observed        Expected        Difference
--------------------------------------------------------------------------
50%        50109           49891        0.50109         0.50000         0.00109
60%        59783           40217        0.59783         0.60000         0.00217
70%        69961           30039        0.69961         0.70000         0.00039
90%        89910           10090        0.89910         0.90000         0.00090
30%        29751           70249        0.29751         0.30000         0.00249

CSV file 'coin_toss_analysis.csv' created successfully.
Open it in Excel, LibreOffice or Google Sheets and insert a Bar Graph.

========== ASCII GRAPH ==========
Observed Head Probability

50% (0.50109) | █████████████████████████
60% (0.59783) | ██████████████████████████████
70% (0.69961) | ███████████████████████████████████
90% (0.89910) | █████████████████████████████████████████████
30% (0.29751) | ███████████████

============== CONCLUSION ==============
1. A fair coin converges towards probability 0.5.
2. A biased coin converges towards its own probability.
3. Increasing the number of tosses reduces random error.
4. This demonstrates the Law of Large Numbers.

Time Complexity  : O(n)
Space Complexity : O(1)
```

---

## Sample Explanation

**The fair coin converges to 0.5.** At 10 tosses the observed fraction can sit
anywhere; at 100 tosses the run above was off by `0.04`. By 100,000 tosses the
error has fallen to `0.00058` and by 1,000,000 to `0.00056`. The error shrinks
roughly as `1/√n`, so one extra decade of tosses buys only about **half** a
correct digit — gaining a full digit takes 100× the samples. That is what makes
Monte Carlo convergence slow.

**Every biased coin converges to its own bias, not to 0.5.** Over 100,000 tosses
each, no coin drifted from its programmed bias by more than `0.00249`. The
simulation is measuring each coin's true parameter, so a 90 % coin is just as
predictable as a fair one — it simply converges to a different limit.

**What this demonstrates.** Fairness is not a property the simulation assumes; it
is a limit the frequency approaches. A short run cannot distinguish a fair coin
from a mildly biased one, but a long run separates 50 % from 60 % decisively.

## Committed Artefacts

| File | Description |
|------|-------------|
| [`coin_toss_analysis.csv`](coin_toss_analysis.csv) | Recorded run — observed vs expected frequency for all five biases |
| [`coin_toss_analysis.svg`](coin_toss_analysis.svg) | Grouped bar chart plotted from that CSV |

In the committed CSV the fair coin lands at `0.50045` against an expected `0.50`,
and the largest deviation across all five coins is `0.00079`.

---

## Build and Run

```bash
gcc -Wall -Wextra q2_coin_toss.c -o q2 -lm
./q2
```

> **Heads up.** The program writes `coin_toss_analysis.csv` into the *current
> working directory* under a fixed name. Running it from inside `Q2/` overwrites
> the committed copy. To keep it intact, compile from `Q2/` but place the binary
> in a scratch directory and run it there:
>
> ```bash
> mkdir -p /tmp/daa
> gcc -Wall -Wextra q2_coin_toss.c -o /tmp/daa/q2 -lm
> cd /tmp/daa && ./q2
> ```

## Files

| File | Description |
|------|-------------|
| [`q2_coin_toss.c`](q2_coin_toss.c) | Solution source |
| [`sample.txt`](sample.txt) | Sample input and output |
| [`coin_toss_analysis.csv`](coin_toss_analysis.csv) | Measurements from a recorded run |
| [`coin_toss_analysis.svg`](coin_toss_analysis.svg) | Plot of those measurements |
