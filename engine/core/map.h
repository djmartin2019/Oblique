#ifndef MAP_H
#define MAP_H

#include "core/constants.h"

#define MAP_WIDTH 20
#define MAP_HEIGHT 20

extern int tile_map[MAP_HEIGHT][MAP_WIDTH];

int map_is_walkable(int x, int y);

int load_map(const char* filename);

#endif
