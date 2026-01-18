#include "core/map.h"
#include <stdio.h>

int tile_map[MAP_HEIGHT][MAP_WIDTH];

int map_is_walkable(int x, int y) {
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
        return 0;

    // Convention: 0 = walkable (ground), non-zero = blocked (walls/obstacles)
    // This matches the map file where 0 means empty/walkable terrain
    return tile_map[y][x] == 0;
}

int load_map(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Failed to load map file: %s\n", filename);
        return 0;
    }

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (fscanf(file, "%d", &tile_map[y][x]) != 1) {
                printf("Invalid tile data at (%d, %d)\n", x, y);
                fclose(file);
                return 0;
            }
        }
    }

    fclose(file);
    return 1;
}
