// -----------------------------------------------------------------------------
// entity.h
//
// Entity Component System (ECS) for managing all game objects.
// This module handles:
//
// - Entity creation and initialization
// - Rendering of all entities (player, NPCs, props)
// - Movement system with pathfinding and interpolation
// - AI behavior integration
//
// The entity system consolidates players, NPCs, and props into a single
// unified system. All entities use the same movement, rendering, and update
// logic, ensuring consistency across the game.
//
// Design goals:
// - Unified system for all game objects
// - Smooth interpolation-based movement with pathfinding
// - Separation of logical position (x, y) and visual position (render_x, render_y)
// - Easy integration with AI behaviors
// -----------------------------------------------------------------------------

#ifndef ENTITY_H
#define ENTITY_H

#include "render/camera.h"
#include "ai/ai.h"
#include "navigation/pathfinding.h"

#include <SDL2/SDL.h>

// -----------------------------------------------------------------------------
// Constants
// -----------------------------------------------------------------------------

#define MAX_ENTITIES 128  // Maximum number of entities in the world
#define DEFAULT_AP_MAX 6  // Default action points per combat turn

// -----------------------------------------------------------------------------
// Types
// -----------------------------------------------------------------------------

// Forward declaration
typedef struct Entity Entity;

// Behavior function pointer type for AI behaviors.
//
// Behavior functions are called each frame to update entity state.
// They can modify entity position, state, or other properties.
//
// Args:
//   self: Pointer to the entity being updated
//
// Examples: player_behavior, wander_behavior, chase_behavior
typedef void (*BehaviorFunc)(Entity* self);

// Represents a single game object (player, NPC, prop, etc.).
//
// The Entity struct consolidates all game objects into a unified system.
// All entities share the same rendering, movement, and update logic.
//
// Position System:
// - x, y: Logical tile position (integer, updated when tile is reached)
// - render_x, render_y: Visual position (float, interpolated for smooth movement)
//
// Movement System:
// - path: Current pathfinding path (NULL if idle)
// - moving: Whether entity is currently interpolating between tiles
// - move_progress: Interpolation progress (0.0 -> 1.0)
// - from_x/y, to_x/y: Start and destination tiles for current movement
// - move_cooldown: Ticks remaining until next tile movement
// - move_delay: Ticks to wait between tile movements
//
// Rendering:
// - sprite: Main sprite texture
// - width, height: Sprite dimensions
// - offset_x, offset_y: Pixel offsets to align sprite feet with tile center
// - sprite_idle/wander/chase: Optional state-specific sprites for NPCs
//
// AI System:
// - behavior: Function pointer to behavior function (NULL if none)
// - state: Current AI state (for NPCs)
// - is_player: Flag to identify the player entity
//
// The entity system uses interpolation-based movement: entities smoothly
// slide between tiles instead of teleporting. The logical position (x, y)
// updates only when a tile is reached, while the visual position (render_x,
// render_y) interpolates between tiles for smooth animation.
struct Entity {
    // Logical tile position (integer, updated when tile is reached)
    int x, y;

    // Visual position for rendering (float, interpolated for smooth movement)
    float render_x;
    float render_y;

    // Movement interpolation state
    float move_progress;    // 0.0 -> 1.0, progress between from and to positions
    int moving;             // 1 if currently interpolating, 0 otherwise
    int from_x, from_y;     // Starting tile for current movement
    int to_x, to_y;         // Destination tile for current movement

    // Sprite rendering
    SDL_Texture* sprite;    // Main sprite texture
    int width, height;      // Sprite dimensions
    int offset_x, offset_y; // Pixel offsets to align sprite feet with tile center

    // Entity identification
    int is_player;          // 1 if this is the player, 0 otherwise

    // AI behavior system
    BehaviorFunc behavior;  // Function pointer to behavior function
    AIState state;          // Current AI state (for NPCs)

    // Optional state-specific sprites for NPCs
    SDL_Texture* sprite_idle;
    SDL_Texture* sprite_wander;
    SDL_Texture* sprite_chase;

    // Pathfinding and movement
    Path* path;             // Current pathfinding path (NULL if idle)
    int move_cooldown;      // Ticks remaining until next tile movement
    int move_delay;         // Ticks to wait between tile movements (speed control)

    // Combat action points (used only in combat)
    int ap_max;             // Maximum AP per turn
    int ap_current;         // Remaining AP this turn
};

// -----------------------------------------------------------------------------
// Global State
// -----------------------------------------------------------------------------

// Array of all entities in the world.
// Entities are stored in a fixed-size array for simplicity.
extern Entity entities[MAX_ENTITIES];

// Current number of active entities in the entities array.
// This is incremented by add_entity() and used for iteration.
extern int entity_count;

// -----------------------------------------------------------------------------
// Entity Management
// -----------------------------------------------------------------------------

// Initializes the entity system, resetting the entity count.
//
// This should be called at the start of each scene to ensure a clean state.
// Existing entities are not freed (they're overwritten when new ones are added).
void init_entities();

// Creates a new entity and adds it to the entity system.
//
// This function allocates a new entity in the entities array and initializes
// all its fields with the provided values. The entity is assigned the given
// position, sprite, and behavior.
//
// Args:
//   x: Initial tile X coordinate
//   y: Initial tile Y coordinate
//   sprite: SDL texture to render for this entity
//   width: Sprite width in pixels
//   height: Sprite height in pixels
//   offset_x: Horizontal pixel offset for sprite alignment
//   offset_y: Vertical pixel offset for sprite alignment (typically negative
//             to align sprite feet with tile center)
//   is_player: 1 if this is the player entity, 0 otherwise
//   behavior: Function pointer to behavior function (NULL if none)
//
// Returns:
//   The index of the newly created entity in the entities array on success.
//   -1 if the entity array is full (MAX_ENTITIES reached).
//
// The entity is initialized with:
// - render_x, render_y set to the initial position (for smooth rendering)
// - movement fields initialized to idle state
// - path set to NULL (no movement)
// - move_delay set to 6 ticks (default movement speed)
//
// Ownership:
// - The sprite texture is NOT copied or freed by this function
// - The caller retains ownership of the sprite texture
int add_entity(
    int x,
    int y,
    SDL_Texture* sprite,
    int width,
    int height,
    int offset_x,
    int offset_y,
    int is_player,
    BehaviorFunc behavior
);

// -----------------------------------------------------------------------------
// Entity Rendering
// -----------------------------------------------------------------------------

// Renders all entities to the screen using isometric projection.
//
// This function iterates through all active entities and draws them using
// the same coordinate transformation as tiles and grid, ensuring perfect
// alignment. Entities are drawn with their interpolated render positions
// for smooth movement animation.
//
// Args:
//   renderer: SDL renderer to draw with
//   cam: Camera structure containing current camera offset
//
// Rendering process:
// 1. Calculate screen position using isometric projection with render_x/render_y
// 2. Apply sprite offsets to align sprite feet with tile center
// 3. Apply color tint based on AI state (for NPCs)
// 4. Render sprite to screen
//
// Sprite alignment:
// - offset_x, offset_y align the sprite's feet with the tile center
// - For a 32x64 sprite on a 64x32 tile: offset_x=16, offset_y=-48
//
// Color tinting:
// - Player: Always full color (white)
// - NPCs: Tinted based on AI state (gray=idle, green=wander, red=chase)
//
// Entities are rendered after the grid to appear on top of the grid overlay.
void draw_entities(SDL_Renderer* renderer, Camera* cam);

// -----------------------------------------------------------------------------
// Entity Updates
// -----------------------------------------------------------------------------

// Updates all entities for one frame.
//
// This function processes all active entities in the following order:
// 1. AI brain update (for NPCs only) - determines state transitions
// 2. Behavior function execution - handles entity-specific logic
// 3. Movement system update - processes pathfinding and interpolation
//
// The update order ensures that:
// - AI state is determined before behavior functions run
// - Behavior functions can set paths or modify state
// - Movement system processes paths and interpolates positions
//
// This should be called once per frame in the game loop.
void update_entities();

// Updates a single entity's movement system.
//
// This function processes the entity's pathfinding path and handles
// smooth interpolation-based movement between tiles. The movement system:
//
// 1. Skips the first path node if it matches the current position
// 2. Checks if the path is complete (cleans up if so)
// 3. If moving: advances interpolation, updates render position
// 4. If not moving: starts movement to next tile in path (if cooldown expired)
//
// Movement process:
// - Each tile movement is interpolated over multiple frames
// - render_x/render_y smoothly transition from from_x/y to to_x/y
// - When move_progress reaches 1.0, logical position (x, y) is updated
// - Cooldown prevents entities from moving too fast
//
// Args:
//   e: Entity to update (can be NULL, in which case function returns early)
//
// Path handling:
// - If path is NULL, the entity is idle (no movement)
// - If path->current >= path->length, the path is complete (freed)
// - First path node is skipped if it matches current position (path includes start)
//
// Interpolation:
// - move_progress increases by MOVE_PROGRESS per frame (typically 0.2)
// - Takes ~5 frames per tile at 100ms delay = ~500ms per tile
// - Visual position interpolates while logical position stays on current tile
//
// This function should be called once per frame for each entity via update_entities().
// It can also be called individually if per-entity control is needed.
void update_entity_movement(Entity* e);

// -----------------------------------------------------------------------------
// Entity Queries
// -----------------------------------------------------------------------------

// Returns a pointer to the player entity, if one exists.
//
// This function searches through all active entities and returns the first
// one with is_player flag set to 1.
//
// Returns:
//   Pointer to the player entity on success.
//   NULL if no player entity exists in the entities array.
//
// The returned pointer is valid until the entity is removed or the entity
// array is reinitialized. It points directly into the entities array.
Entity* get_player();

#endif  // ENTITY_H
