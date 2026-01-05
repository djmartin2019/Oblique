#include "player.h"
#include "render.h" // for TILE_WIDTH and TILE_HEIGHT
#include <SDL2/SDL_image.h>

#define MAP_WIDTH 10
#define MAP_HEIGHT 10

int init_player(Player* player, SDL_Renderer* renderer) {
    player->x = 5;
    player->y = 5;

    SDL_Surface* surface = IMG_Load("assets/sprites/player.png");
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

void draw_player(Player* player, SDL_Renderer* renderer) {
    int screen_x = (player->x - player->y) * (TILE_WIDTH / 2) + 400;
    int screen_y = (player->x + player->y) * (TILE_HEIGHT / 2) + 50;

    // Adjust so the sprite is centered over the tile
    SDL_Rect dest = {
        screen_x - 16,      // 32px wide sprite
        screen_y - 48,      // 64px tall sprite, bottom on tile
        32,
        64
    };

    SDL_RenderCopy(renderer, player->sprite, NULL, &dest);
}
