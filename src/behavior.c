#include <stdlib.h>
#include <SDL2/SDL.h>

#include "entity.h"
#include "behavior.h"
#include "ai.h"
#include "render.h"

static const Uint8* keystates = NULL;
static int chase_timer = 0;

// -----------------------------------------
// AI Transition Conditions (Stubs for now)
// -----------------------------------------

int should_wander() { return rand() % 200 == 0; }   // Wander occasionally

int sees_player(Entity* self) {
    for (int i = 0; i < entity_count; i++) {
        Entity* other = &entities[i];
        if (other->is_player) {
            int dx = abs(other->x - self->x);
            int dy = abs(other->y - self->y);
            return (dx + dy) <= 5;                  // Within distance of 5
        }
    }

    return 0;
}

int lost_player(Entity* self) {
    for (int i = 0; i < entity_count; i++) {
        Entity* other = &entities[i];
        if (other->is_player) {
            int dx = abs(other->x - self->x);
            int dy = abs(other->y - self->y);
            return (dx + dy) > 7;                   // Loses player when it's 7 or more tiles away
        }
    }
    return 0;
}

// -----------------------------------------
// NPC API Brain
// -----------------------------------------

void npc_brain(Entity* self) {
    switch (self->state) {
        case STATE_IDLE:
            if (should_wander()) {
                self->state = STATE_WANDER;
                self->behavior = wander_behavior;
            }
            break;
        case STATE_WANDER:
            if (sees_player(self)) {
                self->state = STATE_CHASE;
                self->behavior = chase_behavior;
            }
            break;
        case STATE_CHASE:
            if (lost_player(self)) {
                self->state = STATE_IDLE;
                self->behavior = idle_behavior;
            }
            break;
    }

    // Change sprite based on current state
    switch (self->state) {
        case STATE_IDLE:
            self->sprite = self->sprite_idle ? self->sprite_idle : self->sprite;
            break;
        case STATE_WANDER:
            self->sprite = self->sprite_wander ? self->sprite_wander : self->sprite;
            break;
        case STATE_CHASE:
            self->sprite = self->sprite_chase ? self->sprite_chase : self->sprite;
            break;
    }
}

// -----------------------------------------
// Player Behavior
// -----------------------------------------

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

// -----------------------------------------
// NPC behavior
// -----------------------------------------

void wander_behavior(Entity* self) {
    if (rand() % 100 < 2) {
        int dir = rand() % 4;
        if (dir == 0) self->x += 1;
        if (dir == 1) self->x -= 1;
        if (dir == 2) self->y += 1;
        if (dir == 3) self->y -= 1;
    }
}

void chase_behavior(Entity* self) {
    Entity* player = get_player();

    if (!player) return; // Safety check

    if (++chase_timer % 10 != 0) return; // Only moves every 10 ticks

    // Simple tile-by-tile movement towards player
    if (self->x < player->x) self->x++;
    else if (self->x > player->x) self->x--;

    if (self->y < player->y) self->y++;
    else if (self->y > player->y) self->y--;
}

void idle_behavior(Entity* self) {
    // do nothing for now
}
