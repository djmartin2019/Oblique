#include "scene.h"
#include "entity.h"
#include "camera.h"
#include "render.h"
#include "map.h"
#include "behavior.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

static SceneType current_scene = SCENE_EXPLORE;
static int player_id = -1;
static Camera camera;

SceneType get_scene() {
    return current_scene;
}

void setup_explore_scene(SDL_Renderer* renderer) {
    init_entities();        // resets entities array
    load_map("maps/test_map.txt");
    calculate_map_offset();

    // Load player sprites
    SDL_Surface* surface = IMG_Load("assets/sprites/player.png");
    SDL_Texture* player_texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    player_id = add_entity(
            5, 5,
            player_texture,
            32, 64,
            -16, -48,
            1,
            player_behavior
    );

    // NPC sprite (shared for idle/wander/chase for now)
    SDL_Surface* npc_surf = IMG_Load("assets/sprites/npc.png");
    SDL_Texture* npc_tex  = SDL_CreateTextureFromSurface(renderer, npc_surf);
    SDL_FreeSurface(npc_surf);

    int npc_id = add_entity(10, 10, npc_tex, 32, 64, -16, -48, 0, wander_behavior);
    entities[npc_id].state = STATE_IDLE;
    entities[npc_id].sprite_idle   = npc_tex;
    entities[npc_id].sprite_wander = npc_tex;
    entities[npc_id].sprite_chase  = npc_tex;
}

void setup_combat_scene(SDL_Renderer* renderer) {
    // TODO
}

void set_scene(SceneType type, SDL_Renderer* renderer) {
    current_scene = type;

    switch (type) {
        case SCENE_EXPLORE:
            setup_explore_scene(renderer);
            break;
        case SCENE_COMBAT:
            setup_combat_scene(renderer);
            break;
    }
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
