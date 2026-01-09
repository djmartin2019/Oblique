#ifndef SDL_HELPERS_H
#define SDL_HELPERS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

int init_sdl(SDL_Window** window, SDL_Renderer** renderer);
void shutdown_sdl(SDL_Window* window, SDL_Renderer* renderer);

#endif
