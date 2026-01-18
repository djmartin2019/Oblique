// -----------------------------------------------------------------------------
// player.h
//
// Player-specific input handling and behavior.
// This module handles:
//
// - Mouse click input for player movement
// - Screen-to-isometric coordinate conversion
// - Pathfinding integration for player movement
// - Tile selection and highlighting
//
// The player input system integrates with the entity system: when the player
// clicks on a tile, it converts screen coordinates to isometric tile coordinates,
// selects the tile for highlighting, and creates a pathfinding path for the
// player entity to follow.
//
// Design goals:
// - Clean separation of input handling from entity logic
// - Reuse of entity movement system (no special player movement code)
// - Consistent behavior between player and NPC movement
// -----------------------------------------------------------------------------

#ifndef PLAYER_H
#define PLAYER_H

#include "render/camera.h"
#include "entity/entity.h"

#include <SDL2/SDL.h>

// -----------------------------------------------------------------------------
// Legacy Types
// -----------------------------------------------------------------------------

// Legacy Player struct (may be unused - check if still needed).
// Most player functionality now uses the Entity system directly.
typedef struct {
    int x, y;
    SDL_Texture* sprite;
} Player;

// -----------------------------------------------------------------------------
// Player Input
// -----------------------------------------------------------------------------

// Handles mouse click input for player movement.
//
// This function processes left mouse button clicks and converts them into
// player movement commands. When the player clicks on a tile:
//
// 1. Screen coordinates are converted to isometric tile coordinates
// 2. The clicked tile is selected and highlighted (red overlay)
// 3. If the tile is walkable, a pathfinding path is created
// 4. The path is assigned to the player entity for movement
//
// Movement process:
// - The entity system handles actual movement using the assigned path
// - Movement is tile-by-tile with smooth interpolation
// - If a path already exists, it is freed before creating a new one
//
// Args:
//   entity: Pointer to the player entity (should have is_player flag set)
//   event: SDL event containing mouse click information
//
// The function performs:
// - Screen coordinate to isometric tile coordinate conversion
// - Bounds checking (ensures tile is within map)
// - Tile walkability checking (only moves to walkable tiles)
// - Pathfinding path creation and assignment
//
// Tile selection:
// - The clicked tile is always selected (highlighted) regardless of walkability
// - This provides visual feedback even if movement is not possible
// - Only walkable tiles result in movement commands
//
// Pathfinding:
// - Uses find_path() to create a path from current position to clicked tile
// - If pathfinding fails or tile is unwalkable, no movement occurs
// - The path is assigned to entity->path for processing by update_entity_movement()
//
// This function should be called from the main event loop when SDL_MOUSEBUTTONDOWN
// events are detected. It integrates with the entity system and does not
// perform direct position updates.
void handle_player_input(Entity* entity, SDL_Event* event);

// -----------------------------------------------------------------------------
// Legacy Functions (may be unused)
// -----------------------------------------------------------------------------

// Initializes a Player struct (legacy function, may be unused).
//
// This function initializes the legacy Player struct with a starting position
// and loads the player sprite texture.
//
// Args:
//   player: Pointer to Player struct to initialize
//   renderer: SDL renderer for texture creation
//
// Returns:
//   1 on success, 0 on failure (sprite loading failed)
//
// Note: This function may be unused as the entity system now handles player
// initialization via add_entity(). Check if this is still needed.
int init_player(Player* player, SDL_Renderer* renderer);

// Draws a Player struct (legacy function, may be unused).
//
// This function renders a Player struct using the legacy drawing system.
// It calculates isometric screen position and draws the player sprite.
//
// Args:
//   player: Pointer to Player struct to draw
//   renderer: SDL renderer to draw with
//   cam: Camera structure containing current camera offset
//
// Note: This function may be unused as the entity system now handles player
// rendering via draw_entities(). Check if this is still needed.
void draw_player(Player* player, SDL_Renderer* renderer, Camera* cam);

#endif  // PLAYER_H
