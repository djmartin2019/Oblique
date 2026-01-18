// -----------------------------------------------------------------------------
// pathfinding.c
//
// Grid-based pathfinding service used by both player and AI.
// This module is intentionally self-contained:
//
// - It knows about the grid (walkability, bounds)
// - Id does NOT know about entities, rendering, camera, or input
// - It returns a Path (sequence of tiles) and nothing more
//
// The algorithm  implemented here is a straightforward A* over a 2D grid
// with 4-directional movement and uniform tile cost.
//
// Design goals:
// - Correctness over cleverness
// - Explicit state (no hidden globals)
// - Easy to debug and reason about
// -----------------------------------------------------------------------------

#include "navigation/pathfinding.h"
#include "navigation/grid.h"
#include "core/constants.h"

#include <stdlib.h>
#include <limits.h>

#define MAX_PATH_LENGTH 256

// -----------------------------------------------------------------------------
// Internal node representation
//
// Each Node corresponds to a single tile in the map grid.
// These nodes exist ONLY during pathfinding and are discarded afterwards.
//
// Parent pointers are stored as coordinates (x, y) instead of pointers
// to avoid ownership issues and make reconstruction trivial.
// -----------------------------------------------------------------------------

typedef struct {
    int x, y;

    int g_cost;     // Cost from start
    int h_cost;     // Heuristic to goal
    int f_cost;     // g + h

    int parent_x;
    int parent_y;

    int in_open;
    int in_closed;
} Node;

// -----------------------------------------------------------------------------
// Manhattan distance heuristic
//
// This is appropriate because:
// - Movement is restricted to 4 directions
// - All tiles have uniform cost
//
// If diagonal movement or terrain costs are added later,
// this is one of the first functions that would need adjustment.
// -----------------------------------------------------------------------------

static int heuristic(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

// -----------------------------------------------------------------------------
// Convert (x, y) tile coordinates into a pointer within the flat node array.
//
// Nodes are stored in a single contiguous block for simplicity
// Access pattern: row-major order (y * width + x).
// -----------------------------------------------------------------------------

static Node* get_node(Node* nodes, int x, int y) {
    return &nodes[y * MAP_WIDTH + x];
}

// -----------------------------------------------------------------------------
// Free a Path returned by find_path.
//
// Ownership rule:
// - pathfinding allocates the Path
// - the caller is responsible for freeing it when done
// -----------------------------------------------------------------------------

void free_path(Path* path) {
    if (!path) return;
    free(path->nodes);
    free(path);
}

// -----------------------------------------------------------------------------
// Find the open node with the lowest f_cost.
//
// This is an O(n) scan over the grid, which is acceptable for now.
// If performance ever becomes an issue, this is where a priority queue
// or binary heap would be introduced.
// -----------------------------------------------------------------------------

static Node* find_lowest_f(Node* nodes) {
    Node* best = NULL;

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            Node* n = get_node(nodes, x, y);
            if (!n->in_open) continue;

            if (!best || n->f_cost < best->f_cost) {
                best = n;
            }
        }
    }

    return best;
}

// -----------------------------------------------------------------------------
// Reconstruct the final path once the goal node is reached.
//
// Walks backward from the goal node to the start using parent pointers,
// then reverses the result so the path is ordered start -> goal.
//
// Notes:
// - The starting tile is NOT included in the path
// - The goal tile IS included
// - Path length is capped by MAX_PATH_LENGTH
// -----------------------------------------------------------------------------

static Path* reconstruct_path(Node* nodes, Node* goal, int start_x, int start_y) {

    Path* path = malloc(sizeof(Path));
    if (!path) return NULL;

    path->nodes = malloc(sizeof(PathNode) * MAX_PATH_LENGTH);
    path->length = 0;
    path->current = 0;

    Node* current = goal;

    // Walk backwards through parent links (start node has parent_x == -1, so we stop before adding it)
    while (current->parent_x != -1) {
        path->nodes[path->length++] = (PathNode) {
            current->x,
            current->y
        };

        current = get_node(nodes, current->parent_x, current->parent_y);
    }

    // Always add the start node at the end (before reversing)
    // This ensures the path includes the starting position
    path->nodes[path->length++] = (PathNode) { start_x, start_y };

    // Reverse path so it runs from start -> goal
    for (int i = 0; i < path->length / 2; i++) {
        PathNode tmp = path->nodes[i];
        path->nodes[i] = path->nodes[path->length - 1 - i];
        path->nodes[path->length - 1 - i] = tmp;
    }

    return path;
}

// -----------------------------------------------------------------------------
// Public API: find_path
//
// Attempts to find a path from (start_x, start_y) to (goal_x, goal_y).
//
// Returns:
// - A newly allocated Path on success
// - NULL if no valid path exists
//
// This function performs NO movement.
// It only plans a route.
// -----------------------------------------------------------------------------

Path* find_path(int start_x, int start_y, int goal_x, int goal_y) {

    // Early out if start == goal (already there)
    if (start_x == goal_x && start_y == goal_y) {
        // Return a path with just the start node (will be cleaned up immediately)
        Path* path = malloc(sizeof(Path));
        if (!path) return NULL;
        path->nodes = malloc(sizeof(PathNode));
        path->nodes[0] = (PathNode) { start_x, start_y };
        path->length = 1;
        path->current = 0;
        return path;
    }

    // Check if tiles are walkable
    if (!is_tile_walkable(start_x, start_y)) {
        printf("Pathfinding: Start tile (%d,%d) is not walkable\n", start_x, start_y);
        return NULL;
    }
    
    if (!is_tile_walkable(goal_x, goal_y)) {
        printf("Pathfinding: Goal tile (%d,%d) is not walkable\n", goal_x, goal_y);
        return NULL;
    }

    // Allocate temporary node grid
    Node* nodes = calloc(MAP_WIDTH * MAP_HEIGHT, sizeof(Node));
    if (!nodes) return NULL;

    // Initialize nodes metadata
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            Node* n = get_node(nodes, x, y);
            n->x = x;
            n->y = y;
            n->g_cost = INT_MAX;
            n->parent_x = -1;
            n->parent_y = -1;
        }
    }

    // Initialize starting node
    Node* start = get_node(nodes, start_x, start_y);
    start->g_cost = 0;
    start->h_cost = heuristic(start_x, start_y, goal_x, goal_y);
    start->f_cost = start->h_cost;
    start->in_open = 1;

    // Main A* loop
    while (1) {
        Node* current = find_lowest_f(nodes);
        if (!current) break; // No more reachable nodes

        // Goal reached: reconstruct and return path
        if (current->x == goal_x && current->y == goal_y) {
            Path* path = reconstruct_path(nodes, current, start_x, start_y);
            free(nodes);
            return path;
        }

        current->in_open = 0;
        current->in_closed = 1;

        // Cardinal movement only (no diagonals)
        static int dirs[4][2] = {
            {  1,  0 },
            { -1,  0 },
            {  0,  1 },
            {  0, -1 }
        };

        for (int i = 0; i < 4; i++) {
            int nx = current->x + dirs[i][0];
            int ny = current->y + dirs[i][1];

            if (!is_tile_in_bounds(nx, ny)) continue;
            if (!is_tile_walkable(nx, ny)) continue;

            Node* neighbor = get_node(nodes, nx, ny);
            if (neighbor->in_closed) continue;

            int tentative_g = current->g_cost + 1;

            // If this path to neighbor is better, record it
            if (!neighbor->in_open || tentative_g < neighbor->g_cost) {
                neighbor->parent_x = current->x;
                neighbor->parent_y = current->y;
                neighbor->g_cost = tentative_g;
                neighbor->h_cost = heuristic(nx, ny, goal_x, goal_y);
                neighbor->f_cost = neighbor->g_cost + neighbor->h_cost;
                neighbor->in_open = 1;
            }
        }
    }

    // No path found
    printf("Pathfinding: A* algorithm exhausted all possibilities, no path found from (%d,%d) to (%d,%d)\n", 
           start_x, start_y, goal_x, goal_y);
    free(nodes);
    return NULL;
}
