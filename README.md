# Oblique Engine

*A retro-style isometric RPG engine built in pure C, inspired by late-90s classics like Fallout.*

This is a masochistically low-level engine being built from scratch using C and SDL2. The goal is to explore what it takes to make a working isometric game engine in the style of 1997-era CRPGs, and to build a solid understanding of systems-level programming, rendering, game loops, and engine architecture.

> "Why not use Unity?"
> Because I want to suffer **on purpose**.

---

## Features (Planned or Working)

* [x] 2D tile-based map system
* [x] Isometric projection rendering
* [x] Player movement with camera following
* [x] Basic UI system
* [x] Entity system with behavior functions
* [x] NPC AI system (idle, wander, chase behaviors)
* [x] Collision detection system
* [x] Scene management system
* [x] Inventory screen with three dumb items
* [ ] Combat system
* [ ] Quests and triggers
* [ ] Save/load system
* [ ] Audio support
* [ ] A reason to keep going

---

## Tech Stack

* **Language:** C (C99, because ANSI was just *too* cursed)
* **Libraries:** SDL2 for graphics and input, SDL2_image for asset loading
* **Assets:** Placeholder pixel art (probably from OpenGameArt or MSPaint)
* **Build system:** Makefile (pain edition)

---

## Project Structure

The engine is organized into **modular subsystems**, each with clear responsibilities and interfaces:

```
oblique/
├── engine/                   # Engine subsystems
│   ├── core/                 # Core engine systems
│   │   ├── constants.c/h     # Game constants and configuration
│   │   ├── engine.c/h        # Engine initialization
│   │   ├── game.c/h          # Game loop and main logic
│   │   ├── input.c/h         # Input handling
│   │   ├── map.c/h           # Map loading and tile system
│   │   └── scene.c/h         # Scene management (explore, combat)
│   ├── entity/               # Entity system
│   │   ├── entity.c/h        # Entity lifecycle and management
│   │   └── player.c/h        # Player-specific logic
│   ├── render/               # Rendering system
│   │   ├── camera.c/h        # Camera and viewport management
│   │   └── render.c/h        # Rendering pipeline
│   ├── ai/                   # AI system
│   │   ├── ai.c/h            # AI state machine and brain logic
│   │   └── behavior.c/h      # Behavior functions (wander, idle, chase)
│   ├── collision/            # Collision detection
│   │   ├── collision.c/h     # Collision queries and resolution
│   ├── ui/                   # UI system
│   │   ├── ui.c/h            # User interface rendering
│   └── helpers/              # Utility functions
│       └── sdl_helpers.c/h   # SDL initialization and cleanup
├── data/                     # Game assets and data
│   ├── fonts/                # Font files
│   ├── maps/                 # Map data files
│   ├── sprites/              # Sprite assets
│   └── tiles/                # Tile assets
├── src/                      # Application entry point
│   └── main.c                # Main function and game loop
├── tools/                    # Development tools
│   └── map_editor/           # Map editor utility
│       ├── map_editor.c/h
└── Makefile                  # Build configuration
```

---

## Architecture & Data Flow

The engine follows a **system-based architecture** where subsystems communicate through well-defined interfaces. Here's the main game loop and data flow:

### Main Game Loop

```
1. Input System      → Capture keyboard state
2. Player Input      → Feed input to player behavior
3. Scene Update      → Update all systems:
   ├── Camera        → Follow player position
   ├── Entities      → Update all entities (player + NPCs)
   │   ├── AI Brain  → Update NPC AI states
   │   └── Behavior  → Execute behavior functions
   └── Collision     → Resolve movement conflicts
4. Render System     → Draw frame:
   ├── Map           → Draw tile grid
   └── Entities      → Draw all entities with depth sorting
5. Present           → Show frame on screen
```

### Subsystem Organization

Each subsystem is **encapsulated** with its own interface, making the engine modular and maintainable:

- **Core System** (`core/`) - Manages the game loop, input, scenes, and map loading
- **Entity System** (`entity/`) - Handles all game objects (player, NPCs, props) with behavior functions
- **Render System** (`render/`) - Manages isometric projection, camera, and rendering pipeline
- **AI System** (`ai/`) - Provides AI state machine and behavior functions that can be assigned to entities
- **Collision System** (`collision/`) - Handles collision detection and resolution
- **UI System** (`ui/`) - Renders user interface elements

This structure allows each system to be **developed and replaced independently** without rewriting the entire engine.

---

## How to Run

1. Install SDL2 (`libsdl2-dev` on Linux, `brew install sdl2` on macOS, or cry on Windows)
2. Install SDL2_image (`libsdl2-image-dev` on Linux, `brew install sdl2_image` on macOS)
3. Clone the repo
4. Run `make`
5. Pray
6. Execute `./oblique` and behold the janky glory

---

## Goals

* Master C through practical, painful use
* Understand game architecture from scratch
* Build a modular, maintainable engine architecture
* Create clear subsystem boundaries with well-defined interfaces
* Build something that *feels* powerful, even if it's held together with duct tape
* Create a playable, working isometric RPG prototype

---

## Design Philosophy

The engine is structured around **modular ownership** - each subsystem has clear responsibilities and interfaces. Entities use behavior function pointers (`BehaviorFunc`) that allow AI behaviors to be assigned dynamically. This design makes the system:

- **Replaceable**: Systems can be swapped out without rewriting everything
- **Extensible**: New behaviors can be added by implementing the `BehaviorFunc` interface
- **Testable**: Each subsystem can be tested independently
- **Future-proof**: Ready for scripting system integration later

Right now behaviors are hardcoded functions (`wander_behavior`, `idle_behavior`, etc.), but the interface is designed to support scripted behaviors in the future without major refactoring.

---

## What This Isn't

* A modern game engine
* Easy to use
* Documented well (yet)
* Mentally healthy

---

## Status

Currently in **pre-alpha tech demo** phase. You can move around, talk to an NPC, watch NPCs wander with AI behaviors, open an inventory, and contemplate your life choices. The engine architecture is becoming more cohesive with each refactor, moving from "prototyping chaos" toward "modular system of systems."

---

## License

MIT. Do whatever you want. But if you blame me for your suffering, that’s on you.

---

## Author

Built by someone who decided that pain = progress.

