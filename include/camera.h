#ifndef CAMERA_H
#define CAMERA_H

typedef struct Camera {
    int x, y; // world offset in pixels
} Camera;

extern int map_offset_x;
extern int map_offset_y;

void update_camera(Camera* cam, int player_x, int player_y);
void calculate_map_offset();

#endif
