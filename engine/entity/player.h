#ifndef PLAYER_H
#define PLAYER_H

#include "render/camera.h"
#include "entity/entity.h"

#include <SDL2/SDL.h>

typedef struct {
    int x, y; // grid position
    SDL_Texture* sprite;
} Player;

int init_player(Player* player, SDL_Renderer* renderer);
void handle_player_input(Entity* entity, SDL_Event* event);
void draw_player(Player* player, SDL_Renderer* renderer, struct Camera* cam);

#endif
