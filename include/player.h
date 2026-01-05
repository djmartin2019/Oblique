#ifndef PLAYER_H
#define PLAYER_H

#include "camera.h"
#include <SDL2/SDL.h>

typedef struct {
    int x, y; // grid position
    SDL_Texture* sprite;
} Player;

int init_player(Player* player, SDL_Renderer* renderer);
void handle_player_input(Player* player, const Uint8* keystates);
void draw_player(Player* player, SDL_Renderer* renderer, struct Camera* cam);

#endif
