#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include "map.h"
#include "render.h"
#include "camera.h"
#include "entity.h"

int main(int argc, char*argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    int success = 0;

    int player_index = -1;

    // Initalize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_ERROR: %s\n", SDL_GetError());
        goto cleanup;
    }

    // Initialize SDL_Image for PNG loading
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "SDL_image could not initialize PNG support! IMG_ERROR: %s\n", IMG_GetError());
        goto cleanup;
    }

    // Create game window
    window = SDL_CreateWindow("Oblique Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Window could not be created! SDL_ERROR: %s\n", SDL_GetError());
        goto cleanup;
    }

    // Create renderering context
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Renderer could not be created! SDL_ERROR: %s\n", SDL_GetError());
        goto cleanup;
    }

    // Load the map file
    if (!load_map("maps/test_map.txt")) {
        fprintf(stderr, "Failed to load map file.\n");
        goto cleanup;
    }

    // Load tiles textures
    if (!load_tile_textures(renderer)) {
        fprintf(stderr, "Failed to load tile textures.\n");
        goto cleanup;
    }

    // Initialize entity system
    init_entities();
    calculate_map_offset();

    // Load player sprite
    SDL_Surface* surface = IMG_Load("assets/sprites/player.png");
    if (!surface) {
        printf("Failed to load player sprite: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Texture* player_texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // Add player to entity list
    player_index = add_entity(
            5,5,            // position
            player_texture, // texture
            32, 64,         // dimensions
            -16, -48,       // offset for centering
            1               // is_player
    );

    // Add test NPC
    SDL_Surface* npc_surf = IMG_Load("assets/sprites/npc.png");
    SDL_Texture* npc_tex  = SDL_CreateTextureFromSurface(renderer, npc_surf);
    SDL_FreeSurface(npc_surf);

    add_entity(6, 5, npc_tex, 32, 64, -16, -48, 0);
    add_entity(7, 4, npc_tex, 32, 64, -16, -48, 0);
    add_entity(4, 6, npc_tex, 32, 64, -16, -48, 0);

    // Main game loop
    int running = 1;
    SDL_Event e;

    while (running) {
        const Uint8* keystates = SDL_GetKeyboardState(NULL);

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
        }

        // Basit player movement via arrow keys
        Entity* player = &entities[player_index];
        if (keystates[SDL_SCANCODE_UP])     player->y -= 1;
        if (keystates[SDL_SCANCODE_DOWN])   player->y += 1;
        if (keystates[SDL_SCANCODE_LEFT])   player->x -= 1;
        if (keystates[SDL_SCANCODE_RIGHT])  player->x += 1;

        // Update camera to follow player
        Camera camera;
        update_camera(&camera, player->x, player->y);

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw world and entities
        draw_map(renderer, &camera);
        draw_entities(renderer, &camera);
        calculate_map_offset();

        // Present the final frame
        SDL_RenderPresent(renderer);

        // Simple frame delay
        SDL_Delay(100);
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
