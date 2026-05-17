/*
 * Tower of Hanoi — ASCII visualizer
 * MIT RISC-V Porting Project  |  Mentor: Kurt Keville
 *
 * Build : g++ -O2 -std=c++17 -o hanoi hanoi.cpp
 * Run   : ./hanoi [disks]          (default: 5)
 *
 * Double-precision note: move_count and optimal are declared as double
 * to stay consistent with the project's HPC double-precision requirement
 * and to handle disk counts large enough to overflow a 32-bit integer
 * (2^53 moves fits safely in a 64-bit IEEE-754 double).
 */

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <cmath>
#include <cstdlib>
#include <cassert>

// ── configuration ────────────────────────────────────────────────────────────
static int    DISKS  = 5;
static double DELAY  = 0.25;        // seconds between frames (double precision)
static int    COLS   = 60;

// ── state ────────────────────────────────────────────────────────────────────
// Three pegs as stacks; disk value == its width (DISKS = widest).
static std::vector<int> pegs[3];    // 0=A  1=B  2=C
static double move_count = 0.0;     // double: supports very large disk counts

// ── rendering ────────────────────────────────────────────────────────────────
void draw(const std::string& label)
{
    int col_w = COLS / 3;

    // Build each row top-to-bottom, then print all at once.
    // Cursor-up escape overwrites the previous frame in-place.
    std::string frame;
    int total_rows = DISKS + 2;     // disk rows + floor + label row

    for (int row = DISKS - 1; row >= 0; --row) {
        for (int p = 0; p < 3; ++p) {
            std::string cell;
            if (row < (int)pegs[p].size()) {
                int w = pegs[p][row];
                cell = std::string(w, '#');         // ASCII-safe disk glyph
            } else {
                cell = "|";                         // empty rod
            }
            // centre the cell within col_w characters
            int pad   = col_w - (int)cell.size();
            int left  = pad / 2;
            int right = pad - left;
            frame += std::string(left, ' ') + cell + std::string(right, ' ');
        }
        frame += '\n';
    }

    frame += std::string(COLS, '=') + '\n';

    // Label row: peg names + move counter
    char label_buf[128];
    std::snprintf(label_buf, sizeof(label_buf),
        "%-*s%-*s%-*s  move %4.0f   %s\n",
        col_w, "  A", col_w, "  B", col_w, "  C",
        move_count, label.c_str());
    frame += label_buf;

    // Overwrite previous frame (skip on first draw)
    if (move_count > 0.0)
        std::cout << "\033[" << total_rows << "A";

    std::cout << frame << std::flush;

    // Platform-portable sleep using <chrono>
    std::this_thread::sleep_for(
        std::chrono::milliseconds(static_cast<int>(DELAY * 1000)));
}

// ── core algorithm ───────────────────────────────────────────────────────────
// RECURSION — O(2ⁿ − 1) divide-and-conquer, provably optimal move count.
//
// Base case  : n == 0, nothing to do; unwinds the call stack.
// Inductive  : move n−1 disks out of the way (src→aux), shift the exposed
//              bottom disk (src→tgt), then rebuild on top (aux→tgt).
//
// Each recursive call reduces n by 1; the tree has depth n and
// 2ⁿ − 1 leaf nodes — one per actual disk move.

void hanoi(int n, int src, int tgt, int aux)
{
    if (n == 0) return;             // base case — terminates recursion

    // ── step 1 : clear the path ───────────────────────────────────────────
    hanoi(n - 1, src, aux, tgt);

    // ── step 2 : single direct move (the atomic operation per call) ───────
    pegs[tgt].push_back(pegs[src].back());
    pegs[src].pop_back();
    move_count += 1.0;              // double-precision increment

    const char* names[] = {"A", "B", "C"};
    std::string lbl = std::string(names[src]) + "->" + names[tgt];
    draw(lbl);

    // ── step 3 : rebuild on target ────────────────────────────────────────
    hanoi(n - 1, aux, tgt, src);
}

// ── entry point ──────────────────────────────────────────────────────────────
int main(int argc, char* argv[])
{
    if (argc > 1) DISKS = std::atoi(argv[1]);

    // optimal move count as double — handles DISKS > 30 without overflow
    double optimal = std::pow(2.0, DISKS) - 1.0;

    // Seed peg A: widest disk at bottom (index 0), narrowest at top
    for (int i = DISKS; i >= 1; --i)
        pegs[0].push_back(i);

    std::cout << "Tower of Hanoi  --  " << DISKS
              << " disks  --  " << static_cast<long long>(optimal)
              << " optimal moves\n\n";

    draw("initial state");
    hanoi(DISKS, 0, 2, 1);         // src=A(0)  tgt=C(2)  aux=B(1)

    // ── correctness assertions ────────────────────────────────────────────
    // Verify final state matches expected solution exactly.
    assert(pegs[2].size() == static_cast<size_t>(DISKS) && "Solve failed");
    assert(move_count == optimal                         && "Non-optimal move count");

    std::cout << "\nSolved in " << static_cast<long long>(move_count)
              << " moves  (optimal: " << static_cast<long long>(optimal) << ")\n";
    std::cout << "Assertions passed.\n";

    return 0;
}
