#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include "entity.h"

void wander_behavior(Entity* self);
void player_behavior(Entity* self);
void set_player_input(const Uint8* state);

void npc_brain(Entity* self);
void idle_behavior(Entity* self);
void chase_behavior(Entity* self);

#endif
