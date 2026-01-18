#include <stdlib.h>
#include <SDL2/SDL.h>

#include "entity/entity.h"
#include "ai/behavior.h"
#include "ai/ai.h"
#include "render/render.h"
#include "navigation/pathfinding.h"

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
    // Only pick a new destination if we don't have a path
    if (self->path) return;
    
    if (rand() % 100 < 2) {
        // Pick a random direction
        int dir = rand() % 4;
        int target_x = self->x;
        int target_y = self->y;
        
        if (dir == 0) target_x += 1;
        else if (dir == 1) target_x -= 1;
        else if (dir == 2) target_y += 1;
        else if (dir == 3) target_y -= 1;
        
        // Find path to the target
        Path* path = find_path(self->x, self->y, target_x, target_y);
        if (path && path->length > 0) {
            // Free any existing path
            if (self->path) {
                free_path(self->path);
            }
            self->path = path;
            self->path->current = 0;
            self->moving = 0;
            self->move_progress = 0.0f;
        } else if (path) {
            free_path(path);
        }
    }
}

void chase_behavior(Entity* self) {
    Entity* player = get_player();

    if (!player) return; // Safety check

    // Only update path if we don't have one or we've reached the end
    if (self->path && self->path->current < self->path->length) {
        return; // Still following current path
    }

    if (++chase_timer % 10 != 0) return; // Only recalculate path every 10 ticks

    // Find path to player
    Path* path = find_path(self->x, self->y, player->x, player->y);
    if (path && path->length > 0) {
        // Free any existing path
        if (self->path) {
            free_path(self->path);
        }
        self->path = path;
        self->path->current = 0;
        self->moving = 0;
        self->move_progress = 0.0f;
    } else if (path) {
        free_path(path);
    }
}

void idle_behavior(Entity* self) {
    // do nothing for now
}
