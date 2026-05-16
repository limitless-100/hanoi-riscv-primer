# Tower of Hanoi — ASCII Demo

MIT RISC-V Porting Project · Mentor: Kurt Keville  
Repository: [Hardware-Abstraction-Layer-Transitional-Libraries](https://github.com/clusterchallenge/Hardware-Abstraction-Layer-Transitional-Libraries)

---

## What it does

An animated, in-place ASCII visualizer for the classic Tower of Hanoi puzzle.  
Every disk move is rendered live in the terminal with Unicode block glyphs.

```
     ██████      |            |          move   31   A→C
    ████████     |            |
   ██████████    |            |
  ████████████   |            |
 ██████████████  |            |
============================================================
       A               B               C
```

## Algorithm

| Property | Value |
|---|---|
| Strategy | Divide-and-conquer recursion |
| Time complexity | O(2ⁿ − 1) moves — provably optimal |
| Space complexity | O(n) call-stack depth |
| Move count | 2ⁿ − 1 for n disks |

The three-step recursive decomposition:

1. Move **n − 1** disks `src → aux` (clears the path)  
2. Move disk **n** `src → tgt` (single direct operation)  
3. Move **n − 1** disks `aux → tgt` (rebuild on target)

This maps cleanly to RISC-V's `JAL`/`JALR` call/return mechanism and is a
canonical example used when teaching stack-frame behaviour on new ISAs.

## Usage

```bash
# default: 5 disks
python3 hanoi.py

# custom disk count
python3 hanoi.py 7
```

Requires Python 3.8+, a VT100-compatible terminal, and nothing else.

## RISC-V relevance

- The recursion depth equals the disk count, making stack pressure predictable
  and measurable — useful for profiling RISC-V ABI call overhead.
- The 2ⁿ − 1 move count provides a deterministic workload for cycle-accurate
  simulation (Spike, QEMU) or benchmarking on physical silicon.
- Porting this to C and compiling with `riscv64-unknown-elf-gcc` with varying
  `-O` flags illustrates how the compiler unwinds tail-adjacent recursion.

## License

MIT
