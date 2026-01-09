#include "entity/player.h"
#include "render/render.h" // for TILE_WIDTH and TILE_HEIGHT
#include "render/camera.h"
#include "core/constants.h"
#include "core/map.h"

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

void handle_player_input(Player* player, const Uint8* keystates) {
    // Movement keys (diagonal-friendly layout)
    if (keystates[SDL_SCANCODE_UP]) {
        if (player->y > 0) player->y -= 1;
    }
    if (keystates[SDL_SCANCODE_DOWN]) {
        if (player->y < MAP_HEIGHT - 1) player->y += 1;
    }
    if (keystates[SDL_SCANCODE_LEFT]) {
        if (player->x > 0) player->x -= 1;
    }
    if (keystates[SDL_SCANCODE_RIGHT]) {
        if (player->x < MAP_WIDTH - 1) player->x += 1;
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
