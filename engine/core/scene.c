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
#include <stdlib.h>
#include <stdio.h>

static SceneType current_scene = SCENE_EXPLORE;
static int player_id = -1;
static Camera camera;
static int combat_active = 0;
static int active_turn_index = 0;
static int turn_started = 0;

#define COMBAT_TRIGGER_RANGE 5
#define COMBAT_CLEAR_RANGE 7

SceneType get_scene() {
    return current_scene;
}

int is_combat_active(void) {
    return combat_active;
}

int is_entity_turn(Entity* e) {
    if (!combat_active) return 1;
    if (!e || entity_count <= 0) return 0;
    return e == &entities[active_turn_index];
}

static int find_player_index(void) {
    for (int i = 0; i < entity_count; i++) {
        if (entities[i].is_player) return i;
    }
    return -1;
}

static int is_enemy_near_player(int range) {
    int player_index = find_player_index();
    if (player_index < 0) return 0;

    Entity* player = &entities[player_index];

    for (int i = 0; i < entity_count; i++) {
        Entity* other = &entities[i];
        if (other->is_player) continue;

        int dx = abs(other->x - player->x);
        int dy = abs(other->y - player->y);
        if ((dx + dy) <= range) return 1;
    }

    return 0;
}

static void start_combat(void) {
    combat_active = 1;
    active_turn_index = find_player_index();
    if (active_turn_index < 0) active_turn_index = 0;
    turn_started = 0;
}

static void end_combat(void) {
    combat_active = 0;
    active_turn_index = 0;
    turn_started = 0;
}

static void update_combat_state(void) {
    if (!combat_active) {
        if (is_enemy_near_player(COMBAT_TRIGGER_RANGE)) {
            printf("CombatState: ENTER combat\n");
            start_combat();
        }
        return;
    }

    if (!is_enemy_near_player(COMBAT_CLEAR_RANGE)) {
        printf("CombatState: EXIT combat\n");
        end_combat();
    }
}

static void draw_ap_counter(SDL_Renderer* renderer, Entity* entity) {
    if (!entity) return;

    const int start_x = 20;
    const int start_y = 20;
    const int box_size = 12;
    const int box_gap = 4;

    int ap_max = entity->ap_max;
    int ap_current = entity->ap_current;
    if (ap_max < 0) ap_max = 0;
    if (ap_current < 0) ap_current = 0;
    if (ap_current > ap_max) ap_current = ap_max;

    SDL_Rect bg = {
        start_x - 8,
        start_y - 8,
        ap_max * (box_size + box_gap) + 16 - box_gap,
        box_size + 16
    };

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_RenderFillRect(renderer, &bg);

    for (int i = 0; i < ap_max; i++) {
        SDL_Rect box = {
            start_x + i * (box_size + box_gap),
            start_y,
            box_size,
            box_size
        };

        if (i < ap_current) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_RenderFillRect(renderer, &box);
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &box);
    }
}

static void start_active_turn(void) {
    if (entity_count <= 0) return;
    Entity* active = &entities[active_turn_index];
    active->ap_current = active->ap_max;
    turn_started = 1;
}

static void advance_turn(void) {
    if (entity_count <= 0) return;
    active_turn_index = (active_turn_index + 1) % entity_count;
    turn_started = 0;
}

static void update_combat_turns(void) {
    if (!combat_active || entity_count <= 0) return;

    Entity* active = &entities[active_turn_index];
    if (!turn_started) {
        start_active_turn();
    }

    if (!active->is_player && !active->moving) {
        int path_done = !active->path || active->path->current >= active->path->length;
        if (path_done && active->ap_current > 0) {
            active->ap_current = 0;
        }
    }

    if (!active->moving && active->ap_current <= 0) {
        advance_turn();
    }
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

    update_combat_state();

    switch (current_scene) {
        case SCENE_EXPLORE:
            update_entities();          // Behaviors, movement, etc.
            break;
        case SCENE_COMBAT:
            update_entities();          // Behaviors, movement, etc.
            // update_combat();            // Turn system
            break;
    }

    update_combat_turns();
}

void render_scene(SDL_Renderer* renderer) {
    calculate_map_offset();
    draw_map(renderer, &camera);            // 1. draw map tiles
    draw_move_grid(renderer, &camera);      // 2. draw grid UNDER player
    draw_entities(renderer, &camera);       // 3. draw player + NPCs
                                            // 4. UI (Coming soon)

    if (is_combat_active()) {
        draw_ap_counter(renderer, get_player());
    }
}

Camera* get_camera(void) {
    return &camera;
}
