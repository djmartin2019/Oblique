#ifndef SCENE_H
#define SCENE_H

#include <SDL2/SDL.h>

struct Entity;

typedef enum {
    SCENE_EXPLORE,
    SCENE_COMBAT,
} SceneType;

#include "render/camera.h"

void set_scene(SceneType type, SDL_Renderer* renderer);

SceneType get_scene();
int is_combat_active(void);
int is_combat_forced(void);
void force_combat(void);
void clear_forced_combat(void);
int is_entity_turn(struct Entity* e);

void update_scene();
void setup_explore_scene(SDL_Renderer* renderer);
void render_scene(SDL_Renderer* renderer);

Camera* get_camera(void);

#endif
