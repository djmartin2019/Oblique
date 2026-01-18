// -----------------------------------------------------------------------------
// pathfinding.h
//
// Grid-based pathfinding service used by both player and AI.
// This module is intentionally self-contained:
//
// - It knows about the grid (walkability, bounds)
// - It does NOT know about entities, rendering, camera, or input
// - It returns a Path (sequence of tiles) and nothing more
//
// The algorithm implemented here is a straightforward A* over a 2D grid
// with 4-directional movement and uniform tile cost.
//
// Design goals:
// - Correctness over cleverness
// - Explicit state (no hidden globals)
// - Easy to debug and reason about
// -----------------------------------------------------------------------------

#ifndef PATHFINDING_H
#define PATHFINDING_H

// -----------------------------------------------------------------------------
// Types
// -----------------------------------------------------------------------------

// Represents a single tile coordinate in a path.
//
// Paths are sequences of tile coordinates that entities follow for movement.
// Each PathNode represents one step in the path from start to goal.
typedef struct {
    int x;
    int y;
} PathNode;

// Represents a complete path from start to goal.
//
// The path is stored as an array of PathNode structures, ordered from
// start to goal. The path includes the starting position to ensure proper
// movement handling when the entity is already at the start tile.
//
// Fields:
//   nodes: Array of tile coordinates forming the path (start -> goal)
//   length: Number of nodes in the path
//   current: Current position in the path (for step-by-step movement)
//
// Ownership:
// - Paths are allocated by find_path() using malloc()
// - The caller is responsible for freeing the path with free_path()
// - Paths must be freed to avoid memory leaks
//
// Usage:
// - Assign a path to an entity's path field for movement
// - The movement system advances path->current as the entity moves
// - When path->current >= path->length, the path is complete
typedef struct {
    PathNode* nodes;
    int length;
    int current;
} Path;

// -----------------------------------------------------------------------------
// Public API
// -----------------------------------------------------------------------------

// Attempts to find a path from (start_x, start_y) to (goal_x, goal_y) using A*.
//
// This function performs a complete A* search over the grid to find an optimal
// path from the start position to the goal. The algorithm uses:
// - Manhattan distance heuristic (appropriate for 4-directional movement)
// - Uniform tile cost (1 per tile)
// - 4-directional movement only (cardinal directions, no diagonals)
//
// Algorithm overview:
// 1. Initialize all nodes with g_cost = INT_MAX (unvisited)
// 2. Start with initial position at g_cost = 0, calculate heuristic
// 3. Repeatedly select the open node with lowest f_cost (g + h)
// 4. Explore its 4 neighbors, updating costs if a better path is found
// 5. When goal is reached, reconstruct path by walking backward through parents
// 6. Reverse path so it's ordered start -> goal
//
// Args:
//   start_x: Starting tile X coordinate
//   start_y: Starting tile Y coordinate
//   goal_x: Goal tile X coordinate
//   goal_y: Goal tile Y coordinate
//
// Returns:
//   A newly allocated Path on success, containing the sequence of tiles
//   from start to goal (including the start tile). The caller is responsible
//   for freeing this path with free_path().
//
//   NULL if:
//   - No valid path exists (blocked by obstacles)
//   - Start or goal tile is not walkable
//   - Start equals goal (returns a single-node path that will be cleaned up)
//
// Performance:
// - Time complexity: O(n) where n is the number of reachable tiles
// - Uses linear scan for finding lowest f_cost node (O(n) per iteration)
// - For large maps, consider upgrading to a priority queue/heap
//
// This function performs NO movement. It only plans a route.
// The resulting path must be assigned to an entity and processed by the
// movement system for actual movement to occur.
Path* find_path(int start_x, int start_y, int goal_x, int goal_y);

// Frees a Path allocated by find_path().
//
// This function deallocates both the Path structure and its internal nodes
// array. It is safe to call with NULL (no-op).
//
// Args:
//   path: Path to free, or NULL
//
// Ownership rule:
// - Paths are allocated by find_path() using malloc()
// - The caller is responsible for freeing the path when done
// - Failure to call free_path() will result in memory leaks
// - After calling free_path(), the path pointer becomes invalid
//
// Should be called when:
// - An entity completes its movement along a path
// - A new path replaces an existing one
// - The entity is destroyed or no longer needs the path
void free_path(Path* path);

#endif  // PATHFINDING_H
