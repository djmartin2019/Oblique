#include "core/map.h"
#include "core/scene.h"
#include "render/render.h"
#include "render/camera.h"
#include "entity/entity.h"
#include "entity/player.h"
#include "ai/behavior.h"
#include "helpers/sdl_helpers.h"
#include "navigation/grid.h"

#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

void game_loop(SDL_Renderer* renderer) {
    // Main game loop
    int running = 1;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;

            // Feed input into player behavior system
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                Entity* player = get_player();
                Camera* cam = get_camera();
                if (player && cam) {
                    handle_player_input(player, &e, cam);
                }
            }
        }

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
