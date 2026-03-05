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

Add `-g3` for debug symbols (useful with GDB and Valgrind):

```bash
gcc -Wall -Werror -std=gnu11 -O2 -lm -g3 "Data Structures Project.c" -o "Data Structures Project"
```

Run with an input file:

```bash
./"Data Structures Project" < input.txt
```

Save output to a file:

```bash
./"Data Structures Project" < input.txt > my_output.txt
```
