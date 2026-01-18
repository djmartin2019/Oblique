// Implementation file for player.h
// See player.h for detailed documentation.

#include "entity/player.h"
#include "entity/entity.h"
#include "render/render.h"
#include "render/camera.h"
#include "core/constants.h"
#include "core/map.h"
#include "core/scene.h"
#include "navigation/grid.h"
#include "navigation/pathfinding.h"

#include <SDL2/SDL_image.h>

// -----------------------------------------------------------------------------
// Legacy Functions
// -----------------------------------------------------------------------------

int init_player(Player* player, SDL_Renderer* renderer) {
    player->x = 5;
    player->y = 5;

    SDL_Surface* surface = IMG_Load(PLAYER_SPRITE);
    if (!surface) {
        printf("Failed to load player sprite: %s\n", SDL_GetError());
        return 0;
    }

    player->sprite = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    return player->sprite != NULL;
}

void draw_player(Player* player, SDL_Renderer* renderer, Camera* cam) {
    int screen_x = (player->x - player->y) * (TILE_WIDTH / 2) - cam->x + map_offset_x;
    int screen_y = (player->x + player->y) * (TILE_HEIGHT / 2) - cam->y + map_offset_y;

    SDL_Rect dest = {
        screen_x - 16,
        screen_y - 48,
        32,
        64
    };

    SDL_RenderCopy(renderer, player->sprite, NULL, &dest);
}

// -----------------------------------------------------------------------------
// Player Input
// -----------------------------------------------------------------------------

void handle_player_input(Entity* entity, SDL_Event* event) {
    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
        int mouse_x = event->button.x;
        int mouse_y = event->button.y;

        Camera* cam = get_camera();
        if (!cam) return;

        int tile_x, tile_y;
        screen_to_iso(mouse_x, mouse_y, cam, &tile_x, &tile_y);

        if (tile_x >= 0 && tile_x < MAP_WIDTH &&
            tile_y >= 0 && tile_y < MAP_HEIGHT) {

            select_tile(tile_x, tile_y);

            if (move_tiles[tile_y][tile_x].valid) {
                if (entity->path) {
                    free_path(entity->path);
                    entity->path = NULL;
                }

                Path* path = find_path(entity->x, entity->y, tile_x, tile_y);
                
                if (path && path->length > 0) {
                    entity->path = path;
                    entity->path->current = 0;
                    entity->moving = 0;
                    entity->move_progress = 0.0f;
                } else if (path) {
                    free_path(path);
                }
            }
        }
    }
}
