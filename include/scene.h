#ifndef SCENE_H
#define SCENE_H

#include <SDL2/SDL.h>

typedef enum {
    SCENE_EXPLORE,
    SCENE_COMBAT,
} GameScene;

void set_scene(GameScene scene);

GameScene get_scene();

void update_scene();
void render_scene(SDL_Renderer* renderer);

#endif
