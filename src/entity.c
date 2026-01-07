#include "entity.h"
#include "camera.h"
#include "render.h"     //For TILE_WIDTH / TILE_HEIGHT

Entity entities[MAX_ENTITIES];
int entity_count = 0;

void init_entities() {
    entity_count = 0;
}

int add_entity(int x, int y, SDL_Texture* sprite, int width, int height, int offset_x, int offset_y, int is_player) {
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

    return entity_count - 1; // Return index
}

void draw_entities(SDL_Renderer* renderer, Camera* am) {
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

        SDL_RenderCopy(renderer, e->sprite, NULL, &dest);
    }
}
