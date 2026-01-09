#include "entity.h"
#include "camera.h"
#include "render.h"     //For TILE_WIDTH / TILE_HEIGHT
#include "behavior.h"

Entity entities[MAX_ENTITIES];
int entity_count = 0;

void init_entities() {
    entity_count = 0;
}

int add_entity(int x, int y, SDL_Texture* sprite, int width, int height, int offset_x, int offset_y, int is_player, BehaviorFunc behavior) {
    if (entity_count >= MAX_ENTITIES) return -1;

    Entity* e = &entities[entity_count++];
    e->x = x;
    e->y = y;
    e->sprite = sprite;
    e->width = width;
    e->height = height;
    e->offset_x = offset_x;
    e->offset_y = offset_y;
    e->is_player = is_player;
    e->behavior = behavior;

    return entity_count - 1; // Return index
}

void draw_entities(SDL_Renderer* renderer, Camera* cam) {
    for (int i = 0; i < entity_count; i++) {
        Entity* e = &entities[i];

        int screen_x = (e->x - e->y) * (TILE_WIDTH / 2) - cam->x + map_offset_x;
        int screen_y = (e->x + e->y) * (TILE_HEIGHT / 2) - cam->y + map_offset_y;

        SDL_Rect dest = {
            screen_x + e->offset_x,
            screen_y + e->offset_y,
            e->width,
            e->height
        };

        // Set sprite color tint based on AI state
        if (!e->is_player) {
            switch (e->state) {
                case STATE_IDLE:
                    SDL_SetTextureColorMod(e->sprite, 64, 64, 64);      // gray
                    break;
                case STATE_WANDER:
                    SDL_SetTextureColorMod(e->sprite, 0, 255, 0);       // green
                    break;
                case STATE_CHASE:
                    SDL_SetTextureColorMod(e->sprite, 255, 0, 0);       // red
                    break;
                default:
                    SDL_SetTextureColorMod(e->sprite, 255, 255, 255);   // default white
                    break;
            }
        } else {
            SDL_SetTextureColorMod(e->sprite, 255, 255, 255);           // Player always render full color
        }

        SDL_RenderCopy(renderer, e->sprite, NULL, &dest);
    }
}

void update_entities() {
    for (int i = 0; i < entity_count; i++) {
        Entity* e = &entities[i];

        // Let the AI update it's brain before acting
        if (!e->is_player) {
            npc_brain(e);
        }

        // Then run its behavior
        if (e->behavior) {
            e->behavior(e);
        }
    }
}
