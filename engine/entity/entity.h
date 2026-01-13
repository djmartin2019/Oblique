#ifndef ENTITY_H
#define ENTITY_H

#include "render/camera.h"
#include "ai/ai.h"

#include <SDL2/SDL.h>

#define MAX_ENTITIES 128

// Forward declaration
typedef struct Entity Entity;

// Behavior function pointer uses Entity*
typedef void (*BehaviorFunc)(Entity* self);

struct Entity {
    int x, y;               // Tile position
    SDL_Texture* sprite;    // Sprite texture

    int width, height;      // Sprite dimensions
    int offset_x, offset_y; // Pixel offsets for alignment
    int is_player;          // Flag to identify the player

    BehaviorFunc behavior;  // AI behavior
    AIState state;

    // State-colored sprites
    SDL_Texture* sprite_idle;
    SDL_Texture* sprite_wander;
    SDL_Texture* sprite_chase;

    int ap;
};

extern Entity entities[MAX_ENTITIES];
extern int entity_count;

Entity* get_player();

void init_entities();
int add_entity(
        int x,
        int y,
        SDL_Texture* sprite,
        int width,
        int height,
        int offset_x,
        int offset_y,
        int is_player,
        BehaviorFunc behavior
);

void draw_entities(SDL_Renderer* renderer, Camera* cam);
void update_entities();

#endif
