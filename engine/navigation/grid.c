// Implementation file for grid.h
// See grid.h for detailed documentation.

#include "navigation/grid.h"
#include "core/map.h"
#include "core/constants.h"
#include "core/tile.h"

#include <string.h>
#include <SDL2/SDL.h>

// -----------------------------------------------------------------------------
// Global State
// -----------------------------------------------------------------------------

HighlightTile move_tiles[MAP_WIDTH][MAP_HEIGHT];
GridSelection selected_tile = { -1, -1, 0 };

// -----------------------------------------------------------------------------
// Internal Types and Constants
// -----------------------------------------------------------------------------

typedef struct {
    int x, y, cost;
} Node;

#define MAX_QUEUE 2048

// -----------------------------------------------------------------------------
// Movement Grid Calculation
// -----------------------------------------------------------------------------

void clear_move_grid(void) {
    memset(move_tiles, 0, sizeof(move_tiles));
}

void calculate_move_grid(int start_x, int start_y, int max_cost) {
    clear_move_grid();

    Node queue[MAX_QUEUE];
    int head = 0, tail = 0;

    queue[tail++] = (Node){start_x, start_y, 0};

    while (head < tail) {
        Node current = queue[head++];

        if (current.x < 0 || current.x >= MAP_WIDTH ||
            current.y < 0 || current.y >= MAP_HEIGHT) {
            continue;
        }

        HighlightTile* tile = &move_tiles[current.y][current.x];

        if (tile->valid && current.cost >= tile->ap_cost) {
            continue;
        }

        if (current.cost > max_cost) {
            continue;
        }

        tile->x = current.x;
        tile->y = current.y;
        tile->ap_cost = current.cost;
        tile->valid = 1;

        // Explore 4-directional neighbors
        queue[tail++] = (Node){current.x + 1, current.y, current.cost + 1};
        queue[tail++] = (Node){current.x - 1, current.y, current.cost + 1};
        queue[tail++] = (Node){current.x, current.y + 1, current.cost + 1};
        queue[tail++] = (Node){current.x, current.y - 1, current.cost + 1};
    }
}

// -----------------------------------------------------------------------------
// Tile Selection
// -----------------------------------------------------------------------------

void select_tile(int tile_x, int tile_y) {
    selected_tile.x = tile_x;
    selected_tile.y = tile_y;
    selected_tile.selected = 1;
}

// -----------------------------------------------------------------------------
// Rendering Helpers
// -----------------------------------------------------------------------------

static void draw_iso_tile_outline(SDL_Renderer* renderer, int screen_x, int screen_y, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    SDL_Point diamond[5] = {
        { screen_x + TILE_WIDTH / 2, screen_y },                    // Top
        { screen_x + TILE_WIDTH,     screen_y + TILE_HEIGHT / 2 },  // Right
        { screen_x + TILE_WIDTH / 2, screen_y + TILE_HEIGHT },      // Bottom
        { screen_x,                  screen_y + TILE_HEIGHT / 2 },  // Left
        { screen_x + TILE_WIDTH / 2, screen_y }                     // Close polygon
    };

    SDL_RenderDrawLines(renderer, diamond, 5);
}

static void fill_iso_tile(SDL_Renderer* renderer, int screen_x, int screen_y, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    int cx = screen_x + TILE_WIDTH / 2;
    int cy = screen_y + TILE_HEIGHT / 2;

    for (int dy = -TILE_HEIGHT/2; dy <= TILE_HEIGHT/2; dy++) {
        // Calculate scanline width based on distance from center (diamond shape)
        int span = (TILE_WIDTH/2) - abs(dy * (TILE_WIDTH / TILE_HEIGHT));
        SDL_RenderDrawLine(renderer, cx - span, cy + dy, cx + span, cy + dy);
    }
}

// -----------------------------------------------------------------------------
// Grid Rendering
// -----------------------------------------------------------------------------

void draw_move_grid(SDL_Renderer* renderer, Camera* cam) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int screen_x = (x - y) * (TILE_WIDTH / 2) - cam->x + map_offset_x;
            int screen_y = (x + y) * (TILE_HEIGHT / 2) - cam->y + map_offset_y;

            SDL_Color white = {255, 255, 255, 80};
            draw_iso_tile_outline(renderer, screen_x, screen_y, white);

            if (selected_tile.selected && selected_tile.x == x && selected_tile.y == y) {
                SDL_Color red = {255, 0, 0, 120};
                fill_iso_tile(renderer, screen_x, screen_y, red);
            }
        }
    }
}

// -----------------------------------------------------------------------------
// Coordinate Conversion
// -----------------------------------------------------------------------------

void screen_to_iso(int screen_x, int screen_y, Camera* cam, int* tile_x, int* tile_y) {
    // Reverse camera and map offset to get world coordinates
    float world_x = (float)(screen_x + cam->x - map_offset_x);
    float world_y = (float)(screen_y + cam->y - map_offset_y);

    // Reverse isometric projection
    // x - y = world_x / (TILE_WIDTH / 2)
    // x + y = world_y / (TILE_HEIGHT / 2)
    // Solving: x = ((x+y) + (x-y)) / 2, y = ((x+y) - (x-y)) / 2
    float half_tile_w = (float)(TILE_WIDTH / 2);
    float half_tile_h = (float)(TILE_HEIGHT / 2);

    float x_minus_y = world_x / half_tile_w;
    float x_plus_y = world_y / half_tile_h;

    *tile_x = (int)((x_plus_y + x_minus_y) / 2.0f + 0.5f);
    *tile_y = (int)((x_plus_y - x_minus_y) / 2.0f + 0.5f);
}

// -----------------------------------------------------------------------------
// Utility Functions
// -----------------------------------------------------------------------------

int is_tile_in_bounds(int x, int y) {
    return (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT);
}

