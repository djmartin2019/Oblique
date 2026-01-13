#ifndef COMBAT_H
#define COMBAT_H

#include <SDL2/SDL.h>

void combat_start(Entity* initiator, Entity* target);
void update_combat();       // Called every frame
void renderer_combat_ui(SDL_Renderer* renderer);
int is_combat_active();     // Returns 1 if we're in combat

#endif
