#!/usr/bin/env python3
"""Generate the Week 4 figures from the programs' own measured output.

For every question this script compiles the committed .c file, runs it, parses
the table it prints, and draws the figures from those numbers.  Nothing is
transcribed by hand, so no figure can disagree with the measured counts -- if a
program's output changes, the plots change with it.

    usage:  python3 "WEEK 4/make_plots.py"  [Q1 Q2 ...]

Writes  WEEK 4/Q<n>/plots/*.png.  Requires matplotlib; parsing alone needs only
the standard library, so the parse_* helpers are importable and testable without
it (that is why matplotlib is imported inside main()).
"""

import math
import os
import re
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.abspath(__file__))

SOURCES = {
    "Q1": "q1_three_colour_stable_sort.c",
    "Q2": "q2_pair_sum_two_sets.c",
    "Q3": "q3_k_sum_subset.c",
    "Q4": "q4_party_peak_occupancy.c",
    "Q5": "q5_merge_intervals.c",
    "Q6": "q6_max_overlap_point.c",
}
# Q1 has no math.h, so it must not be linked against libm.
NEEDS_LIBM = {"Q1": False, "Q2": True, "Q3": True, "Q4": True, "Q5": True, "Q6": True}

BLUE, RED, GREY = "#1f6fb4", "#d62728", "#7f7f7f"
GREEN, ORANGE, PURPLE = "#2ca02c", "#ff7f0e", "#9467bd"


# ----------------------------------------------------------------- run & parse

def run(q):
    """Compile and run question q, returning its stdout."""
    src = os.path.join(ROOT, q, SOURCES[q])
    if not os.path.exists(src):
        raise SystemExit("missing source: %s" % src)
    with tempfile.TemporaryDirectory() as tmp:
        exe = os.path.join(tmp, q.lower())
        cmd = ["gcc", "-std=c11", "-Wall", "-Wextra", "-O2", src, "-o", exe]
        if NEEDS_LIBM[q]:
            cmd.append("-lm")
        cp = subprocess.run(cmd, capture_output=True, text=True)
        if cp.returncode:
            raise SystemExit("%s failed to compile:\n%s" % (q, cp.stderr))
        if cp.stderr.strip():
            print("  warning: %s compiled with diagnostics:\n%s" % (q, cp.stderr))
        cp = subprocess.run([exe], capture_output=True, text=True)
        if cp.returncode:
            raise SystemExit("%s exited %d:\n%s" % (q, cp.returncode, cp.stdout))
        if "MISMATCH" in cp.stdout:
            raise SystemExit("%s reported a MISMATCH; refusing to plot" % q)
        return cp.stdout


def cell(tok):
    """A table cell: an int, a float, or None for the '-' placeholder."""
    if tok == "-":
        return None
    return float(tok) if ("." in tok) else int(tok)


def rows(text, ncols):
    """Every table row with exactly ncols numeric/'-' cells, in order.

    The programs' tables are whitespace-aligned and their prose never forms a
    run of ncols bare numbers, so matching on shape alone is unambiguous and
    survives changes to the column headings.
    """
    out = []
    for line in text.splitlines():
        toks = line.split()
        if len(toks) != ncols:
            continue
        if all(re.fullmatch(r"-|-?\d+(?:\.\d+)?", t) for t in toks):
            out.append([cell(t) for t in toks])
    return out


def rows_flagged(text, nnum, nflag):
    """Rows of nnum numeric cells followed by nflag PASS/FAIL words (Q1's table)."""
    out = []
    for line in text.splitlines():
        toks = line.split()
        if len(toks) != nnum + nflag:
            continue
        nums, flags = toks[:nnum], toks[nnum:]
        if not all(re.fullmatch(r"-?\d+(?:\.\d+)?", t) for t in nums):
            continue
        if not all(f in ("PASS", "FAIL") for f in flags):
            continue
        out.append([cell(t) for t in nums] + flags)
    return out


def col(rs, i):
    return [r[i] for r in rs]


def defined(xs, ys):
    """Drop pairs where y is None (a '-' cell), keeping x aligned."""
    p = [(x, y) for x, y in zip(xs, ys) if y is not None]
    return [x for x, _ in p], [y for _, y in p]


def headroom(ax, rs, cols, frac=0.42):
    """Extend a log y-axis upward so an upper-left legend cannot cover a curve.

    matplotlib sizes the axes to the data, then draws the legend on top of it,
    so a series that peaks in the top-left corner ends up hidden.  Reserving
    frac of the vertical span keeps every curve visible.
    """
    vals = [r[c] for r in rs for c in cols if r[c] is not None and r[c] > 0]
    if not vals:
        return
    lo, hi = math.log10(min(vals)), math.log10(max(vals))
    span = (hi - lo) or 1.0
    ax.set_ylim(10 ** (lo - 0.04 * span), 10 ** (hi + frac * span))


# ------------------------------------------------------------------- plotting

def figure(plt, title, xlabel, ylabel, loglog=True, size=(9.5, 5.6)):
    fig, ax = plt.subplots(figsize=size)
    if loglog:
        ax.set_xscale("log", base=2)
        ax.set_yscale("log", base=10)
    ax.set_title(title, fontsize=12.5, fontweight="bold")
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    ax.grid(True, which="major", ls="--", lw=0.6, alpha=0.45)
    ax.grid(True, which="minor", ls=":", lw=0.4, alpha=0.22)
    return fig, ax


def save(fig, q, name):
    d = os.path.join(ROOT, q, "plots")
    os.makedirs(d, exist_ok=True)
    p = os.path.join(d, name)
    fig.tight_layout()
    fig.savefig(p, dpi=150)
    import matplotlib.pyplot as plt
    plt.close(fig)
    print("  wrote %s/plots/%s" % (q, name))


# ------------------------------------------------------------------------- Q1

def plot_q1(plt, text):
    rs = rows_flagged(text, 5, 2)           # n, count, 3n, flag, ops/n + 2 verdicts
    if not rs:
        raise SystemExit("Q1: no table rows parsed")
    n = col(rs, 0)
    count, three_n, flag = col(rs, 1), col(rs, 2), col(rs, 3)

    fig, ax = figure(plt,
                     "Q1: counting sort costs exactly 3n item touches — linear in n",
                     "n (number of pairs)", "item touches")
    ax.plot(n, count, "o-", color=BLUE, lw=2, label="counting sort, measured")
    ax.plot(n, three_n, "--", color=GREY, lw=2, label="3n (predicted)")
    ax.plot(n, flag, "s-", color=RED, lw=2, label="Dutch flag partition, measured")
    ax.legend(loc="upper left", framealpha=0.9)
    save(fig, "Q1", "1_linearity.png")

    # The n = 12 instance, parsed from the lines the program prints in full.
    # Each token is a number followed by a colour letter, e.g. "19R".
    inst = {}
    for line in text.splitlines():
        for tag, key in (("input", "input"), ("counting sort", "counting"),
                         ("dutch flag", "dutch")):
            if line.strip().startswith(tag):
                inst[key] = [(int(m.group(1)), m.group(2))
                             for m in re.finditer(r"(\d+)([RBY])", line)]
    fig, axes = plt.subplots(1, 2, figsize=(11.5, 4.9))
    fig.suptitle("Q1: both are linear and both group the colours — "
                 "only the stable one keeps the numbers sorted",
                 fontsize=12.5, fontweight="bold")
    for ax, key, name in ((axes[0], "counting", "counting sort (stable)"),
                          (axes[1], "dutch", "Dutch flag (unstable)")):
        items = inst.get(key, [])
        cmap = {"R": RED, "B": BLUE, "Y": ORANGE}
        for pos, (num, c) in enumerate(items):
            ax.bar(pos, num, color=cmap[c], edgecolor="black", lw=0.5)
        # Draw the within-colour trend: ascending is correct, a drop is a defect.
        for c in "RBY":
            xs = [p for p, (_, cc) in enumerate(items) if cc == c]
            ys = [v for (v, cc) in items if cc == c]
            if len(xs) > 1:
                bad = any(b <= a for a, b in zip(ys, ys[1:]))
                ax.plot(xs, ys, "o--" if bad else "o-",
                        color="black" if bad else "#333333",
                        lw=2.2 if bad else 1.4, ms=4,
                        zorder=3)
        ok = all(
            all(b > a for a, b in zip(
                [v for v, cc in items if cc == c],
                [v for v, cc in items if cc == c][1:]))
            for c in "RBY")
        ax.set_title("%s — numbers %s" % (name, "ascending" if ok else "SCRAMBLED"),
                     fontsize=11)
        ax.set_xlabel("position in output (red, then blue, then yellow)")
        ax.set_ylabel("number")
        ax.grid(True, axis="y", ls="--", lw=0.6, alpha=0.4)
    fig.tight_layout(rect=(0, 0, 1, 0.93))
    save(fig, "Q1", "2_stability.png")


# ------------------------------------------------------------------------- Q2

def plot_q2(plt, text):
    rs = rows(text, 7)          # n, sortS1, sortS2, search, walk, bf, ratio
    if not rs:
        raise SystemExit("Q2: no table rows parsed")
    n = col(rs, 0)
    s2, search, walk, bf, ratio = (col(rs, 2), col(rs, 3), col(rs, 4),
                                   col(rs, 5), col(rs, 6))

    fig, ax = figure(plt,
                     "Q2: sorting turns a quadratic search into an n log n one",
                     "n (size of each set)", "operations")
    ax.plot(n, s2, "o-", color=BLUE, lw=2, label="sort S₂ (comparisons)")
    ax.plot(n, search, "s-", color=RED, lw=2, label="binary searches (comparisons)")
    ax.plot(n, walk, "^-", color=GREEN, lw=2, label="two-pointer walk (steps)")
    bx, by = defined(n, bf)
    ax.plot(bx, by, "d--", color=GREY, lw=2, label="brute force (n² pairs)")
    ax.legend(loc="upper left", framealpha=0.9)
    save(fig, "Q2", "1_growth.png")

    fig, ax = figure(plt,
                     "Q2: cost divided by n log₂ n stays bounded — the bound holds",
                     "n (size of each set)", "(sort S₂ + searches) / n log₂ n",
                     loglog=False)
    ax.set_xscale("log", base=2)
    ax.plot(n, ratio, "o-", color=BLUE, lw=2, label="measured ratio")
    ax.axhline(2.0, ls="--", color=GREY, lw=1.6, label="bounded above by 2")
    ax.set_ylim(0, 2.6)
    ax.legend(loc="lower right", framealpha=0.9)
    save(fig, "Q2", "2_ratio.png")


# ------------------------------------------------------------------------- Q3

def plot_q3(plt, text):
    rs = rows(text, 7)          # k, n, probes, tuples, theory, ratio, bf
    if not rs:
        raise SystemExit("Q3: no table rows parsed")
    ks = sorted({r[0] for r in rs})
    colours = {2: BLUE, 3: RED, 4: GREEN}

    fig, ax = figure(plt,
                     "Q3: probes track n^(k-1) log n at every k",
                     "n (size of S)", "binary-search probes")
    for k in ks:
        sub = [r for r in rs if r[0] == k]
        ax.plot(col(sub, 1), col(sub, 2), "o-", color=colours[k], lw=2,
                label="k = %d, measured" % k)
        ax.plot(col(sub, 1), col(sub, 4), "--", color=colours[k], lw=1.4,
                alpha=0.65, label="k = %d, n^(k-1)·log₂n" % k)
    headroom(ax, rs, (2, 4))
    ax.legend(loc="upper left", fontsize=9, framealpha=0.9, ncol=2)
    save(fig, "Q3", "1_probes.png")

    # The ratio approaches 1/(k-1)! from below, because only increasing tuples
    # are enumerated: C(n-1,k-1) ~ n^(k-1)/(k-1)!.
    fact = {2: 1.0, 3: 2.0, 4: 6.0}
    fig, ax = figure(plt,
                     "Q3: the ratio rises toward 1/(k−1)! from below, never like n",
                     "n (size of S)", "probes / n^(k-1)·log₂n", loglog=False)
    ax.set_xscale("log", base=2)
    for k in ks:
        sub = [r for r in rs if r[0] == k]
        ax.plot(col(sub, 1), col(sub, 5), "o-", color=colours[k], lw=2,
                label="k = %d" % k)
        lim = 1.0 / fact[k]
        ax.axhline(lim, ls="--", color=colours[k], lw=1.4, alpha=0.7)
        # Anchored in axes coordinates on the left, where all three heights are
        # clear of both the curves and the legend; an annotation pinned to the
        # last data point spills past the right-hand frame instead.
        ax.text(0.015, lim, "1/(k−1)! = %.3f  (k=%d)" % (lim, k),
                transform=ax.get_yaxis_transform(), ha="left", va="bottom",
                fontsize=8.5, color=colours[k])
    ax.set_ylim(0, 1.15)
    ax.legend(loc="center right", framealpha=0.9)
    save(fig, "Q3", "2_ratio.png")

    fig, ax = figure(plt,
                     "Q3: one factor of n traded for a log n against brute force",
                     "n (size of S)", "subsets or probes examined")
    for k in ks:
        sub = [r for r in rs if r[0] == k]
        ax.plot(col(sub, 1), col(sub, 2), "o-", color=colours[k], lw=2,
                label="k = %d, this algorithm" % k)
        ax.plot(col(sub, 1), col(sub, 6), "d--", color=colours[k], lw=1.4,
                alpha=0.65, label="k = %d, brute force C(n,k)" % k)
    headroom(ax, rs, (2, 6))
    ax.legend(loc="upper left", fontsize=9, framealpha=0.9, ncol=2)
    save(fig, "Q3", "3_saving.png")


# ------------------------------------------------------------------------- Q4

def plot_q4(plt, text):
    rs = rows(text, 7)          # n, sortCmps, sweep, peak, peakT, bf, ratio
    if not rs:
        raise SystemExit("Q4: no table rows parsed")
    n = col(rs, 0)
    sortc, sweep, bf, ratio = col(rs, 1), col(rs, 2), col(rs, 5), col(rs, 6)

    fig, ax = figure(plt,
                     "Q4: sorting the 2n endpoints beats testing every pair",
                     "n (persons)", "operations")
    ax.plot(n, sortc, "o-", color=BLUE, lw=2, label="event sort (comparisons)")
    ax.plot(n, sweep, "^-", color=GREEN, lw=2, label="sweep steps (exactly 2n)")
    bx, by = defined(n, bf)
    ax.plot(bx, by, "d--", color=GREY, lw=2, label="brute force (≈n² checks)")
    ax.legend(loc="upper left", framealpha=0.9)
    save(fig, "Q4", "1_growth.png")

    fig, ax = figure(plt,
                     "Q4: comparisons per (2n)·log₂(2n) converge from below",
                     "n (persons)", "sort comparisons / (2n)·log₂(2n)", loglog=False)
    ax.set_xscale("log", base=2)
    ax.plot(n, ratio, "o-", color=BLUE, lw=2, label="measured ratio")
    ax.axhline(1.0, ls="--", color=GREY, lw=1.6, label="bounded above by 1")
    ax.set_ylim(0, 1.15)
    ax.legend(loc="lower right", framealpha=0.9)
    save(fig, "Q4", "2_ratio.png")

    # The printed worked example: a time row, a delta row and a count row.
    grab = {}
    for line in text.splitlines():
        s = line.split()
        if len(s) > 2 and s[0] in ("time", "delta", "count"):
            grab[s[0]] = [int(v) for v in s[1:]]
    if {"time", "count"} <= set(grab) and len(grab["time"]) == len(grab["count"]):
        t, c = grab["time"], grab["count"]
        fig, ax = figure(plt,
                         "Q4: occupancy only changes at a recorded endpoint",
                         "time", "people present", loglog=False, size=(9.5, 4.8))
        ax.step(t, c, where="post", color=BLUE, lw=2.2, label="occupancy")
        ax.plot(t, c, "o", color=BLUE, ms=5)
        peak = max(c)
        pt = t[c.index(peak)]
        ax.axvline(pt, ls="--", color=RED, lw=1.6)
        ax.annotate("peak = %d at t = %d" % (peak, pt), xy=(pt, peak),
                    xytext=(8, -14), textcoords="offset points",
                    color=RED, fontsize=10, fontweight="bold")
        ax.set_xticks(t)
        ax.set_ylim(0, peak + 1)
        ax.legend(loc="upper right", framealpha=0.9)
        save(fig, "Q4", "3_worked_example.png")


# ------------------------------------------------------------------------- Q5

def plot_q5(plt, text):
    rs = rows(text, 6)          # n, sortCmps, nlogn, ratio, mergeSteps, outCount
    if not rs:
        raise SystemExit("Q5: no table rows parsed")
    n = col(rs, 0)
    sortc, theory, ratio = col(rs, 1), col(rs, 2), col(rs, 3)
    steps, outc = col(rs, 4), col(rs, 5)

    fig, ax = figure(plt,
                     "Q5: the sort dominates; the merge pass is exactly one step per interval",
                     "n (intervals)", "operations")
    ax.plot(n, sortc, "o-", color=BLUE, lw=2, label="sort (comparisons)")
    ax.plot(n, theory, "--", color=GREY, lw=2, label="n·log₂n")
    ax.plot(n, steps, "^-", color=GREEN, lw=2, label="merge pass (n−1 steps)")
    ax.plot(n, outc, "s-", color=RED, lw=2, label="merged intervals returned")
    ax.legend(loc="upper left", framealpha=0.9)
    save(fig, "Q5", "1_growth.png")

    fig, axes = plt.subplots(1, 2, figsize=(11.5, 4.9))
    fig.suptitle("Q5: the bound holds, and on dense input the merging is substantial",
                 fontsize=12.5, fontweight="bold")
    ax = axes[0]
    ax.set_xscale("log", base=2)
    ax.plot(n, ratio, "o-", color=BLUE, lw=2)
    ax.axhline(1.0, ls="--", color=GREY, lw=1.6)
    ax.set_ylim(0, 1.15)
    ax.set_title("comparisons / n·log₂n, bounded below 1", fontsize=11)
    ax.set_xlabel("n (intervals)")
    ax.set_ylabel("ratio")
    ax.grid(True, ls="--", lw=0.6, alpha=0.45)
    ax = axes[1]
    ax.set_xscale("log", base=2)
    ax.plot(n, [100.0 * o / m for o, m in zip(outc, n)], "s-", color=RED, lw=2)
    ax.set_title("output size as a percentage of input", fontsize=11)
    ax.set_xlabel("n (intervals)")
    ax.set_ylabel("outCount / n  (%)")
    ax.grid(True, ls="--", lw=0.6, alpha=0.45)
    fig.tight_layout(rect=(0, 0, 1, 0.93))
    save(fig, "Q5", "2_ratio_and_compression.png")

    # The lab sheet's own example, drawn as bars: input above, merged below.
    got = re.search(r"input\s*:\s*((?:\(\d+,\d+\)\s*)+)", text)
    out = re.search(r"output\s*:\s*((?:\(\d+,\d+\)\s*)+)", text)
    if got and out:
        pairs = lambda s: [tuple(map(int, m.groups()))
                           for m in re.finditer(r"\((\d+),(\d+)\)", s)]
        gi, go = pairs(got.group(1)), pairs(out.group(1))
        fig, ax = figure(plt,
                         "Q5: the lab sheet's example — {(1,3),(2,6),(8,10),(7,18)} "
                         "merges to {(1,6),(7,18)}",
                         "coordinate", "", loglog=False, size=(9.5, 4.4))
        for i, (x, y) in enumerate(gi):
            ax.barh(len(gi) - i, y - x, left=x, height=0.6,
                    color=BLUE, alpha=0.75, edgecolor="black", lw=0.6)
            ax.text(x - 0.25, len(gi) - i, "(%d,%d)" % (x, y),
                    ha="right", va="center", fontsize=9)
        for i, (x, y) in enumerate(go):
            ax.barh(-1 - i, y - x, left=x, height=0.6,
                    color=RED, alpha=0.8, edgecolor="black", lw=0.6)
            ax.text(x - 0.25, -1 - i, "(%d,%d)" % (x, y),
                    ha="right", va="center", fontsize=9, fontweight="bold")
        ax.axhline(0.1, color="black", lw=1.0)
        ax.set_yticks([len(gi) - i for i in range(len(gi))] +
                      [-1 - i for i in range(len(go))])
        ax.set_yticklabels(["input"] * len(gi) + ["merged"] * len(go), fontsize=9)
        ax.set_xlim(-2.5, max(y for _, y in gi) + 1)
        ax.grid(True, axis="x", ls="--", lw=0.6, alpha=0.45)
        save(fig, "Q5", "3_worked_example.png")


# ------------------------------------------------------------------------- Q6

def plot_q6(plt, text):
    rs = rows(text, 7)          # n, sortCmps, sweep, depth, p, bf, ratio
    if not rs:
        raise SystemExit("Q6: no table rows parsed")
    n = col(rs, 0)
    sortc, sweep, bf, ratio = col(rs, 1), col(rs, 2), col(rs, 5), col(rs, 6)

    fig, ax = figure(plt,
                     "Q6: only the 2n endpoints are examined, not every point on the line",
                     "n (intervals)", "operations")
    ax.plot(n, sortc, "o-", color=BLUE, lw=2, label="event sort (comparisons)")
    ax.plot(n, sweep, "^-", color=GREEN, lw=2, label="sweep steps (exactly 2n)")
    bx, by = defined(n, bf)
    ax.plot(bx, by, "d--", color=GREY, lw=2, label="brute force (n² checks)")
    ax.legend(loc="upper left", framealpha=0.9)
    save(fig, "Q6", "1_growth.png")

    fig, ax = figure(plt,
                     "Q6: comparisons per (2n)·log₂(2n) creep toward 1, never past it",
                     "n (intervals)", "sort comparisons / (2n)·log₂(2n)", loglog=False)
    ax.set_xscale("log", base=2)
    ax.plot(n, ratio, "o-", color=BLUE, lw=2, label="measured ratio")
    ax.axhline(1.0, ls="--", color=GREY, lw=1.6, label="bounded above by 1")
    ax.set_ylim(0, 1.15)
    ax.legend(loc="lower right", framealpha=0.9)
    save(fig, "Q6", "2_ratio.png")

    # The lab sheet's example: the four intervals, and the depth beneath them.
    m = re.search(r"S\s*=\s*((?:\(\d+,\d+\)\s*)+)", text)
    if m:
        iv = [tuple(map(int, g.groups()))
              for g in re.finditer(r"\((\d+),(\d+)\)", m.group(1))]
        fig, axes = plt.subplots(2, 1, figsize=(9.8, 6.2),
                                 gridspec_kw={"height_ratios": [1.15, 1]},
                                 sharex=True)
        fig.suptitle("Q6: the lab sheet's example — no point lies in all four, "
                     "and p = 50 lies in three",
                     fontsize=12.5, fontweight="bold")
        ax = axes[0]
        for i, (l, r) in enumerate(iv):
            ax.barh(len(iv) - i, r - l, left=l, height=0.62, color=BLUE,
                    alpha=0.75, edgecolor="black", lw=0.6)
            ax.text(l - 1.5, len(iv) - i, "(%d,%d)" % (l, r),
                    ha="right", va="center", fontsize=9)
        ax.set_yticks([])
        ax.set_ylabel("the four intervals")
        ax.grid(True, axis="x", ls="--", lw=0.6, alpha=0.45)

        # Closed intervals, so depth at x counts l <= x <= r.
        xs = sorted({v for pair in iv for v in pair})
        lo, hi = min(xs) - 5, max(xs) + 5
        grid = list(range(lo, hi + 1))
        depth = [sum(1 for l, r in iv if l <= x <= r) for x in grid]
        ax = axes[1]
        ax.plot(grid, depth, color=RED, lw=2.2, label="depth (closed intervals)")
        ax.fill_between(grid, depth, color=RED, alpha=0.16)
        best = max(depth)
        ax.axhline(best, ls="--", color=GREY, lw=1.4)
        ax.plot([50], [sum(1 for l, r in iv if l <= 50 <= r)], "o",
                color="black", ms=8, zorder=4)
        ax.annotate("p = 50, depth %d" % sum(1 for l, r in iv if l <= 50 <= r),
                    xy=(50, sum(1 for l, r in iv if l <= 50 <= r)),
                    xytext=(10, 8), textcoords="offset points",
                    fontsize=10, fontweight="bold")
        ax.set_ylim(0, len(iv) + 0.6)
        ax.set_yticks(range(len(iv) + 1))
        ax.set_xlabel("point p on the line")
        ax.set_ylabel("intervals containing p")
        ax.grid(True, ls="--", lw=0.6, alpha=0.45)
        ax.legend(loc="upper right", framealpha=0.9)
        fig.tight_layout(rect=(0, 0, 1, 0.93))
        save(fig, "Q6", "3_worked_example.png")


PLOTS = {"Q1": plot_q1, "Q2": plot_q2, "Q3": plot_q3,
         "Q4": plot_q4, "Q5": plot_q5, "Q6": plot_q6}


def main(argv):
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt
    plt.rcParams.update({
        "font.size": 10.5,
        "axes.titlesize": 12.5,
        "axes.edgecolor": "#444444",
        "figure.facecolor": "white",
        "savefig.facecolor": "white",
    })
    wanted = [a.upper() for a in argv[1:]] or sorted(SOURCES)
    for q in wanted:
        if q not in PLOTS:
            raise SystemExit("unknown question %r" % q)
        print("%s: compiling and running..." % q)
        PLOTS[q](plt, run(q))
    print("done.")


if __name__ == "__main__":
    main(sys.argv)
