#include "render/camera.h"
#include "render/render.h" // For TILE_WIDTH and TILE_HEIGHT
#include "core/map.h"    // For MAP_WIDTH / MAP_HEIGHT

int map_offset_x = 0;
int map_offset_y = 0;

// Centers the map on the screen at startup
void calculate_map_offset() {
    // Get the tile in the *center* of the map grid
    int center_tile_x = MAP_WIDTH / 2;
    int center_tile_y = MAP_HEIGHT / 2;

    // Convert center tile to screen coordinates (in isometric space)
    int map_center_x = (center_tile_x - center_tile_y) * (TILE_WIDTH / 2);
    int map_center_y = (center_tile_x + center_tile_y) * (TILE_HEIGHT / 2);

    // Calculate how far to offset the map so that the center tile ends up
    // in the *center* of the game window (800x600)
    map_offset_x = (800 / 2) - map_center_x;
    map_offset_y = (600 / 2) - map_center_y;
}

// Keeps the player centered by moving the camera offset
void update_camera(Camera* cam, int player_x, int player_y) {
    // Screen center in pixels
    int screen_center_x = 800 / 2;
    int screen_center_y = 600 / 2;

    // Convert player's tile position to isometric screen coordinates
    int iso_x = (player_x - player_y) * (TILE_WIDTH / 2) + map_offset_x;
    int iso_y = (player_x + player_y) * (TILE_HEIGHT / 2) + map_offset_y;

    // Move the camera so that the player is always centered on the screen
    cam->x = iso_x - screen_center_x;
    cam->y = iso_y - screen_center_y;
}
