# RV32I

High-performance, zero-dependency RV32I execution engine in C11. Features native ELF32 binary loading, interactive CLI disassembler & register debugger. Zero bloat.

## Overview

**RV32I** is a minimal, fast RISC-V 32-bit integer (RV32I ISA) emulation engine. It loads and executes ELF32 binaries compiled for RISC-V, decodes instructions on-the-fly, and optionally displays execution traces with human-readable mnemonics.

Perfect for:
- Learning RISC-V instruction execution
- Embedded systems testing
- Lightweight binary verification
- Custom RISC-V toolchains

## Features

- **Native ELF32 Loading** — Parses and loads ELF32 binaries directly into simulated memory with validation
- **Complete RV32I Decoder** — Supports all 6 instruction formats (R, I, S, B, U, J) with inline decoding
- **Interactive Disassembler** — Human-readable instruction mnemonics (ADD, ADDI, SUB, JAL, LUI, SW, etc.)
- **Execution Tracer** — Optional `--trace` mode prints each instruction before execution
- **Simulated I/O** — UART MMIO at `0x80000000` for character output
- **Minimal Footprint** — Single-pass compilation, ~300 lines of core logic
- **POSIX Portable** — Builds on Linux, macOS, and other Unix-like systems
- **CMake Support** — Optional CMake build system for flexible compilation

## Building

### Using Make (Recommended)

```bash
make              # Compile to ./rv32i_core
make clean        # Remove build artifacts
```

### Using CMake

```bash
mkdir build
cd build
cmake ..
make
```

**Requirements:** `gcc` (or compatible C11 compiler), `make` (and `cmake` for CMake builds)

## Usage

```bash
# Execute an ELF32 RISC-V binary
./rv32i_core <binary.elf>

# Run with execution trace (prints every instruction)
./rv32i_core <binary.elf> --trace
```

**Output:**
- Normal mode: program output (via UART MMIO) only
- Trace mode: `[PC_ADDRESS] INSTRUCTION_MNEMONIC` for each step, followed by program output

## Architecture

```
engine/
  cpu.c / cpu.h        32 registers, PC, halt flag; execution loop (cpu_step)
  memory.c / memory.h  1 MB linear RAM; 32-bit aligned I/O with safety checks
  elf_loader.c / .h    ELF32 parser and binary loader with validation

disassemble/
  disasm.c / disasm.h  Instruction-to-assembly conversion with ABI register names

shared/
  instruction.h        Inline instruction decoder for 6 formats
  types.h              Shared types and constants

main.c                 CLI entry point and main loop
Makefile               Make build rules
CMakeLists.txt         CMake build configuration
```

### Execution Flow

1. `main()` opens the ELF file and calls `elf_load()`
2. `elf_load()` parses the ELF header, validates architecture, maps sections into memory, returns entry point
3. `cpu_init()` sets up registers and PC to entry point
4. Main loop: `cpu_step()` repeatedly:
   - Fetches raw instruction from memory at PC
   - Decodes via `rv_decode()` (inline, no overhead)
   - Executes matching opcode (ADD, ADDI, SUB, JAL, LUI, AUIPC, SW, LW, etc.)
   - Updates PC and registers
   - Halts on ECALL (`0x73`) or unknown instruction
5. Optional trace mode prints disassembly before each step

## Supported Instructions

| Type | Opcode | Mnemonics |
|------|--------|-----------|
| **U-type** | `0x37`, `0x17` | LUI, AUIPC |
| **J-type** | `0x6F` | JAL |
| **I-type** | `0x13`, `0x67`, `0x03`, `0x73` | ADDI, JALR, LW, ECALL |
| **B-type** | `0x63` | BEQ, BNE, BLT, BGE, BLTU, BGEU |
| **R-type** | `0x33` | ADD, SUB, SLL, SRL, SRA, XOR, OR, AND |
| **S-type** | `0x23` | SB, SH, SW (store) |

## Memory Map

```
0x00000000 – 0x0FFFFF    Program & Data (1 MB RAM)
0x80000000               UART MMIO (character output)
```

Writing a byte to `0x80000000` outputs it to stdout.

## Example

Create a minimal RV32I binary (e.g., with riscv64-unknown-elf toolchain):

```asm
_start:
    addi x1, x0, 42      # x1 = 42
    ecall                # Halt
```

Compile and run:
```bash
riscv64-unknown-elf-gcc -c -O0 example.s -o example.o
riscv64-unknown-elf-ld -Ttext 0x0 example.o -o example.elf
./rv32i_core example.elf --trace
```

Output (trace mode):
```
[0x00000000] addi   x1, x0, 42
[0x00000004] ecall
```

## Design Principles

- **Zero external dependencies** — Only libc
- **Clarity over performance** — Readable C11, minimal macro usage
- **Single responsibility** — Separation: loader, CPU, memory, disassembler
- **Inline where it counts** — Instruction decoding is `static inline` for zero overhead
- **Safe by default** — Error handling, bounds checking, validation

## Limitations

- **Subset of RV32I** — Implements ~25 key instructions; extensible
- **No privilege levels** — No U-mode/S-mode/M-mode separation
- **No interrupts** — Single-threaded, blocking execution
- **Strict alignment** — 32-bit memory I/O must be word-aligned
- **No caching** — Direct memory access (suitable for small binaries)

## Version History

### v2 (Current — 2026-07-25)
**Major improvements to robustness and compatibility:**
- ✅ **ELF Validation** — Added ELF magic check and RISC-V architecture validation in `elf_load()`
- ✅ **Enhanced Error Handling** — Improved error messages for memory allocation and ELF loading failures
- ✅ **Memory Safety** — Added comprehensive bounds checking and NULL pointer guards
- ✅ **CPU Refactoring** — Refactored `cpu_init()` and `cpu_step()` for improved clarity and safety
- ✅ **Instruction Decoder Fix** — Fixed J-type immediate bit mask calculation (0x01000000 → 0x00100000)
- ✅ **Disassembler Improvements** — Enhanced register index validation and added ABI register name table
- ✅ **Build System** — Added CMake support for flexible compilation workflows
- ✅ **Extended Instruction Support** — Added full branch family (BEQ, BNE, BLT, BGE, BLTU, BGEU) and load/store variants (LB, LH, LW, LBU, LHU, SB, SH, SW)

### v1 (2026-07-24)
**Initial release with core functionality:**
- Basic RV32I execution engine
- ELF32 binary loading
- Interactive disassembler with trace mode
- Fundamental instruction set support
- Makefile-based build system

## License

MIT License — See LICENSE file

## Contributing

Fork, branch, and submit a pull request. Focus areas:
- Expand instruction set (multiply/divide, floating-point)
- Add memory protection / privilege modes
- Optimize for large binaries
- Cross-platform testing
- Performance profiling

---

**Built with ❤️ for RISC-V enthusiasts.**
