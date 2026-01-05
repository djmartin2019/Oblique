#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>

#define TILE_WIDTH 64
#define TILE_HEIGHT 32

int load_tile_textures(SDL_Renderer* renderer);
void draw_map(SDL_Renderer* renderer);

#endif
