#include "core/tile.h"
#include "core/map.h"

TileDef tile_defs[] = {
    [TILE_GRASS]    = { 1, 1 },
    [TILE_ROAD]     = { 1, 1 },
    [TILE_RUBBLE]   = { 1, 2 },
    [TILE_WATER]    = { 0, 0 }
};

int is_tile_walkable(int x, int y) {
    int id = tile_map[y][x];
    return tile_defs[id].walkable;
}

int tile_move_cost(int x, int y) {
    int id = tile_map[y][x];
    return tile_defs[id].move_cost;
}
