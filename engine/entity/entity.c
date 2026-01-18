#include "entity/entity.h"
#include "render/camera.h"
#include "render/render.h"     //For TILE_WIDTH / TILE_HEIGHT
#include "ai/behavior.h"
#include "core/constants.h"

#define MOVE_PROGRESS 0.2f  // Movement speed (0.2 means ~5 frames per tile at 100ms delay = 500ms per tile)

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
    e->render_x = (float)x;  // Initialize render position
    e->render_y = (float)y;
    e->sprite = sprite;
    e->width = width;
    e->height = height;
    e->offset_x = offset_x;
    e->offset_y = offset_y;
    e->is_player = is_player;
    e->behavior = behavior;
    
    // Initialize movement fields
    e->move_progress = 0.0f;
    e->moving = 0;
    e->from_x = x;
    e->from_y = y;
    e->to_x = x;
    e->to_y = y;
    e->path = NULL;
    e->move_cooldown = 0;
    e->move_delay = 6;  // Default move delay

    return entity_count - 1; // Return index
}

void draw_entities(SDL_Renderer* renderer, Camera* cam) {
    for (int i = 0; i < entity_count; i++) {
        Entity* e = &entities[i];

        // Use interpolated render position
        float rx = e->render_x;
        float ry = e->render_y;

        // Calculate screen position using the same formula as tiles and grid
        // This ensures entities align perfectly with the grid
        int screen_x = (rx - ry) * (TILE_WIDTH / 2) - cam->x + map_offset_x;
        int screen_y = (rx + ry) * (TILE_HEIGHT / 2) - cam->y + map_offset_y;
        
        // Apply sprite offsets to position the sprite correctly on the tile
        // offset_x and offset_y are used to align the sprite's feet with the tile center
        screen_x += e->offset_x;
        screen_y += e->offset_y;

        SDL_Rect dest = {
            screen_x,
            screen_y,
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

        update_entity_movement(e);
    }
}

Entity* get_player() {
    for (int i = 0; i < entity_count; i++) {
        if (entities[i].is_player) return &entities[i];
    }
    return NULL;
}

void update_entity_movement(Entity* e) {
    if (!e) return;
    
    if (!e->path) {
        return;
    }

    // Skip the first node if it's the current position (path includes start node)
    if (e->path->current < e->path->length) {
        PathNode first = e->path->nodes[e->path->current];
        if (first.x == e->x && first.y == e->y) {
            printf("Skipping first node (%d,%d) - matches current position\n", first.x, first.y);
            e->path->current++;
        }
    }

    // If we've reached the end of the path, clean up
    if (e->path->current >= e->path->length) {
        free_path(e->path);
        e->path = NULL;
        e->moving = 0;
        return;
    }

    // If we're currently interpolating between tiles, continue moving
    if (e->moving) {
        // Advance interpolation (tune this value for speed)
        e->move_progress += MOVE_PROGRESS;

        if (e->move_progress >= 1.0f) {
            // Snap to destination tile logically
            e->x = e->to_x;
            e->y = e->to_y;

            // Finish movement
            e->render_x = (float)e->x;
            e->render_y = (float)e->y;

            e->move_progress = 0.0f;
            e->moving = 0;

            // Advance path
            e->path->current++;

            // Apply cooldown before next tile
            e->move_cooldown = e->move_delay;
        } else {
            // Interpolate render position only
            float t = e->move_progress;
            e->render_x = e->from_x + (e->to_x - e->from_x) * t;
            e->render_y = e->from_y + (e->to_y - e->from_y) * t;
        }

        return;
    }

    // Not moving yet, respect cooldown
    if (e->move_cooldown > 0) {
        e->move_cooldown--;
        return;
    }

    // Move to next tile in the path
    PathNode next = e->path->nodes[e->path->current];
    printf("Starting movement: from (%d,%d) to (%d,%d), path current=%d/%d\n", 
           e->x, e->y, next.x, next.y, e->path->current, e->path->length);

    // Store starting position for interpolation
    e->from_x = (float)e->x;
    e->from_y = (float)e->y;
    
    // Store destination (don't update logical position yet - that happens when movement completes)
    e->to_x = next.x;
    e->to_y = next.y;

    // Start moving
    e->moving = 1;
    e->move_progress = 0.0f;
}
