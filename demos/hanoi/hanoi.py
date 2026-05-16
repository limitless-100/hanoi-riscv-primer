#!/usr/bin/env python3
"""
Tower of Hanoi — ASCII visualizer
MIT RISC-V Porting Project  |  Mentor: Kurt Keville
"""

import time, sys

# ── configuration ────────────────────────────────────────────────────────────
DISKS   = int(sys.argv[1]) if len(sys.argv) > 1 else 5
DELAY   = 0.35          # seconds between frames
COLS    = 60            # terminal width for the display
MOVES   = [0]           # mutable counter shared across recursive calls

# ── state ────────────────────────────────────────────────────────────────────
# Three pegs, each a stack (list) of disk widths; widest disk == DISKS.
pegs: dict[str, list[int]] = {
    "A": list(range(DISKS, 0, -1)),   # source  — pre-loaded large→small
    "B": [],                           # auxiliary
    "C": [],                           # target
}

# ── rendering ────────────────────────────────────────────────────────────────
def draw(label: str = "") -> None:
    """Render the three pegs as a single ASCII frame."""
    col_w = COLS // 3
    rows  = []

    for row in range(DISKS - 1, -1, -1):          # top row → floor row
        line = ""
        for peg in ("A", "B", "C"):
            stack = pegs[peg]
            if row < len(stack):
                w    = stack[row]
                disk = ("█" * w).center(col_w)     # filled block glyph
            else:
                disk = "|".center(col_w)            # empty rod
            line += disk
        rows.append(line)

    rows.append("=" * COLS)                        # floor
    rows.append(
        f"{'A':^{col_w}}{'B':^{col_w}}{'C':^{col_w}}"
        f"   move {MOVES[0]:>4}   {label}"
    )

    # ANSI cursor-up trick: overwrite the previous frame in-place
    if MOVES[0] > 0:
        sys.stdout.write(f"\033[{len(rows)}A")

    sys.stdout.write("\n".join(rows) + "\n")
    sys.stdout.flush()
    time.sleep(DELAY)

# ── core algorithm ───────────────────────────────────────────────────────────
# RECURSION — the classic O(2ⁿ − 1) divide-and-conquer decomposition.
#
# Base case  : a single disk moves directly; no further decomposition needed.
# Inductive  : to move n disks from src → tgt using aux as buffer —
#              1. move n-1 disks src → aux  (recursive sub-problem)
#              2. move disk n    src → tgt  (one direct operation)
#              3. move n-1 disks aux → tgt  (recursive sub-problem)
#
# Each level halves the remaining problem; the recursion tree has depth n
# and 2ⁿ − 1 leaves, which is provably optimal for this puzzle.

def hanoi(n: int, src: str, tgt: str, aux: str) -> None:
    """Move n disks from src to tgt, using aux as the buffer peg."""

    if n == 0:          # base case — nothing to move
        return

    # ── step 1 : expose the bottom disk ──────────────────────────────────────
    hanoi(n - 1, src, aux, tgt)

    # ── step 2 : one atomic move (the single direct operation per call) ───────
    disk = pegs[src].pop()
    pegs[tgt].append(disk)
    MOVES[0] += 1
    draw(f"{src}→{tgt}")

    # ── step 3 : rebuild the stack on the target peg ──────────────────────────
    hanoi(n - 1, aux, tgt, src)

# ── entry point ──────────────────────────────────────────────────────────────
if __name__ == "__main__":
    print(f"Tower of Hanoi  —  {DISKS} disks  —  {2**DISKS - 1} optimal moves\n")
    draw("initial state")          # render before any move
    hanoi(DISKS, "A", "C", "B")   # solve: source=A  target=C  aux=B
    print(f"\nSolved in {MOVES[0]} moves  (optimal: {2**DISKS - 1})")
