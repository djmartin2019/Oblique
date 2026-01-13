#include "render/render.h"
#include "core/map.h"
#include "render/camera.h"
#include "core/constants.h"

#include <SDL2/SDL_image.h>

SDL_Texture* tile_textures[1]; // Only 1 tile for now

int load_tile_textures(SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(GRASS_TILE);
    if (!surface) {
        printf("Failed to load tile: %s\n", SDL_GetError());
        return 0;
    }
    tile_textures[0] = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return 1;
}

// Draws the map tiles in isometric space using camera + map offset
void draw_map(SDL_Renderer* renderer, Camera* cam) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int tile_id = tile_map[y][x]; // Get the tile type (e.g., grass, dirt, etc.)

            // Convert from tile corrdinates to screen position in isometric space
            // Formula transforms square grid to diamond layout
            int screen_x = (x - y) * (TILE_WIDTH / 2)
                - cam->x                            // Move relative to camera
                + map_offset_x;                     // Center the map at startup
            int screen_y = (x + y) * (TILE_HEIGHT / 2)
                - cam->y
                + map_offset_y;

            // Define where to draw this tile on screen
            SDL_Rect dest = { screen_x, screen_y, TILE_WIDTH, TILE_HEIGHT };
            SDL_RenderCopy(renderer, tile_textures[tile_id], NULL, &dest);
        }
    }
}
