# Tower of Hanoi — C++ ASCII Demo

MIT RISC-V Porting Project · Mentor: Kurt Keville  
Repository: [Hardware-Abstraction-Layer-Transitional-Libraries](https://github.com/clusterchallenge/Hardware-Abstraction-Layer-Transitional-Libraries)

---

## What it does

An animated, in-place ASCII visualizer for the Tower of Hanoi puzzle, written
in C++17 with double-precision arithmetic throughout — consistent with the
project's HPC numerical requirements.

```
     #####      |            |          move   31   A->C
    #######     |            |
   #########    |            |
  ###########   |            |
 #############  |            |
============================================================
  A               B               C
```

---

## Algorithm

| Property | Value |
|---|---|
| Strategy | Divide-and-conquer recursion |
| Time complexity | O(2ⁿ − 1) moves — provably optimal |
| Space complexity | O(n) call-stack depth |
| Numeric type | `double` (64-bit IEEE-754) — supports n up to ~53 without overflow |

**Three-step recursive decomposition:**

1. Move **n − 1** disks `src → aux` — clears the path to the bottom disk
2. Move disk **n** `src → tgt` — the single direct operation per call
3. Move **n − 1** disks `aux → tgt` — rebuild the tower on the target peg

---

## Build & Run

### Linux / macOS / Git Bash on Windows (MinGW)

```bash
# compile
g++ -O2 -std=c++17 -o hanoi hanoi.cpp

# run (default 5 disks)
./hanoi

# run with custom disk count
./hanoi 7
```

### Windows (MSVC — Developer Command Prompt)

```cmd
cl /EHsc /std:c++17 /O2 hanoi.cpp /Fe:hanoi.exe
hanoi.exe 5
```

### Cross-compile for RISC-V

```bash
riscv64-unknown-elf-g++ -O2 -std=c++17 -o hanoi_riscv hanoi.cpp

# run under QEMU user-mode emulation
qemu-riscv64 ./hanoi_riscv 5

# or simulate on Spike + proxy kernel
spike pk hanoi_riscv 5
```

---

## Optimization flag comparison (RISC-V ABI study)

Compiling at different `-O` levels shows how `g++` handles recursive
call-frame generation on RISC-V — a core concern for the HAL porting project:

```bash
# unoptimized — full prologue/epilogue on every call
riscv64-unknown-elf-g++ -O0 -std=c++17 -S -o hanoi_O0.s hanoi.cpp

# optimized — observe tail-call and inlining decisions
riscv64-unknown-elf-g++ -O2 -std=c++17 -S -o hanoi_O2.s hanoi.cpp

# diff the two assembly files to see exactly what the compiler changed
diff hanoi_O0.s hanoi_O2.s
```

---

## Benchmarking

```bash
# wall-clock time for a large workload
time ./hanoi 20        # 1,048,575 moves

# compare x86 vs RISC-V timing
time ./hanoi 20
time qemu-riscv64 ./hanoi_riscv 20
```

---


## License

MIT
