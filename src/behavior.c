#include <stdlib.h>
#include <SDL2/SDL.h>
#include "entity.h"
#include "behavior.h"

// Player behavior
const Uint8* keystates = NULL;

void set_player_input(const Uint8* state) {
    keystates = state;
}

void player_behavior(Entity* self) {
    if (!keystates) return;

    if (keystates[SDL_SCANCODE_UP])     self->y -= 1;
    if (keystates[SDL_SCANCODE_DOWN])   self->y += 1;
    if (keystates[SDL_SCANCODE_LEFT])   self->x -= 1;
    if (keystates[SDL_SCANCODE_RIGHT])  self->x += 1;
}

// NPC behavior
void wander_behavior(Entity* self) {
    if (rand() % 100 < 2) {
        int dir = rand() % 4;
        if (dir == 0) self->x += 1;
        if (dir == 1) self->x -= 1;
        if (dir == 2) self->y += 1;
        if (dir == 3) self->y -= 1;
    }
}
