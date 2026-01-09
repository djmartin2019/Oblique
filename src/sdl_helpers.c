#include "sdl_helpers.h"
#include <stdio.h>

int init_sdl(SDL_Window** window, SDL_Renderer** renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_ERROR: %s\n", SDL_GetError());
        return 0;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "SDL_image could not initialize PNG support! IMG_ERROR: %s\n", IMG_GetError());
        return 0;
    }

    *window = SDL_CreateWindow("Oblique Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    if (!*window) {
        fprintf(stderr, "Window could not be created! SDL_ERROR: %s\n", SDL_GetError());
        return 0;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (!*renderer) {
        fprintf(stderr, "Renderer could not be created! SDL_ERROR: %s\n", SDL_GetError());
        return 0;
    }

    return 1;
}

void shutdown_sdl(SDL_Window* window, SDL_Renderer* renderer) {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}
