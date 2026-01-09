#include "scene.h"
#include "entity.h"
#include "camera.h"
#include "render.h"

static GameScene current_scene = SCENE_EXPLORE;
static Camera camera;

void set_scene(GameScene scene) {
    current_scene = scene;
}

GameScene get_scene() {
    return current_scene;
}

void update_scene() {
    Entity* player = get_player();
    if (player) {
        update_camera(&camera, player->x, player->y);
    }

    update_entities();                  // AI + player input

    switch (current_scene) {
        case SCENE_EXPLORE:
            update_entities();          // Behaviors, movement, etc.
            break;
        case SCENE_COMBAT:
            // update_combat();     // Turn system
            break;
    }
}

void render_scene(SDL_Renderer* renderer) {
    calculate_map_offset();
    draw_map(renderer, &camera);
    draw_entities(renderer, &camera);
}
