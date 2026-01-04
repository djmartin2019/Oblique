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

Currently in **pre-alpha tech demo** phase. You can move around, talk to an NPC, open an inventory, and contemplate your life choices.

---

## License

MIT. Do whatever you want. But if you blame me for your suffering, that’s on you.

---

## Author

Built by someone who decided that pain = progress.

