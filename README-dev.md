# Oblique Engine - Camera System Overview

This document explains how the isometric camera system in the Oblique Engine works, in plain English, for Future David (who has likely forgotten how anything works).

---

## 🤖 Core Concepts

The engine simulates an old-school isometric view, like Fallout 1/2 or tactics RPGs. The main systems involved:

* **Tile grid** — The game world is a 2D grid of tiles.
* **Isometric projection** — Grid coordinates are converted to diamond-shaped screen positions.
* **Camera** — Moves around the world to keep the player centered.
* **Map offset** — Centers the map when the game starts.

---

## 🧮 Coordinate Flow

To draw anything on screen:

```
World (tile) position
  → Isometric projection
    → Apply map offset (to center map)
      → Subtract camera position (to simulate camera scroll)
        = Final screen position
```

> `screen = isometric(world) + map_offset - camera`

Simple. Not easy.

---

## 🔹 `calculate_map_offset()`

Centers the map when the game boots.

```c
int center_tile_x = MAP_WIDTH / 2;
int center_tile_y = MAP_HEIGHT / 2;

int map_center_x = (center_tile_x - center_tile_y) * (TILE_WIDTH / 2);
int map_center_y = (center_tile_x + center_tile_y) * (TILE_HEIGHT / 2);

map_offset_x = (WINDOW_WIDTH / 2) - map_center_x;
map_offset_y = (WINDOW_HEIGHT / 2) - map_center_y;
```

This ensures the *middle tile* lands dead center of the window, not the top-left.

---

## 🔸 `update_camera()`

Centers the camera on the player every frame.

```c
int iso_x = (player_x - player_y) * (TILE_WIDTH / 2) + map_offset_x;
int iso_y = (player_x + player_y) * (TILE_HEIGHT / 2) + map_offset_y;

cam->x = iso_x - screen_center_x;
cam->y = iso_y - screen_center_y;
```

The camera doesn’t move the player. It moves the *world* to make it look like the player is staying still.

---

## 🗺️ `draw_map()`

Draws the map by projecting every tile into screen space using isometric math.

```c
int screen_x = (x - y) * (TILE_WIDTH / 2) - cam->x + map_offset_x;
int screen_y = (x + y) * (TILE_HEIGHT / 2) - cam->y + map_offset_y;
```

Each tile is drawn in order, using its `tile_id` to grab the texture.

---

## 🧍‍♂️ `draw_player()`

Draws the player at their tile, centered visually using an offset.

```c
int screen_x = (player->x - player->y) * (TILE_WIDTH / 2) - cam->x + map_offset_x;
int screen_y = (player->x + player->y) * (TILE_HEIGHT / 2) - cam->y + map_offset_y;

SDL_Rect dest = {
    screen_x - 16,      // 32px wide sprite
    screen_y - 48,      // 64px tall sprite, feet on tile
    32,
    64
};
```

---

## 💡 Debug Tips

* If the player floats weirdly, your sprite offset is probably wrong.
* If the player and tile locations don't line up, your map_offset is wrong.
* If the player is centered but the world drifts, your camera math is cursed.
* If everything is broken, stop hardcoding +400 and +50 everywhere.

---

## 🧠 Remember

> World + offset - camera = screen position

The camera doesn’t move the player. The world moves *around* the player.

Yes, Unity does all this for you. But now *you* are Unity. Sleep well.

