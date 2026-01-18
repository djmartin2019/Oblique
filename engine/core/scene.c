#include "core/scene.h"
#include "core/map.h"
#include "core/constants.h"
// #include "core/combat.h"
#include "entity/entity.h"
#include "render/camera.h"
#include "render/render.h"
#include "ai/behavior.h"
#include "navigation/grid.h"

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
    load_map(DEFAULT_MAP);
    load_tile_textures(renderer);

    calculate_map_offset();

    // Load player sprites
    SDL_Surface* surface = IMG_Load(PLAYER_SPRITE);
    SDL_Texture* player_texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    player_id = add_entity(
            5, 5,
            player_texture,
            32, 64,
            16,   // offset_x: center sprite horizontally (TILE_WIDTH/2 - sprite_width/2 = 32 - 16 = 16)
            -48,  // offset_y: align feet with tile center
                  // Tile center is at TILE_HEIGHT/2 = 16px from tile top
                  // Sprite bottom should be at tile center, so: offset_y + sprite_height = TILE_HEIGHT/2
                  // Therefore: offset_y = 16 - 64 = -48
            1,
            player_behavior
    );

    // NPC sprite (shared for idle/wander/chase for now)
    SDL_Surface* npc_surf = IMG_Load(NPC_SPRITE);
    SDL_Texture* npc_tex  = SDL_CreateTextureFromSurface(renderer, npc_surf);
    SDL_FreeSurface(npc_surf);

    int npc_id = add_entity(10, 10, npc_tex, 32, 64, 16, -48, 0, wander_behavior);
    entities[npc_id].state = STATE_IDLE;
    entities[npc_id].sprite_idle   = npc_tex;
    entities[npc_id].sprite_wander = npc_tex;
    entities[npc_id].sprite_chase  = npc_tex;
}

void setup_combat_scene(SDL_Renderer* renderer) {
    // For now, no reloading or new map. Eventually, you could load a special "combat_map"

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
        calculate_move_grid(player->x, player->y, 10);
    }

    update_entities();                  // AI + player input

    switch (current_scene) {
        case SCENE_EXPLORE:
            update_entities();          // Behaviors, movement, etc.
            break;
        case SCENE_COMBAT:
            // update_combat();            // Turn system
            break;
    }
}

void render_scene(SDL_Renderer* renderer) {
    calculate_map_offset();
    draw_map(renderer, &camera);            // 1. draw map tiles
    draw_move_grid(renderer, &camera);      // 2. draw grid UNDER player
    draw_entities(renderer, &camera);       // 3. draw player + NPCs
                                            // 4. UI (Coming soon)

    if (current_scene == SCENE_COMBAT) {
        // renderer_combat_ui(renderer);
    }
}

Camera* get_camera(void) {
    return &camera;
}
