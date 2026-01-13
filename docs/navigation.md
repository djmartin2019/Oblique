# Navigation System

The navigation system provides grid overlay visualization, tile selection, and coordinate conversion between screen space and tile space.

## Grid Overlay

The grid system provides visual feedback for tile selection and movement planning.

### Drawing the Grid

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

### Selected Tile Highlight

The selected tile is drawn with a red fill, rendered before entities so the player appears on top:

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

## Coordinate Conversion

### Screen to Tile Coordinates

Converts screen coordinates (mouse click position) to tile coordinates, accounting for camera offset:

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

## Movement Grid

The movement grid calculates valid tiles the player can move to based on their current position and action points (or movement range).

```c
void calculate_move_grid(int start_x, int start_y, int max_cost) {
    // Uses BFS to find all reachable tiles within max_cost
    // Marks tiles as valid in move_tiles[y][x].valid
}
```

This is used to highlight valid movement destinations and restrict player movement.

## Input Handling

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

## Future Improvements

- Pathfinding system (A* algorithm)
- Step-based movement animation
- Movement range visualization
- Obstacle avoidance
- Multi-tile entity support
