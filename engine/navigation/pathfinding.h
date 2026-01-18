#ifndef PATHFINDING_H
#define PATHFINDING_H

typedef struct {
    int x;
    int y;
} PathNode;

typedef struct {
    PathNode* nodes;
    int length;
    int current;
} Path;

Path* find_path(int start_x, int start_y, int goal_x, int goal_y);
void free_path(Path* path);

#endif
