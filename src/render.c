#include "render.h"
#include "map.h"
#include <SDL2/SDL_image.h>

#define TILE_SIZE 64

SDL_Texture* tile_textures[1]; // Only 1 tile for now

int load_tile_textures(SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load("assets/tiles/grass.png");
    if (!surface) {
        printf("Failed to load tile: %s\n", SDL_GetError());
        return 0;
    }
    tile_textures[0] = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return 1;
}

void draw_map(SDL_Renderer* renderer) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int tile_id = tile_map[y][x];
            SDL_Rect dest = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
            SDL_RenderCopy(renderer, tile_textures[tile_id], NULL, &dest);
        }
    }
}
