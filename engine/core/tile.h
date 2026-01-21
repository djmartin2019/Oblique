typedef struct {
    int walkable;
    int move_cost;
} TileDef;

enum {
    TILE_GRASS  = 0,
    TILE_ROAD   = 1,
    TILE_RUBBLE = 2,
    TILE_WATER  = 3
};

int is_tile_walkable(int x, int y);

int tile_move_cost(int x, int y);
