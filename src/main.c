#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include "map.h"
#include "render.h"

int main(int argc, char*argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    int success = 0;

    // Initalize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_ERROR: %s\n", SDL_GetError());
        goto cleanup;
    }

    // Initialize SDL_Image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "SDL_image could not initialize PNG support! IMG_ERROR: %s\n", IMG_GetError());
        goto cleanup;
    }

    // Create window
    window = SDL_CreateWindow("Oblique Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Window could not be created! SDL_ERROR: %s\n", SDL_GetError());
        goto cleanup;
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Renderer could not be created! SDL_ERROR: %s\n", SDL_GetError());
        goto cleanup;
    }

    // Load Map
    if (!load_map("maps/test_map.txt")) {
        fprintf(stderr, "Failed to load map file.\n");
        goto cleanup;
    }

    // Load tiles
    if (!load_tile_textures(renderer)) {
        fprintf(stderr, "Failed to load tile textures.\n");
        goto cleanup;
    }

    // Main loop
    int running = 1;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        draw_map(renderer);

        SDL_RenderPresent(renderer);
    }

    success = 1; // Program ran successfully

cleanup:
    // Clean up resources in reverse order of creation
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return success ? 0 : 1;
}
