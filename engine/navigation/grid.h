#ifndef NAVIGATION_GRID_H
#define NAVIGATION_GRID_H

#include "core/constants.h"
#include "core/map.h"
#include "render/camera.h"

#include <SDL2/SDL.h>

typedef struct {
    int x, y;
    int valid;      // Can be moved to
    int ap_cost;    // Cost to move here
} HighlightTile;

typedef struct {
    int x;
    int y;
    int selected; // 0 = not selected, 1 = selected
} GridSelection;

extern GridSelection selected_tile;

// Public 2D array storing valid movement tiles
extern HighlightTile move_tiles[MAP_WIDTH][MAP_HEIGHT];

// Compute reachable tiles from a starting point (with AP or range limit)
void calculate_move_grid(int start_x, int start_y, int max_cost);

// Clear all move tiles (reset state)
void clear_move_grid(void);

// Render movement grid visually (outlines only, no selected highlight)
void draw_move_grid(SDL_Renderer* renderer, Camera* cam);

// Draw the selected tile highlight (call after drawing entities)
void draw_selected_tile(SDL_Renderer* renderer, Camera* cam);

void select_tile(int tile_x, int tile_y);

void screen_to_iso(int screen_x, int screen_y, Camera* cam, int* tile_x, int* tile_y);

#endif
