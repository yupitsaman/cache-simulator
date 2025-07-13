# Multi-Level Cache Hierarchy Simulator

This project is a comprehensive C++ simulator for a three-level (L1, L2, L3) inclusive cache hierarchy. It serves as an educational tool for understanding the core principles of cache architecture, including memory access, hit/miss logic, replacement policies, and performance analysis. Users can configure all key architectural parameters to experiment with different designs and observe their impact on performance.

## Features

*   **Three-Level Cache Hierarchy:** Simulates a complete L1, L2, and L3 cache system.
*   **Fully Configurable:** Users can define the following for each cache level:
    *   Total Cache Size (in bytes)
    *   Block Size (in bytes)
    *   Associativity (N-way set associative, including direct-mapped and fully associative)
    *   Hit Latency (in cycles)
*   **Replacement Policies:** Supports both **Least Recently Used (LRU)** and **First-In, First-Out (FIFO)** policies, chosen once for the entire hierarchy.
*   **Inclusive Cache Policy:** The simulator correctly maintains cache inclusivity. If a data block is present in a higher-level cache (e.g., L1), it is guaranteed to be present in all lower-level caches (L2, L3).
*   **Detailed Performance Metrics:** Provides a comprehensive report upon completion, including:
    *   **Per-Level Statistics:** Access count, hits, misses, hit rate, and miss rate for each cache level.
    *   **Overall System Performance:** Total simulation cycles and the calculated Average Memory Access Time (AMAT).
    *   **Verification:** A theoretical, formula-based AMAT calculation is also provided to verify the accuracy of the direct simulation.

## Project Structure

The project is organized into a modular structure for clarity, maintainability, and ease of understanding.

```
cache-simulator/
├── cache_config.h        # Defines core data structures (structs) and enums for configuration.
├── cache_line.h          # Defines the structure of a single cache line (the smallest data unit).
├── cache_level.h         # Header file for the CacheLevel class, defining its public interface.
├── cache_level.cpp       # Implementation of the core cache level logic (hit, miss, replacement).
├── cache_simulator.h     # Header file for the main CacheSimulator class.
├── cache_simulator.cpp   # Implementation of the hierarchy manager and statistics aggregator.
├── main.cpp              # The main application driver, handling all user interaction.
└── generate_trace.py     # A Python utility to generate random memory access traces for testing.
```

## Getting Started

### Prerequisites

To compile and run this project, you will need:
*   A C++ compiler that supports the C++17 standard (e.g., `g++` or `clang++`).
*   For Windows users, `g++` can be installed via a toolset like **MSYS2** or **MinGW-w64**.

### Compilation

This project does not use a Makefile and can be compiled directly from the command line.

1.  Open a terminal or command prompt (like PowerShell, Git Bash, or a standard Linux/macOS terminal).
2.  Navigate to the project's root directory (`cache-simulator/`).
3.  Run the following command to compile all C++ source files into a single executable:

    ```bash
    g++ main.cpp cache_simulator.cpp cache_level.cpp -o cache_sim.exe -std=c++17 -Wall
    ```

### Running the Simulator

1.  After a successful compilation, an executable file will be created in your directory.
2.  Run the simulator from your terminal:

    ```bash
    ./cache_sim.exe
    ```

3.  The program will guide you through a series of prompts to configure the cache system and then ask for the memory access trace.

#### Example Input Trace

When prompted, you can enter a mix of hexadecimal (starting with `0x`) and decimal addresses, separated by commas.

```
Enter memory access trace as comma-separated hexadecimal or decimal addresses...
0x100, 4096, 0x140, 0x100, 0x180, 4352, 0x1c0, 0x100
```

## Using the Trace Generator

A Python script, `generate_trace.py`, is included to help create test inputs.

### Prerequisites for the Script
*   Python 3 must be installed on your system.

### How to Use
1.  Run the script from your terminal:
    ```bash
    python generate_trace.py
    ```
2.  Follow the prompts to enter the desired number of addresses and the address range.
3.  The script will print a formatted, comma-separated list of hexadecimal addresses to the console.
4.  Copy this entire output string and paste it into the C++ simulator when it prompts for the memory access trace.

## Example Output

After a successful run, the simulator will produce a detailed report similar to this:

```text
=============== Simulation Results ===============
Total Memory Accesses Requested: 1000
Total Simulation Cycles:         33090
Average Memory Access Time (AMAT): 33.0900 cycles

--- L1 Cache Stats ---
 Accesses: 1000
 Hits:     730
 Misses:   270
 Hit Rate: 73.0000%
 Miss Rate:27.0000%

--- L2 Cache Stats ---
 Accesses: 270
 Hits:     65
 Misses:   205
 Hit Rate: 24.0741%
 Miss Rate:75.9259%

--- L3 Cache Stats ---
 Accesses: 205
 Hits:     8
 Misses:   197
 Hit Rate: 3.9024%
 Miss Rate:96.0976%
==================================================

AMAT Calculation (Formula-based):
  AMAT L3 = L3_HitT + (L3_MR * MemLat) = 30.0000 + (0.9610 * 100.0000) = 126.0976
  AMAT L2 = L2_HitT + (L2_MR * AMAT_L3) = 12.0000 + (0.7593 * 126.0976) = 107.7407
  AMAT L1 = L1_HitT + (L1_MR * AMAT_L2) = 4.0000 + (0.2700 * 107.7407) = 33.0900
(Formula AMAT should closely match the simulated AMAT: 33.0900)
==================================================
```