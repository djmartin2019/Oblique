# Oblique Engine - Camera System Overview

This document explains how the isometric camera system in the Oblique Engine works, in plain English, for Future David (who has likely forgotten how anything works).

---

## 🤖 Core Concepts

The engine simulates an old-school isometric view, like Fallout 1/2 or tactics RPGs. The main systems involved:

* **Tile grid** — The game world is a 2D grid of tiles.
* **Isometric projection** — Grid coordinates are converted to diamond-shaped screen positions.
* **Camera** — Moves around the world to keep the player centered.
* **Map offset** — Centers the map when the game starts.
* **Entity system** — Tracks and renders all game objects (player, NPCs, props)

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

This ensures the *middle tile* lands dead center of the window, not the top-left.

---

## 🔸 `update_camera()`

Centers the camera on the player every frame.

```c
int iso_x = (player_x - player_y) * (TILE_WIDTH / 2) + map_offset_x;
int iso_y = (player_x + player_y) * (TILE_HEIGHT / 2) + map_offset_y;

cam->x = iso_x - screen_center_x;
cam->y = iso_y - screen_center_y;
```

The camera doesn’t move the player. It moves the *world* to make it look like the player is staying still.

---

## 🗺️ `draw_map()`

Draws the map by projecting every tile into screen space using isometric math.

```c
int screen_x = (x - y) * (TILE_WIDTH / 2) - cam->x + map_offset_x;
int screen_y = (x + y) * (TILE_HEIGHT / 2) - cam->y + map_offset_y;
```

Each tile is drawn in order, using its `tile_id` to grab the texture.

---

## 🧍‍♂️ `draw_player()`

Draws the player at their tile, centered visually using an offset.

```c
int screen_x = (player->x - player->y) * (TILE_WIDTH / 2) - cam->x + map_offset_x;
int screen_y = (player->x + player->y) * (TILE_HEIGHT / 2) - cam->y + map_offset_y;

SDL_Rect dest = {
    screen_x - 16,      // 32px wide sprite
    screen_y - 48,      // 64px tall sprite, feet on tile
    32,
    64
};
```

---

## 👥 Entity System Overview

### `Entity` Struct:

```c
typedef struct {
    int x, y;               // Logical tile position
    float render_x, render_y; // Visual position (for interpolation)
    
    // Movement interpolation
    float move_progress;    // 0.0 -> 1.0
    int moving;
    int from_x, from_y;    // Starting tile for current movement
    int to_x, to_y;         // Destination tile for current movement
    
    // Pathfinding
    Path* path;             // Current path (NULL if idle)
    int move_cooldown;      // Ticks until next tile move
    int move_delay;         // How many ticks between moves
    
    SDL_Texture* sprite;
    int width, height;
    int offset_x, offset_y; // Visual offsets for sprite alignment
    int is_player;
    BehaviorFunc behavior;  // AI behavior function
} Entity;
```

### Purpose:

* Consolidates players, NPCs, and props into a single system
* Supports pathfinding and smooth interpolation-based movement
* Allows iteration, update, and rendering in one place
* AI behaviors use the same movement system as the player

### Drawing Entities:

```c
void draw_entities(SDL_Renderer* renderer, Camera* cam) {
    for (int i = 0; i < entity_count; i++) {
        Entity* e = &entities[i];
        
        // Use interpolated render position for smooth movement
        float rx = e->render_x;
        float ry = e->render_y;
        
        // Same formula as tiles/grid for perfect alignment
        int screen_x = (rx - ry) * (TILE_WIDTH / 2) - cam->x + map_offset_x;
        int screen_y = (rx + ry) * (TILE_HEIGHT / 2) - cam->y + map_offset_y;
        
        // Apply sprite offsets to align feet with tile center
        screen_x += e->offset_x;
        screen_y += e->offset_y;
        
        SDL_Rect dest = { screen_x, screen_y, e->width, e->height };
        SDL_RenderCopy(renderer, e->sprite, NULL, &dest);
    }
}
```

---

## 🧭 Pathfinding System

### A* Algorithm

The engine uses a grid-based A* pathfinding algorithm for navigation:

```c
Path* find_path(int start_x, int start_y, int goal_x, int goal_y);
void free_path(Path* path);
```

**Features:**
- 4-directional movement (cardinal directions only)
- Uniform tile cost (1 per tile)
- Walkability checks via `is_tile_walkable()`
- Path includes start node for proper movement handling

**Path Structure:**
```c
typedef struct {
    PathNode* nodes;  // Array of tile coordinates
    int length;       // Number of nodes in path
    int current;      // Current position in path
} Path;
```

### Walkability

Tiles are walkable if `tile_map[y][x] == 0` (convention: 0 = walkable, non-zero = blocked).

---

## 🚶 Movement System

### Interpolation-Based Movement

Entities move tile-by-tile with smooth interpolation between tiles:

1. **Path Assignment**: When a path is assigned, `path->current = 0`
2. **Movement Start**: Entity stores `from_x/from_y` (current tile) and `to_x/to_y` (next tile)
3. **Interpolation**: Each frame, `move_progress` increases (0.0 → 1.0)
4. **Visual Position**: `render_x/render_y` interpolate between `from` and `to` positions
5. **Tile Arrival**: When `move_progress >= 1.0`, logical position updates and path advances

**Movement Update:**
```c
void update_entity_movement(Entity* e) {
    if (!e || !e->path) return;
    
    // Skip first node if it matches current position
    if (e->path->current < e->path->length) {
        PathNode first = e->path->nodes[e->path->current];
        if (first.x == e->x && first.y == e->y) {
            e->path->current++;
        }
    }
    
    // If moving, interpolate
    if (e->moving) {
        e->move_progress += MOVE_PROGRESS; // 0.2 per frame
        if (e->move_progress >= 1.0f) {
            // Arrived at tile, update logical position
            e->x = e->to_x;
            e->y = e->to_y;
            e->render_x = (float)e->x;
            e->render_y = (float)e->y;
            e->path->current++;
            e->moving = 0;
        } else {
            // Interpolate visual position
            float t = e->move_progress;
            e->render_x = e->from_x + (e->to_x - e->from_x) * t;
            e->render_y = e->from_y + (e->to_y - e->from_y) * t;
        }
        return;
    }
    
    // Start next movement if cooldown expired
    if (e->move_cooldown > 0) {
        e->move_cooldown--;
        return;
    }
    
    // Begin movement to next tile in path
    PathNode next = e->path->nodes[e->path->current];
    e->from_x = (float)e->x;
    e->from_y = (float)e->y;
    e->to_x = next.x;
    e->to_y = next.y;
    e->moving = 1;
    e->move_progress = 0.0f;
}
```

**Movement Speed:**
- `MOVE_PROGRESS = 0.2f` (takes ~5 frames per tile at 100ms delay = 500ms per tile)
- `move_delay` controls cooldown between tiles (default: 6 ticks for player, 10 for NPCs)

---

## ⚔️ Combat-Only AP Movement

Movement is **free outside of combat** and **AP-gated during combat**:

- **Explore mode**: entities follow paths without AP/turn limits.
- **Combat mode**: each entity spends **1 AP per tile**; turns rotate once AP hits 0.
- **Combat trigger**: combat starts when an NPC enters `STATE_COMBAT` (e.g., close range or attack) and ends once no NPCs remain in combat.

AP is only consumed when a tile step begins. If AP hits 0, movement pauses until the entity’s next turn.

---

## 🤖 AI Behaviors with Pathfinding

NPCs use the same pathfinding system as the player:

### Wander Behavior

```c
void wander_behavior(Entity* self) {
    if (self->path) return; // Already moving
    
    if (rand() % 100 < 2) { // 2% chance per frame
        int dir = rand() % 4;
        int target_x = self->x + (dir == 0 ? 1 : dir == 1 ? -1 : 0);
        int target_y = self->y + (dir == 2 ? 1 : dir == 3 ? -1 : 0);
        
        Path* path = find_path(self->x, self->y, target_x, target_y);
        if (path && path->length > 0) {
            self->path = path;
            self->path->current = 0;
        }
    }
}
```

### Chase Behavior

```c
void chase_behavior(Entity* self) {
    Entity* player = get_player();
    if (!player || (self->path && self->path->current < self->path->length)) {
        return; // No player or still following path
    }
    
    if (++chase_timer % 10 != 0) return; // Recalculate every 10 ticks
    
    Path* path = find_path(self->x, self->y, player->x, player->y);
    if (path && path->length > 0) {
        if (self->path) free_path(self->path);
        self->path = path;
        self->path->current = 0;
    }
}
```

---

## 🎨 Rendering System

### Entity Alignment

Entities are rendered using the same coordinate formula as tiles/grid to ensure perfect alignment:

```c
// Calculate tile position in isometric space
int screen_x = (rx - ry) * (TILE_WIDTH / 2) - cam->x + map_offset_x;
int screen_y = (rx + ry) * (TILE_HEIGHT / 2) - cam->y + map_offset_y;

// Apply offsets to align sprite feet with tile center
screen_x += e->offset_x;  // Typically 16 (centers 32px sprite on 64px tile)
screen_y += e->offset_y; // Typically -48 (aligns 64px sprite feet with tile center)
```

**Offset Calculation:**
- `offset_x = TILE_WIDTH/2 - sprite_width/2` (centers horizontally)
- `offset_y = TILE_HEIGHT/2 - sprite_height` (aligns feet with tile center)

For a 32x64 sprite on a 64x32 tile:
- `offset_x = 32 - 16 = 16`
- `offset_y = 16 - 64 = -48`

---

## 💡 Debug Tips

* If the player floats weirdly, your sprite offset is probably wrong.
* If the player and tile locations don't line up, your map_offset is wrong.
* If the player is centered but the world drifts, your camera math is cursed.
* If entities don't move, check that `path` is assigned and `update_entity_movement()` is called.
* If pathfinding returns NULL, verify tiles are walkable (0 = walkable, non-zero = blocked).
* If movement is jittery, check `MOVE_PROGRESS` value and frame timing.
* If entities render off-grid, verify `render_x/render_y` use same formula as tiles.
* If everything is broken, stop hardcoding +400 and +50 everywhere.

---

## 🧠 Remember

> World + offset - camera = screen position

The camera doesn't move the player. The world moves *around* the player.

**Movement Flow:**
1. Click tile → `find_path()` → assign to `entity->path`
2. `update_entity_movement()` processes path tile-by-tile
3. `render_x/render_y` interpolate between tiles for smooth visual movement
4. Logical `x/y` update when tile is reached

**Key Concepts:**
- `x, y` = logical tile position (integer)
- `render_x, render_y` = visual position (float, for interpolation)
- Path includes start node, so skip it if it matches current position
- All entities (player + NPCs) use the same movement system

Yes, Unity does all this for you. But now *you* are Unity. Sleep well.
