#include "map.h"
#include <stdio.h>

int tile_map[MAP_HEIGHT][MAP_WIDTH];

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
