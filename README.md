# Maze Generator with Graphs

A C++ maze generator that uses graph-based algorithms to create solvable mazes with guaranteed shortest paths.

## 🎯 Overview

This project generates random mazes represented as 2D grids where each tile can connect to its neighbors through enabled/disabled paths. The generator uses a **spanning tree algorithm** combined with **BFS pathfinding** to ensure every maze has a valid solution.

## 🏗️ Architecture

### Core Classes

1. **`tile`** - Represents a single cell in the maze
   - Stores neighbors (North, South, East, West)
   - Tracks path states (open/closed) for each direction
   - Contains coordinate information (x, y)

2. **`TileList`** - Doubly-linked list for storing tiles
   - Used for: all tiles, visited tiles during traversal, shortest path
   - Provides efficient lookup and traversal operations

3. **`maze`** - Main maze generator
   - Initializes the grid structure
   - Generates maze using spanning tree algorithm
   - Finds and preserves shortest path
   - Applies randomization while protecting the solution path

## 🔧 Algorithm

### Generation Process

1. **Tile Initialization** (Recursive)
   - Creates all tiles in the grid
   - Establishes neighbor relationships

2. **Spanning Tree Creation** (DFS)
   - Uses recursive depth-first search
   - Randomly selects neighbor order
   - Opens paths to create a perfect maze (no loops)

3. **Shortest Path Calculation** (BFS)
   - Finds optimal path from start (0,0) to random end tile
   - Uses breadth-first search for shortest distance

4. **Randomization** (90% Preservation)
   - Randomly opens/closes 10% of paths
   - **Protects shortest path tiles** - never modified
   - **Protects connections to path tiles** - preserved
   - Maintains spanning tree structure (~90% intact)

5. **Path Recalculation** (BFS)
   - Finds new shortest path after randomization
   - Ensures solution still exists

## 📊 Sample Maze

```
===== Maze ASCII Visualization =====
Legend: S=Start, E=End, *=Path, Walls=lines

+---+---+---+---+---+---+---+---+---+---+
| S   *   * |                   |       |
+---+---+   +---+---+---+---+   +---+   +
|       | *   *   *   * |       |       |
+   +   +---+---+---+   +   +---+---+   +
|   |       |       | * |               |
+   +   +   +   +   +   +---+---+---+   +
|       |   |   |   | * | *   *   * |   |
+   +   +   +   +---+   +   +---+   +   +
|   |               | * | * |   | * |   |
+   +   +   +   +   +   +   +   +   +   +
|       |       |   | * | * | *   * |   |
+   +---+   +   +   +   +   +   +---+   +
|   |               | *   * | * |       |
+   +   +---+---+   +---+---+   +   +   +
|   |   |   |       | *   *   * |       |
+   +---+---+---+---+   +---+---+---+---+
|   |   |   |     E | *   * |   |   |   |
+   +---+---+---+   +---+   +---+   +   +
|       |       | *   *   *         |   |
+---+---+---+---+---+---+---+---+---+---+
====================================


===== Shortest Path =====
Path length: 31 tiles
Path: (0,0) -> (1,0) -> (2,0) -> (2,1) -> (3,1) ->
      (4,1) -> (5,1) -> (5,2) -> (5,3) -> (5,4) ->
      (5,5) -> (5,6) -> (6,6) -> (6,5) -> (6,4) ->
      (6,3) -> (7,3) -> (8,3) -> (8,4) -> (8,5) ->
      (7,5) -> (7,6) -> (7,7) -> (6,7) -> (5,7) ->
      (5,8) -> (6,8) -> (6,9) -> (5,9) -> (4,9) ->
      (4,8)
========================
```

## 🎮 Usage

### Compilation

```bash
g++ -std=c++11 -o MazeGen MazeGen.cpp
```

### Execution

```bash
./MazeGen
```

### Customization

Change maze dimensions in `main()`:

```cpp
int main() {
    maze myMaze(20, 20);  // Create a 20x20 maze instead of 10x10
    myMaze.printMazeASCII();
    myMaze.displayShortestPath();
    myMaze.validateMaze();
    return 0;
}
```

Adjust randomization probability in `generateMazePaths()` method (default is 0.90 for 90% preservation):

```cpp
void generateMazePaths() {
    // ...existing code...
    
    // Change 0.90 to 0.85 for 15% randomization instead of 10%
    addRandomExtraPaths(0.85);
    
    // ...existing code...
}
```

## 📈 Features

- ✅ **Guaranteed Solution** - Always has a valid path from start to end
- ✅ **Configurable Size** - Any width × height dimensions
- ✅ **Random Generation** - Different maze each run
- ✅ **Shortest Path Preserved** - Solution path never broken
- ✅ **ASCII Visualization** - Clear visual representation
- ✅ **Validation System** - Verifies maze integrity

## 🔍 Key Innovations

1. **Path Protection System**
   - Shortest path tiles are completely skipped during randomization
   - Connections to path tiles are never modified
   - Ensures solution remains valid after all modifications

2. **Spanning Tree Base**
   - Creates perfect maze foundation (no loops, all connected)
   - Provides baseline complexity
   - ~90% structure preserved after randomization

3. **Dual BFS Approach**
   - Initial pathfinding on spanning tree
   - Recalculation after randomization
   - Handles cycles introduced by random paths

## 📝 Code Structure

```
MazeGen.cpp
├── tile class           (Maze cell representation)
├── TileList class       (Linked list container)
└── maze class
    ├── Initialization   (Grid setup)
    ├── Generation       (Spanning tree DFS)
    ├── Pathfinding      (BFS algorithm)
    ├── Randomization    (90% preservation)
    └── Visualization    (ASCII output)
```

## 🐛 Known Limitations

- Recursive tile initialization may cause stack overflow on very large mazes (>100×100)
- ASCII visualization only practical for small-to-medium mazes (<50×50)
- Random end tile may be placed at start location (probability 1/n² for n×n maze)
- Uses `rand()` which is not cryptographically secure

## 🚀 Future Enhancements

- [ ] Iterative tile initialization for larger mazes
- [ ] GUI visualization using graphics library
- [ ] Multiple difficulty levels (vary randomization %)
- [ ] Different maze algorithms (Prim's, Kruskal's, etc.)
- [ ] Save/load maze to file
- [ ] Solver animation

## 📄 License

This project is open source and available for educational purposes.

## 👤 Author

Created as a graph algorithms and data structures learning project.

---

**Note**: The maze generator uses `rand()` seeded with `time(0)`, so running multiple times within the same second may produce identical mazes. For production use or better randomness, consider using C++11's `<random>` library with `std::mt19937` and `std::random_device`.
