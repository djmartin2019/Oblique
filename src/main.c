#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include "map.h"
#include "render.h"
#include "camera.h"
#include "entity.h"
#include "behavior.h"
#include "scene.h"
#include "sdl_helpers.h"

void game_loop(SDL_Renderer* renderer) {
    // Main game loop
    int running = 1;
    SDL_Event e;

    while (running) {
        const Uint8* keystates = SDL_GetKeyboardState(NULL);

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
        }

        // Feed input into player behavior system
        set_player_input(keystates);

        // Let all entities update (including player AI or input)
        update_scene();

        // Clear screen first
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw world and entities
        render_scene(renderer);

        // Present the final frame
        SDL_RenderPresent(renderer);

        // Simple frame delay
        SDL_Delay(100);
    }
}

int main(int argc, char*argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if (!init_sdl(&window, &renderer)) return 1;

    set_scene(SCENE_EXPLORE, renderer);

    game_loop(renderer);

    shutdown_sdl(window, renderer);
    return 0;
}
