#ifndef SCENE_H
#define SCENE_H

#include <SDL2/SDL.h>

typedef enum {
    SCENE_EXPLORE,
    SCENE_COMBAT,
} SceneType;

#include "render/camera.h"

void set_scene(SceneType type, SDL_Renderer* renderer);

SceneType get_scene();

void update_scene();
void setup_explore_scene(SDL_Renderer* renderer);
void render_scene(SDL_Renderer* renderer);

Camera* get_camera(void);

#endif
