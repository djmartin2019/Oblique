// -----------------------------------------------------------------------------
// grid.h
//
// Grid visualization and interaction system for the isometric map.
// This module handles:
//
// - Visual grid overlay rendering (white outlines, red highlights)
// - Tile selection and highlighting
// - Screen-to-isometric coordinate conversion (for mouse clicks)
// - Movement grid calculation (BFS-based reachability)
// - Walkability checks for pathfinding integration
//
// The grid system provides visual feedback for player interaction and
// movement planning, similar to classic tactics RPGs.
//
// Design goals:
// - Clear visual feedback for tile selection
// - Accurate coordinate conversion accounting for camera and map offsets
// - Efficient movement grid calculation using breadth-first search
// - Integration with pathfinding system for walkability checks
// -----------------------------------------------------------------------------

#ifndef NAVIGATION_GRID_H
#define NAVIGATION_GRID_H

#include "core/constants.h"
#include "core/map.h"
#include "render/camera.h"

#include <SDL2/SDL.h>

// -----------------------------------------------------------------------------
// Types
// -----------------------------------------------------------------------------

// Represents a single tile in the movement grid with its validity and cost.
//
// The movement grid is calculated by calculate_move_grid() to determine
// which tiles are reachable from a starting position within a given cost.
//
// Fields:
//   x, y: Tile coordinates in the map grid
//   valid: 1 if the tile is reachable, 0 otherwise
//   ap_cost: Movement cost (AP cost) to reach this tile from the start
typedef struct {
    int x, y;
    int valid;      // Can be moved to
    int ap_cost;    // Cost to move here
} HighlightTile;

// Represents the currently selected tile for visual highlighting.
//
// The selected tile is displayed with a red fill overlay in draw_move_grid()
// to provide visual feedback to the player.
//
// Fields:
//   x, y: Tile coordinates of the selected tile
//   selected: 1 if a tile is selected, 0 otherwise
typedef struct {
    int x;
    int y;
    int selected; // 0 = not selected, 1 = selected
} GridSelection;

// -----------------------------------------------------------------------------
// Global State
// -----------------------------------------------------------------------------

// Currently selected tile for highlighting.
// Set by select_tile() and used by draw_move_grid() for red highlight.
extern GridSelection selected_tile;

// 2D array storing valid movement tiles and their costs.
// Used by the movement grid calculation and pathfinding system.
//
// This array is populated by calculate_move_grid() and can be queried
// by other systems to determine valid movement targets.
extern HighlightTile move_tiles[MAP_WIDTH][MAP_HEIGHT];

// -----------------------------------------------------------------------------
// Movement Grid Calculation
// -----------------------------------------------------------------------------

// Clears all movement tiles, resetting the grid to an empty state.
//
// This should be called before calculate_move_grid() to ensure a clean slate.
// All tiles are marked as invalid (not reachable).
//
// Typically called internally by calculate_move_grid(), but exposed for
// manual grid management if needed.
void clear_move_grid(void);

// Calculates all reachable tiles from a starting position using breadth-first search.
//
// This function performs a BFS traversal from (start_x, start_y) to find all
// tiles that can be reached within max_cost moves. Each tile is marked as valid
// and stores its movement cost (AP cost).
//
// The algorithm:
// 1. Start with the initial position at cost 0
// 2. For each tile, explore its 4 neighbors (cardinal directions)
// 3. Mark neighbors as valid if they're walkable and within max_cost
// 4. Continue until no more reachable tiles are found
//
// Args:
//   start_x: Starting tile X coordinate
//   start_y: Starting tile Y coordinate
//   max_cost: Maximum movement cost (AP cost) allowed
//
// The results are stored in the global move_tiles array, which can be queried
// by other systems (e.g., pathfinding, UI) to determine valid movement targets.
//
// The algorithm uses only cardinal directions (no diagonals), matching the
// pathfinding system's movement constraints.
void calculate_move_grid(int start_x, int start_y, int max_cost);

// -----------------------------------------------------------------------------
// Tile Selection
// -----------------------------------------------------------------------------

// Marks a tile as selected for visual highlighting.
//
// When a tile is selected, it will be rendered with a red fill overlay
// in draw_move_grid() to provide visual feedback to the player.
//
// Args:
//   tile_x: X coordinate of the tile to select
//   tile_y: Y coordinate of the tile to select
//
// The selected tile persists until select_tile() is called again with
// different coordinates. The selection state is stored in the global
// selected_tile variable.
void select_tile(int tile_x, int tile_y);

// -----------------------------------------------------------------------------
// Grid Rendering
// -----------------------------------------------------------------------------

// Renders the movement grid overlay on the map.
//
// This function draws the visual grid that overlays the isometric map, providing
// visual feedback for tile boundaries and selection. Each tile is drawn with:
// - A white outline (diamond shape) for all tiles
// - A red fill for the currently selected tile (if any)
//
// Args:
//   renderer: SDL renderer to draw with
//   cam: Camera structure containing current camera offset
//
// The grid is drawn using the same coordinate transformation as the map tiles,
// ensuring perfect alignment. The selected tile highlight is drawn on top of
// the white grid outline.
//
// Rendering order:
// 1. White grid outlines (all tiles)
// 2. Red fill for selected tile (if selected)
//
// This should be called after drawing the map but before drawing entities,
// so entities appear on top of the grid.
void draw_move_grid(SDL_Renderer* renderer, Camera* cam);

// -----------------------------------------------------------------------------
// Coordinate Conversion
// -----------------------------------------------------------------------------

// Converts screen coordinates (mouse position) to isometric tile coordinates.
//
// This function performs the inverse of the isometric projection used for
// rendering. It accounts for:
// - Camera offset (to convert screen space to world space)
// - Map offset (to account for map centering)
// - Isometric projection reversal (to convert world space to tile coordinates)
//
// Args:
//   screen_x: X coordinate in screen space (pixels from left edge)
//   screen_y: Y coordinate in screen space (pixels from top edge)
//   cam: Camera structure containing current camera offset
//   tile_x: Output parameter for the resulting tile X coordinate
//   tile_y: Output parameter for the resulting tile Y coordinate
//
// The conversion process:
// 1. Reverse camera and map offset to get world coordinates
// 2. Reverse the isometric projection to get tile coordinates
//
// Forward projection (rendering):
//   screen_x = (x - y) * (TILE_WIDTH / 2) - cam->x + map_offset_x
//   screen_y = (x + y) * (TILE_HEIGHT / 2) - cam->y + map_offset_y
//
// Reverse projection (this function):
//   world_x = screen_x + cam->x - map_offset_x
//   world_y = screen_y + cam->y - map_offset_y
//   x = (world_y / (TILE_HEIGHT / 2) + world_x / (TILE_WIDTH / 2)) / 2
//   y = (world_y / (TILE_HEIGHT / 2) - world_x / (TILE_WIDTH / 2)) / 2
//
// Uses floating-point arithmetic for accuracy, then rounds to nearest integer.
//
// This function is used for mouse click detection to determine which tile
// was clicked by the player.
void screen_to_iso(int screen_x, int screen_y, Camera* cam, int* tile_x, int* tile_y);

// -----------------------------------------------------------------------------
// Utility Functions
// -----------------------------------------------------------------------------

// Checks if a tile coordinate is within the map bounds.
//
// Args:
//   x: Tile X coordinate to check
//   y: Tile Y coordinate to check
//
// Returns:
//   1 if the tile is within bounds (0 <= x < MAP_WIDTH, 0 <= y < MAP_HEIGHT)
//   0 otherwise
//
// This is a utility function used by pathfinding and other systems to validate
// tile coordinates before accessing map data.
int is_tile_in_bounds(int x, int y);

// Checks if a tile is walkable (can be moved onto).
//
// This function combines bounds checking with map walkability checks.
// A tile is walkable if:
// 1. It is within map bounds
// 2. The map data indicates it's walkable (tile_map[y][x] == 0)
//
// Args:
//   x: Tile X coordinate to check
//   y: Tile Y coordinate to check
//
// Returns:
//   1 if the tile is walkable
//   0 if the tile is out of bounds or blocked
//
// Walkability convention:
// - 0 = walkable (ground/empty)
// - Non-zero = blocked (wall/obstacle)
//
// This function is used by the pathfinding system to determine valid movement.
int is_tile_walkable(int x, int y);

#endif  // NAVIGATION_GRID_H
