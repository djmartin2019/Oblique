#include "entity/player.h"
#include "entity/entity.h"
#include "render/render.h" // for TILE_WIDTH and TILE_HEIGHT
#include "render/camera.h"
#include "core/constants.h"
#include "core/map.h"
#include "navigation/grid.h"

#include <SDL2/SDL_image.h>

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

void handle_player_input(Entity* entity, SDL_Event* event, Camera* cam) {
    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
        int mouse_x = event->button.x;
        int mouse_y = event->button.y;

        int tile_x, tile_y;
        screen_to_iso(mouse_x, mouse_y, cam, &tile_x, &tile_y);

        // Ensure tile is in bounds
        if (tile_x >= 0 && tile_x < MAP_WIDTH &&
            tile_y >= 0 && tile_y < MAP_HEIGHT) {

            if (move_tiles[tile_y][tile_x].valid) {
                select_tile(tile_x, tile_y);

                // Move the player instantly (you can replace with pathfinding later)
                entity->x = tile_x;
                entity->y = tile_y;
            }
        }
    }
}

// Draws the player relative to the camera + map offset, centered on their tile
void draw_player(Player* player, SDL_Renderer* renderer, Camera* cam) {
    // Get player's isometric screen position relative to the camera and map offset
    int screen_x = (player->x - player->y) * (TILE_WIDTH / 2) - cam->x + map_offset_x;
    int screen_y = (player->x + player->y) * (TILE_HEIGHT / 2) - cam->y + map_offset_y;

    // Adjust drawing rectangle to center the player sprite on the tile
    // Assumes the sprite is 32x64px, with the feet at the tile center
    SDL_Rect dest = {
        screen_x - 16,      // Shift left half the sprite's width
        screen_y - 48,      // Shift up so the feet sit on tile
        32,
        64
    };

    SDL_RenderCopy(renderer, player->sprite, NULL, &dest);
}
