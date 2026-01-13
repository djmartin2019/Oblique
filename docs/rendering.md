# Rendering System

The rendering system handles isometric projection, camera management, and the rendering pipeline.

## Coordinate System

The engine uses an isometric coordinate system where tile coordinates are converted to diamond-shaped screen positions.

### Coordinate Flow

To draw anything on screen:

```
World (tile) position
  → Isometric projection
    → Apply map offset (to center map)
      → Subtract camera position (to simulate camera scroll)
        = Final screen position
```

> `screen = isometric(world) + map_offset - camera`

### Isometric Projection

Tile coordinates `(x, y)` are converted to screen coordinates using:

```c
int screen_x = (x - y) * (TILE_WIDTH / 2) - cam->x + map_offset_x;
int screen_y = (x + y) * (TILE_HEIGHT / 2) - cam->y + map_offset_y;
```

## Camera System

The camera follows the player, keeping them centered on screen. The camera doesn't move the player—it moves the *world* to make it look like the player is staying still.

### Map Offset

Centers the map when the game boots:

```c
int center_tile_x = MAP_WIDTH / 2;
int center_tile_y = MAP_HEIGHT / 2;

int map_center_x = (center_tile_x - center_tile_y) * (TILE_WIDTH / 2);
int map_center_y = (center_tile_x + center_tile_y) * (TILE_HEIGHT / 2);

map_offset_x = (WINDOW_WIDTH / 2) - map_center_x;
map_offset_y = (WINDOW_HEIGHT / 2) - map_center_y;
```

This ensures the *middle tile* lands dead center of the window, not the top-left.

### Camera Update

Centers the camera on the player every frame:

```c
int iso_x = (player_x - player_y) * (TILE_WIDTH / 2) + map_offset_x;
int iso_y = (player_x + player_y) * (TILE_HEIGHT / 2) + map_offset_y;

cam->x = iso_x - screen_center_x;
cam->y = iso_y - screen_center_y;
```

## Rendering Order

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

## Entity Rendering

Entities are drawn at their tile position with pixel offsets for alignment:

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

## Future Improvements

- Depth sorting for entities (render back-to-front based on Y position)
- Sprite batching for performance
- Lighting and shadow systems
- Particle effects
