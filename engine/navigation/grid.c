#include "navigation/grid.h"
#include "core/map.h"
#include "core/constants.h"

#include <string.h>     // for memset
#include <SDL2/SDL.h>


HighlightTile move_tiles[MAP_WIDTH][MAP_HEIGHT];

typedef struct {
    int x, y, cost;
} Node;

#define MAX_QUEUE 2048

GridSelection selected_tile = { -1, -1, 0 };

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

        if (current.x < 0 || current.x >= MAP_WIDTH || current.y < 0 || current.y >= MAP_HEIGHT)
            continue;

        HighlightTile* tile = &move_tiles[current.y][current.x];

        if (tile->valid && current.cost >= tile->ap_cost)
            continue;

        if (current.cost > max_cost)
            continue;

        tile->x = current.x;
        tile->y = current.y;
        tile->ap_cost = current.cost;
        tile->valid = 1;

        // Add neighboring tiles
        queue[tail++] = (Node){current.x + 1, current.y, current.cost + 1};
        queue[tail++] = (Node){current.x - 1, current.y, current.cost + 1};
        queue[tail++] = (Node){current.x, current.y + 1, current.cost + 1};
        queue[tail++] = (Node){current.x, current.y - 1, current.cost + 1};
    }
}

void select_tile(int tile_x, int tile_y) {
    selected_tile.x = tile_x;
    selected_tile.y = tile_y;
    selected_tile.selected = 1;
}

static void draw_iso_tile_outline(SDL_Renderer* renderer, int screen_x, int screen_y, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    SDL_Point diamond[5] = {
        { screen_x + TILE_WIDTH / 2, screen_y },                    // Top
        { screen_x + TILE_WIDTH,     screen_y + TILE_HEIGHT / 2 },  // Right
        { screen_x + TILE_WIDTH / 2, screen_y + TILE_HEIGHT },      // Bottom
        { screen_x,                  screen_y + TILE_HEIGHT / 2 },  // Left
        { screen_x + TILE_WIDTH / 2, screen_y }                     // Back to top
    };

    SDL_RenderDrawLines(renderer, diamond, 5);
}

static void fill_iso_tile(SDL_Renderer* renderer, int screen_x, int screen_y, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    int cx = screen_x + TILE_WIDTH / 2;
    int cy = screen_y + TILE_HEIGHT / 2;

    for (int dy = -TILE_HEIGHT/2; dy <= TILE_HEIGHT/2; dy++) {
        int span = (TILE_WIDTH/2) - abs(dy * (TILE_WIDTH / TILE_HEIGHT));
        SDL_RenderDrawLine(renderer,
            cx - span,
            cy + dy,
            cx + span,
            cy + dy
        );
    }
}

void draw_move_grid(SDL_Renderer* renderer, Camera* cam) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {

            // Convert tile coordinates to isometric screen coords
            int screen_x = (x - y) * (TILE_WIDTH / 2) - cam->x + map_offset_x;
            int screen_y = (x + y) * (TILE_HEIGHT / 2) - cam->y + map_offset_y;

           // White grid by default
           SDL_Color white = {255, 255, 255, 80};
           draw_iso_tile_outline(renderer, screen_x, screen_y, white);
        }
    }
}

void draw_selected_tile(SDL_Renderer* renderer, Camera* cam) {
    if (selected_tile.selected) {
        int x = selected_tile.x;
        int y = selected_tile.y;
        
        // Convert tile coordinates to isometric screen coords
        int screen_x = (x - y) * (TILE_WIDTH / 2) - cam->x + map_offset_x;
        int screen_y = (x + y) * (TILE_HEIGHT / 2) - cam->y + map_offset_y;

        // Red fill for selected tile
        SDL_Color red = {255, 0, 0, 120};
        fill_iso_tile(renderer, screen_x, screen_y, red);
    }
}

// Converts screen coordinates to isometric tile coordinates
// Accounts for camera offset and map offset
void screen_to_iso(int screen_x, int screen_y, Camera* cam, int* tile_x, int* tile_y) {
    // Add camera offset back to get world coordinates
    // screen = (x-y)*TW/2 - cam + offset, so world = screen + cam - offset
    int world_x = screen_x + cam->x - map_offset_x;
    int world_y = screen_y + cam->y - map_offset_y;

    // Reverse the isometric projection:
    // world_x = (x - y) * (TILE_WIDTH / 2)
    // world_y = (x + y) * (TILE_HEIGHT / 2)
    // Solving for x and y:
    // x = (world_x/(TW/2) + world_y/(TH/2)) / 2
    // y = (world_y/(TH/2) - world_x/(TW/2)) / 2
    *tile_x = (world_x / (TILE_WIDTH / 2) + world_y / (TILE_HEIGHT / 2)) / 2;
    *tile_y = (world_y / (TILE_HEIGHT / 2) - world_x / (TILE_WIDTH / 2)) / 2;
}

