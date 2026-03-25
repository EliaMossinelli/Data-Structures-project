# Data-Structures-project

A C implementation of an optimal routing system over a hexagonal grid map, developed as the final project for the **Algorithms and Data Structures** course at **Politecnico di Milano** (2024–2025).

---

## Overview

The program models a geographic surface as a rectangular grid of hexagonal tiles. Each hexagon has a traversal cost, and vehicles move between adjacent hexagons paying the departure cost. The system supports dynamic cost updates over circular regions and toggleable one-way air routes, then answers shortest-path queries efficiently.

---

## Commands

| Command | Description |
|---|---|
| `init <cols> <rows>` | Initializes the hexagonal map |
| `change_cost <x> <y> <v> <radius>` | Updates traversal costs over a hexagonal radius using a distance-weighted formula |
| `toggle_air_route <x1> <y1> <x2> <y2>` | Adds or removes a directed air connection between two hexagons |
| `travel_cost <xp> <yp> <xd> <yd>` | Computes the minimum travel cost between two hexagons |

---

## Key Design Choices

- **Dijkstra with lazy deletion** on a min-heap for shortest-path queries
- **Hash table cache** (linear probing) to avoid recomputing repeated `travel_cost` queries — particularly effective given that queries tend to cluster in the same map regions
- **Cube coordinate system** for correct hexagonal distance computation
- **Dynamic memory allocated once** at `init` time and reused across queries to minimize overhead

---

## Build & Run

> **Note:** the following instructions are for Linux/macOS (or Windows with WSL — see [how to install WSL](https://learn.microsoft.com/en-us/windows/wsl/install)).

Compile with the same flags used by the grader:

```bash
gcc -Wall -Werror -std=gnu11 -O2 -lm "Data Structures Project.c" -o "Data Structures Project"
```

Run with an input file:

```bash
./"Data Structures Project" < input.txt
```

Save output to a file:

```bash
./"Data Structures Project" < input.txt > my_output.txt
```

---

## Development

Tools used during development on Linux.

> **Note:** Valgrind and ASAN are **incompatible** — they partially overlap in functionality. Remove `-fsanitize=address` before running any Valgrind tool.

Install all tools on Debian/Ubuntu:

```bash
sudo apt install gdb valgrind kcachegrind massif-visualizer build-essential
```

---

### GDB — Debugger

Compile with debug symbols (replace `-O2` with `-g3`):

```bash
gcc -Wall -Werror -std=gnu11 -g3 -lm "Data Structures Project.c" -o "Data Structures Project"
```

Launch the debugger:

```bash
gdb ./"Data Structures Project"
```

Useful GDB commands:

| Command | Description |
|---|---|
| `run < input.txt` | Run the program with input |
| `break main` | Set a breakpoint at `main` |
| `next` | Execute next line |
| `step` | Step into a function |
| `print var` | Print the value of a variable |
| `backtrace` | Show the call stack |
| `quit` | Exit GDB |

Full reference: http://users.ece.utexas.edu/~adnan/gdb-refcard.pdf

---

### ASAN — Address SANitizer

Detects out-of-bounds memory accesses with byte-level precision. Use this **before** Valgrind for a faster feedback loop.

Compile with:

```bash
gcc -Wall -Werror -std=gnu11 -g3 -fsanitize=address -lm "Data Structures Project.c" -o "Data Structures Project"
```

Run normally — the program will abort and print a detailed report if a memory error is detected:

```bash
./"Data Structures Project" < input.txt
```

---

### Valgrind — Memcheck

Detects memory leaks, use-after-free, double-free, and reads from uninitialized variables.

> Remove `-fsanitize=address` from compilation flags before using Valgrind.

```bash
gcc -Wall -Werror -std=gnu11 -g3 -lm "Data Structures Project.c" -o "Data Structures Project"
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./"Data Structures Project" < input.txt
```

| Flag | Description |
|---|---|
| `--leak-check=full` | Detailed report of all leaks |
| `--show-leak-kinds=all` | Show direct, indirect, and possible leaks |
| `--track-origins=yes` | Track origin of uninitialized values |

Reference: https://valgrind.org

---

### Callgrind — CPU Profiling

Instruments the program to measure time spent in each function. Useful for identifying bottlenecks in Dijkstra or the hash table.

```bash
valgrind --tool=callgrind ./"Data Structures Project" < input.txt
kcachegrind callgrind.out.<PID>
```

Replace `<PID>` with the process ID printed by Valgrind at the end of the run.

---

### Massif — Heap Memory Profiling

Tracks heap memory usage over time, showing which allocations consume the most memory.

```bash
valgrind --tool=massif ./"Data Structures Project" < input.txt
massif-visualizer massif.out.<PID>
```

Replace `<PID>` with the process ID printed by Valgrind at the end of the run.
