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

### GDB — Debugger

Compile with debug symbols (no `-O2`):

```bash
gcc -Wall -Werror -std=gnu11 -g -lm "Data Structures Project.c" -o "Data Structures Project"
```

Launch the debugger:

```bash
gdb ./"Data Structures Project"
```

Useful GDB commands:

| Command | Description |
|---|---|
| `break main` | Set a breakpoint at `main` |
| `run < input.txt` | Run the program with input |
| `next` | Execute next line |
| `step` | Step into a function |
| `print var` | Print the value of a variable |
| `backtrace` | Show the call stack |
| `quit` | Exit GDB |

---

### Valgrind — Memory Leak Detection

```bash
valgrind --leak-check=full --show-leak-kinds=all ./"Data Structures Project" < input.txt
```

Useful flags:

| Flag | Description |
|---|---|
| `--leak-check=full` | Detailed report of all leaks |
| `--show-leak-kinds=all` | Show direct, indirect, and possible leaks |
| `--track-origins=yes` | Track origin of uninitialized values |
| `--error-exitcode=1` | Return non-zero exit code on errors |

---

### gprof — Performance Profiling

Compile with profiling enabled:

```bash
gcc -Wall -Werror -std=gnu11 -pg -lm "Data Structures Project.c" -o "Data Structures Project"
```

Run the program to generate profiling data:

```bash
./"Data Structures Project" < input.txt
```

Analyze the results:

```bash
gprof ./"Data Structures Project" gmon.out > analysis.txt
cat analysis.txt
```

This shows how much time is spent in each function — useful for identifying bottlenecks in Dijkstra or the hash table.
