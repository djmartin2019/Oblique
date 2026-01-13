# Oblique Engine - Technical Documentation

This document explains the technical systems in the Oblique Engine, in plain English, for Future David (who has likely forgotten how anything works).

---

## 🤖 Core Concepts

The engine simulates an old-school isometric view, like classic tactics RPGs. The main systems involved:

- **Tile grid** — The game world is a 2D grid of tiles.
- **Isometric projection** — Grid coordinates are converted to diamond-shaped screen positions.
- **Camera** — Moves around the world to keep the player centered.
- **Map offset** — Centers the map when the game starts.
- **Entity system** — Tracks and renders all game objects (player, NPCs, props)
- **Grid system** — Overlay grid with click-to-select and tile highlighting

---

## 🧮 Coordinate Flow

To draw anything on screen:

```
World (tile) position
  → Isometric projection
    → Apply map offset (to center map)
      → Subtract camera position (to simulate camera scroll)
        = Final screen position
```

> `screen = isometric(world) + map_offset - camera`

Simple. Not easy.

---

## 🔹 `calculate_map_offset()`

Centers the map when the game boots.

```c
int center_tile_x = MAP_WIDTH / 2;
int center_tile_y = MAP_HEIGHT / 2;

int map_center_x = (center_tile_x - center_tile_y) * (TILE_WIDTH / 2);
int map_center_y = (center_tile_x + center_tile_y) * (TILE_HEIGHT / 2);

map_offset_x = (WINDOW_WIDTH / 2) - map_center_x;
map_offset_y = (WINDOW_HEIGHT / 2) - map_center_y;
```

This ensures the _middle tile_ lands dead center of the window, not the top-left.

---

## 🔸 `update_camera()`

Centers the camera on the player every frame.

```c
int iso_x = (player_x - player_y) * (TILE_WIDTH / 2) + map_offset_x;
int iso_y = (player_x + player_y) * (TILE_HEIGHT / 2) + map_offset_y;

cam->x = iso_x - screen_center_x;
cam->y = iso_y - screen_center_y;
```

The camera doesn't move the player. It moves the _world_ to make it look like the player is staying still.

---

## 🗺️ `draw_map()`

Draws the map by projecting every tile into screen space using isometric math.

```c
int screen_x = (x - y) * (TILE_WIDTH / 2) - cam->x + map_offset_x;
int screen_y = (x + y) * (TILE_HEIGHT / 2) - cam->y + map_offset_y;
```

Each tile is drawn in order, using its `tile_id` to grab the texture.

---

## 🎯 Grid System

The grid system provides visual feedback for tile selection and movement planning.

### `draw_move_grid()`

Draws the grid overlay with white outlines for all tiles.

```c
void draw_move_grid(SDL_Renderer* renderer, Camera* cam) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int screen_x = (x - y) * (TILE_WIDTH / 2) - cam->x + map_offset_x;
            int screen_y = (x + y) * (TILE_HEIGHT / 2) - cam->y + map_offset_y;

            SDL_Color white = {255, 255, 255, 80};
            draw_iso_tile_outline(renderer, screen_x, screen_y, white);
        }
    }
}
```

### `draw_selected_tile()`

Draws a red highlight on the currently selected tile. Called after entities so the player appears on top.

```c
void draw_selected_tile(SDL_Renderer* renderer, Camera* cam) {
    if (selected_tile.selected) {
        int x = selected_tile.x;
        int y = selected_tile.y;

        int screen_x = (x - y) * (TILE_WIDTH / 2) - cam->x + map_offset_x;
        int screen_y = (x + y) * (TILE_HEIGHT / 2) - cam->y + map_offset_y;

        SDL_Color red = {255, 0, 0, 120};
        fill_iso_tile(renderer, screen_x, screen_y, red);
    }
}
```

### `screen_to_iso()`

Converts screen coordinates (mouse click position) to tile coordinates, accounting for camera offset.

```c
void screen_to_iso(int screen_x, int screen_y, Camera* cam, int* tile_x, int* tile_y) {
    // Add camera offset back to get world coordinates
    int world_x = screen_x + cam->x - map_offset_x;
    int world_y = screen_y + cam->y - map_offset_y;

    // Reverse the isometric projection:
    // world_x = (x - y) * (TILE_WIDTH / 2)
    // world_y = (x + y) * (TILE_HEIGHT / 2)
    // Solving for x and y:
    *tile_x = (world_x / (TILE_WIDTH / 2) + world_y / (TILE_HEIGHT / 2)) / 2;
    *tile_y = (world_y / (TILE_HEIGHT / 2) - world_x / (TILE_WIDTH / 2)) / 2;
}
```

**Important:** The camera offset must be added back before converting, otherwise clicks will be misaligned when the camera has moved.

---

## 🧍‍♂️ Entity System

### `Entity` Struct

```c
typedef struct {
    int x, y;               // Tile position
    SDL_Texture* sprite;    // Sprite texture
    int width, height;      // Sprite dimensions
    int offset_x, offset_y; // Pixel offsets for alignment
    int is_player;          // Flag to identify the player
    BehaviorFunc behavior;  // AI behavior function pointer
    AIState state;          // Current AI state
    int ap;                 // Action points (for combat)
} Entity;
```

### Drawing Entities

```c
void draw_entities(SDL_Renderer* renderer, Camera* cam) {
    for (int i = 0; i < entity_count; i++) {
        Entity* e = &entities[i];

        int screen_x = (e->x - e->y) * (TILE_WIDTH / 2) - cam->x + map_offset_x;
        int screen_y = (e->x + e->y) * (TILE_HEIGHT / 2) - cam->y + map_offset_y;

        SDL_Rect dest = {
            screen_x + e->offset_x,
            screen_y + e->offset_y,
            e->width,
            e->height
        };

        SDL_RenderCopy(renderer, e->sprite, NULL, &dest);
    }
}
```

### Player Positioning

The player sprite is positioned so its bottom (feet) aligns with the tile center:

```c
// Horizontal: center the sprite on the tile
offset_x = TILE_WIDTH / 2 - sprite_width / 2 = 32 - 16 = 16

// Vertical: place sprite bottom at tile center
offset_y = TILE_HEIGHT / 2 - sprite_height = 16 - 64 = -48
```

This ensures the player appears to be standing on the tile, not floating above or below it.

---

## 🎮 Input Handling

### Mouse Click to Tile Selection

Mouse clicks are handled in `handle_player_input()`:

```c
void handle_player_input(Entity* entity, SDL_Event* event, Camera* cam) {
    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
        int mouse_x = event->button.x;
        int mouse_y = event->button.y;

        int tile_x, tile_y;
        screen_to_iso(mouse_x, mouse_y, cam, &tile_x, &tile_y);

        // Ensure tile is in bounds and valid for movement
        if (tile_x >= 0 && tile_x < MAP_WIDTH &&
            tile_y >= 0 && tile_y < MAP_HEIGHT) {

            if (move_tiles[tile_y][tile_x].valid) {
                select_tile(tile_x, tile_y);
                entity->x = tile_x;
                entity->y = tile_y;
            }
        }
    }
}
```

**Important:** Events must be processed inside the `SDL_PollEvent` loop, not after it.

---

## 🎨 Rendering Order

The rendering order is critical for proper visual layering:

```c
void render_scene(SDL_Renderer* renderer) {
    calculate_map_offset();
    draw_map(renderer, &camera);            // 1. Draw map tiles (bottom layer)
    draw_move_grid(renderer, &camera);      // 2. Draw grid outlines
    draw_selected_tile(renderer, &camera);  // 3. Draw selected tile highlight
    draw_entities(renderer, &camera);       // 4. Draw player + NPCs (top layer)
}
```

This ensures:

- Map tiles are the base layer
- Grid outlines are visible but don't obscure entities
- Selected tile highlight appears under the player
- Entities (player, NPCs) appear on top

---

## 💡 Debug Tips

- **Player floats weirdly:** Check sprite offset values. Feet should be at tile center.
- **Player and tile locations don't line up:** Check `map_offset` calculation.
- **Player is centered but world drifts:** Check camera math in `update_camera()`.
- **Mouse clicks don't align with tiles:** Ensure `screen_to_iso()` accounts for camera offset.
- **Selected tile appears over player:** Check rendering order in `render_scene()`.
- **Everything is broken:** Stop hardcoding magic numbers everywhere.

---

## 🧠 Remember

> World + offset - camera = screen position

The camera doesn't move the player. The world moves _around_ the player.

When converting screen coordinates back to tile coordinates, always add the camera offset back first.

Yes, Unity does all this for you. But now _you_ are Unity. Sleep well.

---

## 📁 File Organization

### Navigation System (`engine/navigation/`)

- `grid.c/h` - Grid overlay rendering, tile selection, coordinate conversion
  - `draw_move_grid()` - Draws grid outlines
  - `draw_selected_tile()` - Draws selected tile highlight
  - `screen_to_iso()` - Converts screen coords to tile coords
  - `select_tile()` - Sets the currently selected tile
  - `calculate_move_grid()` - Calculates valid movement tiles from player position

### Entity System (`engine/entity/`)

- `entity.c/h` - Entity management and rendering
- `player.c/h` - Player-specific input handling and logic

### Core Systems (`engine/core/`)

- `scene.c/h` - Scene management and rendering order
- `camera.c/h` - Camera positioning and map offset
- `map.c/h` - Map loading and tile data

---

## 🔧 Recent Changes

### Grid System Implementation

- Added `navigation/grid.c/h` for grid overlay and tile selection
- Implemented `screen_to_iso()` with proper camera offset handling
- Added `draw_selected_tile()` for visual feedback
- Fixed rendering order so player appears above selected tile highlight
- Updated player positioning offsets to align feet with tile center

### Input System Updates

- Mouse click handling moved inside `SDL_PollEvent` loop
- `handle_player_input()` updated to work with Entity system
- Added camera parameter to coordinate conversion functions

---

## 🚧 Future Improvements

- Depth sorting for entities (render back-to-front based on Y position)
- Pathfinding system for movement
- Collision detection integration
- Combat system with action points
- Save/load system
- Scripting system for behaviors
