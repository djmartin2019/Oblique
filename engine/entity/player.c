#include "entity/player.h"
#include "entity/entity.h"
#include "render/render.h" // for TILE_WIDTH and TILE_HEIGHT
#include "render/camera.h"
#include "core/constants.h"
#include "core/map.h"
#include "core/scene.h"  // For get_camera()
#include "navigation/grid.h"
#include "navigation/pathfinding.h"

#include <SDL2/SDL_image.h>

int init_player(Player* player, SDL_Renderer* renderer) {
    player->x = 5;
    player->y = 5;

    SDL_Surface* surface = IMG_Load(PLAYER_SPRITE);
    if (!surface) {
        printf("Failed to load player sprite: %s\n", SDL_GetError());
        return 0;
    }

    player->sprite = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    return player->sprite != NULL;
}

void handle_player_input(Entity* entity, SDL_Event* event) {
    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
        int mouse_x = event->button.x;
        int mouse_y = event->button.y;

        // Get camera for coordinate conversion
        Camera* cam = get_camera();
        if (!cam) return;

        int tile_x, tile_y;
        screen_to_iso(mouse_x, mouse_y, cam, &tile_x, &tile_y);

        // Ensure tile is in bounds
        if (tile_x >= 0 && tile_x < MAP_WIDTH &&
            tile_y >= 0 && tile_y < MAP_HEIGHT) {

            // Always select the tile when clicked (for highlighting)
            select_tile(tile_x, tile_y);

            // Only try to move if the tile is valid for movement
            if (move_tiles[tile_y][tile_x].valid) {
                // Free any existing path
                if (entity->path) {
                    free_path(entity->path);
                    entity->path = NULL;
                }

                // Find path to destination
                Path* path = find_path(entity->x, entity->y, tile_x, tile_y);
                printf("Pathfinding: from (%d,%d) to (%d,%d)\n", entity->x, entity->y, tile_x, tile_y);
                if (path) {
                    printf("Path created: length=%d\n", path->length);
                    if (path->length > 0) {
                        printf("First node: (%d,%d), Last node: (%d,%d)\n", 
                               path->nodes[0].x, path->nodes[0].y,
                               path->nodes[path->length-1].x, path->nodes[path->length-1].y);
                    }
                } else {
                    printf("Path is NULL\n");
                }
                
                if (path && path->length > 0) {
                    entity->path = path;
                    entity->path->current = 0;
                    entity->moving = 0;  // Reset movement state
                    entity->move_progress = 0.0f;
                    printf("Path assigned to entity\n");
                } else if (path) {
                    // Empty path (same tile), free it
                    free_path(path);
                }
            }
        }
    }
}

// Draws the player relative to the camera + map offset, centered on their tile
void draw_player(Player* player, SDL_Renderer* renderer, Camera* cam) {
    // Get player's isometric screen position relative to the camera and map offset
    int screen_x = (player->x - player->y) * (TILE_WIDTH / 2) - cam->x + map_offset_x;
    int screen_y = (player->x + player->y) * (TILE_HEIGHT / 2) - cam->y + map_offset_y;

    // Adjust drawing rectangle to center the player sprite on the tile
    // Assumes the sprite is 32x64px, with the feet at the tile center
    SDL_Rect dest = {
        screen_x - 16,      // Shift left half the sprite's width
        screen_y - 48,      // Shift up so the feet sit on tile
        32,
        64
    };

    SDL_RenderCopy(renderer, player->sprite, NULL, &dest);
}
