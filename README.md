# Oblique Engine

## Executive Summary

Oblique is a modular isometric RPG engine written in C using SDL2, inspired by late-90s CRPGs. The project focuses on low-level engine architecture, isometric rendering, entity systems, AI behaviors, and grid-based navigation, with an emphasis on correctness, extensibility, and systems-level understanding.

The engine is built from scratch to explore game engine fundamentals and demonstrate proficiency in systems programming, rendering pipelines, and modular architecture design.

---

## Key Technical Features

- **Isometric Rendering** - Diamond-projection coordinate system with camera following
- **Entity System** - Unified system for players, NPCs, and props with behavior function pointers
- **Grid-Based Navigation** - Click-to-select movement with visual grid overlay
- **AI System** - State machine with behavior functions (idle, wander, chase)
- **Modular Architecture** - System-based design with clear interfaces and separation of concerns
- **Scene Management** - Support for multiple game modes (explore, combat)
- **Coordinate Conversion** - Screen-to-tile conversion with camera offset handling

---

## Architecture Overview

The engine follows a **system-based architecture** where subsystems communicate through well-defined interfaces. Each system is encapsulated and can be developed or replaced independently.

**Main Systems:**
- Core (game loop, input, scenes, map loading)
- Entity (game objects with behavior functions)
- Render (isometric projection, camera, rendering pipeline)
- Navigation (grid overlay, tile selection, coordinate conversion)
- AI (state machine and behavior functions)
- Collision (detection and resolution)
- UI (interface rendering)

**Main Game Loop:**
```
Input → Scene Update (Camera, Grid, Entities, AI) → Render (Map, Grid, Entities) → Present
```

For detailed architecture documentation, see [docs/architecture.md](docs/architecture.md).

---

## Build & Run

### Prerequisites

- SDL2 (`libsdl2-dev` on Linux, `brew install sdl2` on macOS)
- SDL2_image (`libsdl2-image-dev` on Linux, `brew install sdl2_image` on macOS)

### Build

```bash
git clone https://github.com/djmartin2019/Oblique.git
cd Oblique
make
```

### Run

```bash
./oblique
```

### Controls

- **Mouse Click** - Click on a grid tile to select it and move the player
- **Arrow Keys** - Move the player (legacy, will be replaced with full grid-based movement)

---

## Project Structure

```
oblique/
├── engine/                   # Engine subsystems
│   ├── core/                 # Core engine systems
│   ├── entity/               # Entity system
│   ├── render/               # Rendering system
│   ├── navigation/           # Navigation and grid system
│   ├── ai/                   # AI system
│   ├── collision/            # Collision detection
│   ├── ui/                   # UI system
│   └── helpers/              # Utility functions
├── data/                     # Game assets and data
├── src/                      # Application entry point
├── tools/                    # Development tools
├── docs/                     # Technical documentation
└── Makefile                  # Build configuration
```

See [docs/architecture.md](docs/architecture.md) for detailed system documentation.

---

## Design Philosophy

The engine is structured around **modular ownership** - each subsystem has clear responsibilities and interfaces. Entities use behavior function pointers (`BehaviorFunc`) that allow AI behaviors to be assigned dynamically. This design makes the system:

- **Replaceable**: Systems can be swapped out without rewriting everything
- **Extensible**: New behaviors can be added by implementing the `BehaviorFunc` interface
- **Testable**: Each subsystem can be tested independently
- **Future-proof**: Ready for scripting system integration later

Right now behaviors are hardcoded functions (`wander_behavior`, `idle_behavior`, etc.), but the interface is designed to support scripted behaviors in the future without major refactoring.

---

## Roadmap

### Phase 1: Core Engine Systems ✅
- [x] Rendering, camera, grid, entities

### Phase 2: Navigation & Movement 🔄
- [x] Grid overlay and click-to-select
- [ ] Pathfinding system (A* algorithm)
- [ ] Step-based movement animation

### Phase 3: Combat Systems
- [ ] Action point (AP) system
- [ ] Turn-based combat flow
- [ ] Targeting system

### Phase 4: Content Systems
- [ ] Quest system
- [ ] Dialogue system
- [ ] Inventory system
- [ ] Save/load system

### Phase 5: Tooling & Polish
- [ ] Map editor improvements
- [ ] Debug overlays
- [ ] Audio system
- [ ] Performance profiling

For detailed roadmap, see [docs/roadmap.md](docs/roadmap.md).

---

## Documentation

- [Architecture Overview](docs/architecture.md) - System design and organization
- [Rendering System](docs/rendering.md) - Isometric projection and camera
- [Navigation System](docs/navigation.md) - Grid overlay and coordinate conversion
- [AI System](docs/ai.md) - State machine and behavior functions
- [Roadmap](docs/roadmap.md) - Development phases and planned features

---

## Tech Stack

- **Language:** C (C99)
- **Libraries:** SDL2 (graphics, input), SDL2_image (asset loading)
- **Build System:** Makefile

---

## About Oblique (The Game)

**Oblique** is a post-apocalyptic isometric RPG set in an alternate-history 1980s America where nuclear warfare, not nuclear power, shaped the world. The game follows a Reclaimer—an operative of the American Reclamation Authority (ARA)—as they navigate a world controlled by warring factions: the oil-controlling Petroclave, scattered Settler communities, the fanatical Reverent Forge, and the knowledge-hoarding Circuit Keepers.

The player wears **Obelisk armor**—lightweight, unyielding, and righteous—as they decide whether to restore the old world, burn it down, or build something entirely new.

![Oblique Game Poster](assets/Oblique-Poster.png)

### Game World & Factions

#### 🌍 The World of Oblique

**Genre:** Isometric post-apocalyptic RPG  
**Aesthetic:** 1980s retro-futurism, oil-stained dystopia, militarized techno-Americana  
**Theme:** Rebuild the future. Or burn it again.

#### 💣 Alt-History Backstory

History didn't veer off because of nuclear power—it veered because of nuclear warfare.

In the world of Oblique, nuclear energy was never embraced as a civilian power source. It remained what it was designed to be: a weapon. America, victorious and ravenous after World War II, fell into the hands of General Douglas MacArthur, who rose to presidency following a decisive nuclear victory in the Korean War.

His doctrine of preemptive strength—massive force at any cost—set the tone for the rest of the century. America crushed North Vietnam in record time. Anti-war sentiment never had a chance to form. The Cold War didn't freeze—it ignited. By the 1980s, tactical nukes were being used around the world like drone strikes. The air thickened. The oceans boiled. Oil fields burned like signal fires.

In 1985, the world finally collapsed under its own military-industrial weight. A full-scale nuclear exchange between the U.S., USSR, and China turned Earth into a choking furnace of smoke, ash, and decay.

And from the ruins… came silence. Then, survival. Then… control.

#### 🛡️ The Player's Faction: The American Reclamation Authority (ARA)

**"Restore. Reclaim. Rebirth."**

You are a Reclaimer—an operative of the ARA, an underground remnant of the pre-war American government, bred and trained in isolation for one purpose: Rebuild America.

The ARA is not a people's movement. It is a program. A mission. A cult of patriotic doctrine, forged in the bunkers beneath D.C. and scattered across the country. Its agents wear reclamation armor like sacred vestments and carry energy weapons that hum with ancestral fury.

They believe the world can be restored. But only their version of it.

**Key Traits:**
* Brainwashed military idealism
* Access to rare and advanced energy weapons
* Reclamation armor (Obelisks)
* Radio broadcast propaganda drones
* Small in number, massive in firepower

**Player's Conflict:**
* The world doesn't want to be saved by you.
* You've been trained to rebuild a country that doesn't exist anymore.
* You can liberate… or conquer.

#### ⚙️ The Major Factions

##### 🛢️ The Petroclave

**"Energy is Order."**

The Oil Barons never fell. When the world burned, they just turned off the lights and waited. Now, decades later, they've re-emerged, controlling the only functional power grids, fuel refineries, and pre-war infrastructure left in the South.

They've evolved into a corporate-feudal cult, where each "Baron" runs a city-state powered by black gold. The Petroclave decides who gets power and who gets left in the dark. Literally.

**Key Traits:**
* Massive influence over energy distribution
* Private armies equipped with diesel-powered exosuits, flamethrowers, and combustion-based tech
* Operate out of fortified refineries and rig cities
* Treat settlers like debt-bound laborers
* Worship industrial power as a divine right

##### 🪓 The Settlers

**"We dig. We bleed. We survive."**

Scattered survivors who carved lives out of irradiated ruins and chemical wastelands. These are the families, scavengers, drifters, farmers, and raiders who never had a bunker or a flag—just guns, grit, and duct tape.

They've been under Petroclave rule for decades, trading labor for power, food, or water. But now that the ARA has emerged, some see a new path forward.

Others? They see another tyrant in fancy armor.

**Key Traits:**
* Varied tech (from bolt-action rifles to homemade drones)
* Culture is regionally divided: bayou cultists, desert warbands, ruined city gangs, etc.
* Can be rallied—but easily divided
* Morally complex, desperate, human

##### 🔥 The Reverent Forge

**"Heat is purity. Fire is rebirth."**

An offshoot cult that broke from the Petroclave, the Forge believes in literal purification through combustion. They operate furnace-halls, burn offerings, and see technology as holy when powered by fire.

They are feared even by the Barons. They wear melted metal like armor. They eat coal.

**Key Traits:**
* Fire-based melee weapons and flamethrowers
* Fanatical devotion to "The Flame Protocols"
* Ritual sacrifices of "unclean tech"
* Wear boilerplate armor and respirators
* Often nomadic, but hold massive bonfire ceremonies in ruined stadiums and arenas

##### 📡 The Circuit Keepers

**"All knowledge must be preserved… and restricted."**

Based out of a crumbling data center once run by the military-industrial complex, the Circuit Keepers are a cult of archivists, engineers, and failed AI worshippers.

They hoard information and tech, believing only they are qualified to steward the world's knowledge. Occasionally, they sell old-world blueprints to desperate factions—at a price.

**Key Traits:**
* Fragile but extremely advanced in technology
* Drone networks, radio-wave weaponry, surveillance tech
* Mostly passive, until provoked
* Some seek alliance with the ARA. Others see it as a threat to their monopoly.

#### 🗿 Reclaimer Armor – "Obelisks"

**"Lightweight. Unyielding. Righteous."**

Reclaimers wear Obelisk armor—carbon nanotube lattice plating powered by HY-DRA cores (Hydrogen-Dynamic Reclamation Arrays). The armor is not just protection; it's a symbol of the ARA's mission and a tool of both liberation and oppression.

**Design Principles:**
* Ultra-lightweight carbon nanotube composite that hardens on impact
* Hydrogen fuel cell power source with weeks of operation
* Neuro-linked HUD with tactical overlay, night vision, and voice commands
* Bulletproof at medium to long range, highly resistant to energy weapons
* Agile and responsive—operates like a second skin with servo assistance

**Visual Style:**
* Stormtrooper × Master Chief × Robocop × 80s Black-Ops Mascot
* Full-face visor, semi-reflective, no human detail visible
* Clean, angular, military-industrial lines
* Color scheme: matte black, gunmetal, ceramic white
* Rank insignias burned into the plating

People don't say "He's wearing armor."  
They say "He's wearing an Obelisk."

That's terrifying. That's powerful. That's Oblique.

---

## Status

Currently in **pre-alpha tech demo** phase. Core engine systems are functional: isometric rendering, entity system, grid-based navigation, and AI behaviors are implemented and working.

---

## License

MIT. Do whatever you want. But if you blame me for your suffering, that's on you.

---

## Author

Built by someone who decided that pain = progress.
