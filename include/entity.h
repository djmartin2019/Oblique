#ifndef ENTITY_H
#define ENTITY_H

#include <SDL2/SDL.h>

#define MAX_ENTITIES 128

typedef struct {
    int x, y;               // Tile position
    SDL_Texture* sprite;    // Sprite texture
    int width, height;      // Sprite dimensions
    int offset_x, offset_y; // Pixel offsets for alignment
    int is_player;          // Flag to identify the player
} Entity;

extern Entity entities[MAX_ENTITIES];
extern int entity_count;

void init_entities();
int add_entity(int x, int y, SDL_Texture* sprite, int width, int height, int offset_x, int offset_y, int is_player);
void draw_entities(SDL_Renderer* renderer, struct Camera* cam);

#endif
