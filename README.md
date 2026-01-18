# Oblique Engine

*A retro-style isometric RPG engine built in pure C, inspired by late-90s classics like Fallout.*

This is a masochistically low-level engine being built from scratch using C and SDL2. The goal is to explore what it takes to make a working isometric game engine in the style of 1997-era CRPGs, and to build a solid understanding of systems-level programming, rendering, game loops, and engine architecture.

> "Why not use Unity?"
> Because I want to suffer **on purpose**.

---

## Features (Planned or Working)

* [x] 2D tile-based map system
* [x] Isometric projection rendering
* [x] A* pathfinding algorithm
* [x] Smooth interpolation-based movement (tile-by-tile)
* [x] Player movement with camera following
* [x] AI behaviors using pathfinding (wander, chase)
* [x] Grid-based navigation system
* [x] Basic UI system
* [x] One NPC who says a sad thing
* [x] Inventory screen with three dumb items
* [ ] Combat system
* [ ] Quests and triggers
* [ ] Save/load system
* [ ] Audio support
* [ ] A reason to keep going

---

## Tech Stack

* **Language:** C (C99, because ANSI was just *too* cursed)
* **Libraries:** SDL2 for graphics, input, and audio
* **Assets:** Placeholder pixel art (probably from OpenGameArt or MSPaint)
* **Build system:** Makefile (pain edition)

---

## Project Structure

```
engine/
├── src/              # All code lives here in shame
├── assets/           # Tiles, sprites, fonts
├── maps/             # Hand-crafted text-based maps
├── include/          # Header files for pretending there's structure
└── Makefile          # For yelling at gcc
```

---

## How to Run

1. Install SDL2 (`libsdl2-dev` on Linux, `brew install sdl2` on macOS, or cry on Windows)
2. Clone the repo
3. Run `make`
4. Pray
5. Execute the binary and behold the janky glory

---

## Goals

* Master C through practical, painful use
* Understand game architecture from scratch
* Build something that *feels* powerful, even if it’s held together with duct tape
* Create a playable, working isometric RPG prototype

---

## What This Isn’t

* A modern game engine
* Easy to use
* Documented well
* Mentally healthy

---

## Status

Currently in **pre-alpha tech demo** phase. You can move around using pathfinding, watch NPCs navigate with AI behaviors, talk to an NPC, open an inventory, and contemplate your life choices.

## Movement System

The engine uses **A* pathfinding** with **interpolation-based movement** for smooth, tile-by-tile navigation (inspired by Fallout 1/2):

- **Pathfinding**: A* algorithm finds optimal paths through walkable tiles
- **Interpolation**: Entities smoothly slide between tiles instead of teleporting
- **Unified System**: Both player and NPCs use the same movement logic
- **AI Integration**: NPC behaviors (wander, chase) automatically use pathfinding

---

## License

MIT. Do whatever you want. But if you blame me for your suffering, that’s on you.

---

## Author

Built by someone who decided that pain = progress.

