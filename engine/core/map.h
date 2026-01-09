#ifndef MAP_H
#define MAP_H

#define MAP_WIDTH 20
#define MAP_HEIGHT 20

extern int tile_map[MAP_HEIGHT][MAP_WIDTH];

int load_map(const char* filename);

#endif
