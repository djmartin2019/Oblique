// Implementation file for entity.h
// See entity.h for detailed documentation.

#include "entity/entity.h"
#include "render/camera.h"
#include "render/render.h"
#include "ai/behavior.h"
#include "core/constants.h"

// -----------------------------------------------------------------------------
// Constants
// -----------------------------------------------------------------------------

#define MOVE_PROGRESS 0.2f

// -----------------------------------------------------------------------------
// Global State
// -----------------------------------------------------------------------------

Entity entities[MAX_ENTITIES];
int entity_count = 0;

// -----------------------------------------------------------------------------
// Entity Management
// -----------------------------------------------------------------------------

void init_entities() {
    entity_count = 0;
}

int add_entity(int x, int y, SDL_Texture* sprite, int width, int height, int offset_x, int offset_y, int is_player, BehaviorFunc behavior) {
    if (entity_count >= MAX_ENTITIES) return -1;

    Entity* e = &entities[entity_count++];
    e->x = x;
    e->y = y;
    e->render_x = (float)x;
    e->render_y = (float)y;
    e->sprite = sprite;
    e->width = width;
    e->height = height;
    e->offset_x = offset_x;
    e->offset_y = offset_y;
    e->is_player = is_player;
    e->behavior = behavior;
    
    e->move_progress = 0.0f;
    e->moving = 0;
    e->from_x = x;
    e->from_y = y;
    e->to_x = x;
    e->to_y = y;
    e->path = NULL;
    e->move_cooldown = 0;
    e->move_delay = 6;

    return entity_count - 1;
}

// -----------------------------------------------------------------------------
// Entity Rendering
// -----------------------------------------------------------------------------

void draw_entities(SDL_Renderer* renderer, Camera* cam) {
    for (int i = 0; i < entity_count; i++) {
        Entity* e = &entities[i];

        float rx = e->render_x;
        float ry = e->render_y;

        int screen_x = (rx - ry) * (TILE_WIDTH / 2) - cam->x + map_offset_x;
        int screen_y = (rx + ry) * (TILE_HEIGHT / 2) - cam->y + map_offset_y;
        
        screen_x += e->offset_x;
        screen_y += e->offset_y;

        SDL_Rect dest = {
            screen_x,
            screen_y,
            e->width,
            e->height
        };

        if (!e->is_player) {
            switch (e->state) {
                case STATE_IDLE:
                    SDL_SetTextureColorMod(e->sprite, 64, 64, 64);
                    break;
                case STATE_WANDER:
                    SDL_SetTextureColorMod(e->sprite, 0, 255, 0);
                    break;
                case STATE_CHASE:
                    SDL_SetTextureColorMod(e->sprite, 255, 0, 0);
                    break;
                default:
                    SDL_SetTextureColorMod(e->sprite, 255, 255, 255);
                    break;
            }
        } else {
            SDL_SetTextureColorMod(e->sprite, 255, 255, 255);
        }

        SDL_RenderCopy(renderer, e->sprite, NULL, &dest);
    }
}

// -----------------------------------------------------------------------------
// Entity Updates
// -----------------------------------------------------------------------------

void update_entities() {
    for (int i = 0; i < entity_count; i++) {
        Entity* e = &entities[i];

        if (!e->is_player) {
            npc_brain(e);
        }

        if (e->behavior) {
            e->behavior(e);
        }

        update_entity_movement(e);
    }
}

void update_entity_movement(Entity* e) {
    if (!e) return;
    
    if (!e->path) {
        return;
    }

    // Skip first node if it matches current position
    if (e->path->current < e->path->length) {
        PathNode first = e->path->nodes[e->path->current];
        if (first.x == e->x && first.y == e->y) {
            e->path->current++;
        }
    }

    // Path complete
    if (e->path->current >= e->path->length) {
        free_path(e->path);
        e->path = NULL;
        e->moving = 0;
        return;
    }

    // Interpolating between tiles
    if (e->moving) {
        e->move_progress += MOVE_PROGRESS;

        if (e->move_progress >= 1.0f) {
            e->x = e->to_x;
            e->y = e->to_y;
            e->render_x = (float)e->x;
            e->render_y = (float)e->y;
            e->move_progress = 0.0f;
            e->moving = 0;
            e->path->current++;
            e->move_cooldown = e->move_delay;
        } else {
            float t = e->move_progress;
            e->render_x = e->from_x + (e->to_x - e->from_x) * t;
            e->render_y = e->from_y + (e->to_y - e->from_y) * t;
        }

        return;
    }

    // Cooldown check
    if (e->move_cooldown > 0) {
        e->move_cooldown--;
        return;
    }

    // Start movement to next tile
    PathNode next = e->path->nodes[e->path->current];

    e->from_x = (float)e->x;
    e->from_y = (float)e->y;
    e->to_x = next.x;
    e->to_y = next.y;
    e->moving = 1;
    e->move_progress = 0.0f;
}

// -----------------------------------------------------------------------------
// Entity Queries
// -----------------------------------------------------------------------------

Entity* get_player() {
    for (int i = 0; i < entity_count; i++) {
        if (entities[i].is_player) return &entities[i];
    }
    return NULL;
}
