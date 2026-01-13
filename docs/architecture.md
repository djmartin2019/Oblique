# Architecture Overview

The Oblique Engine follows a **system-based architecture** where subsystems communicate through well-defined interfaces. This design allows each system to be developed and replaced independently without rewriting the entire engine.

## System Organization

Each subsystem is **encapsulated** with its own interface:

- **Core System** (`core/`) - Manages the game loop, input, scenes, and map loading
- **Entity System** (`entity/`) - Handles all game objects (player, NPCs, props) with behavior functions
- **Render System** (`render/`) - Manages isometric projection, camera, and rendering pipeline
- **Navigation System** (`navigation/`) - Grid overlay, tile selection, screen-to-tile coordinate conversion
- **AI System** (`ai/`) - Provides AI state machine and behavior functions that can be assigned to entities
- **Collision System** (`collision/`) - Handles collision detection and resolution
- **UI System** (`ui/`) - Renders user interface elements

## Main Game Loop

```
1. Input System      → Capture keyboard/mouse events
2. Player Input       → Handle mouse clicks for tile selection
3. Scene Update       → Update all systems:
   ├── Camera         → Follow player position
   ├── Grid           → Calculate valid movement tiles
   ├── Entities       → Update all entities (player + NPCs)
   │   ├── AI Brain   → Update NPC AI states
   │   └── Behavior   → Execute behavior functions
   └── Collision      → Resolve movement conflicts
4. Render System      → Draw frame:
   ├── Map            → Draw tile grid
   ├── Grid           → Draw grid outlines
   ├── Selected Tile  → Draw selected tile highlight
   └── Entities       → Draw all entities with depth sorting
5. Present            → Show frame on screen
```

## Design Philosophy

The engine is structured around **modular ownership** - each subsystem has clear responsibilities and interfaces. Entities use behavior function pointers (`BehaviorFunc`) that allow AI behaviors to be assigned dynamically. This design makes the system:

- **Replaceable**: Systems can be swapped out without rewriting everything
- **Extensible**: New behaviors can be added by implementing the `BehaviorFunc` interface
- **Testable**: Each subsystem can be tested independently
- **Future-proof**: Ready for scripting system integration later

Right now behaviors are hardcoded functions (`wander_behavior`, `idle_behavior`, etc.), but the interface is designed to support scripted behaviors in the future without major refactoring.

## Entity System

The entity system consolidates players, NPCs, and props into a single unified system. Each entity has:

- Position (tile coordinates)
- Sprite and rendering properties
- Behavior function pointer (for AI or player input)
- State information (AI state, action points, etc.)

This allows iteration, update, and rendering in one place, and will support depth sorting for proper isometric layering.

See [AI System Documentation](ai.md) for details on behavior functions and AI states.
