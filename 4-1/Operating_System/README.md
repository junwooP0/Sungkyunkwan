Operating Systems Coursework (xv6-based projects)

## Contents
- `xv6-riscv/` : xv6 (RISC-V) used for Projects 0–3
- `pa4_sklt_xv6/` : xv6 (RISC-V) used for Projects 4

## Project Summary
- **Project 0 (Booting xv6)**: Print student info / custom message during xv6 boot
- **Project 1 (System Calls)**: Implement 5 system calls: `getnice`, `setnice`, `ps`, `meminfo`, `waitpid`
- **Project 2 (CPU Scheduling)**: Replace xv6 scheduler with Linux-like **EEVDF** scheduling and extend `ps`
- **Project 3 (Virtual Memory)**: Implement `mmap/munmap/freemem` + page-fault handling for memory mapping
- **Project 4 (Page Replacement)**: Implement page-level swapping (swap-in/out) and manage swappable pages with an LRU list using the clock algorithm
