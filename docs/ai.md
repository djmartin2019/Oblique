# AI System

The AI system provides a state machine and behavior functions that can be assigned to entities, allowing NPCs to exhibit different behaviors based on their current state.

## Architecture

The AI system uses a two-layer approach:

1. **AI Brain** - State machine that transitions between states (idle, wander, chase)
2. **Behavior Functions** - Actions executed each frame based on current state

## AI States

```c
typedef enum {
    STATE_IDLE,
    STATE_WANDER,
    STATE_CHASE
} AIState;
```

### State Transitions

The AI brain evaluates conditions and transitions between states:

```c
void npc_brain(Entity* self) {
    switch (self->state) {
        case STATE_IDLE:
            if (should_wander()) {
                self->state = STATE_WANDER;
                self->behavior = wander_behavior;
            }
            break;
        case STATE_WANDER:
            if (sees_player(self)) {
                self->state = STATE_CHASE;
                self->behavior = chase_behavior;
            }
            break;
        case STATE_CHASE:
            if (lost_player(self)) {
                self->state = STATE_IDLE;
                self->behavior = idle_behavior;
            }
            break;
    }
}
```

## Behavior Functions

Behaviors are function pointers that entities execute each frame:

```c
typedef void (*BehaviorFunc)(Entity* self);
```

### Idle Behavior

NPCs do nothing when idle:

```c
void idle_behavior(Entity* self) {
    // do nothing for now
}
```

### Wander Behavior

NPCs randomly move in different directions:

```c
void wander_behavior(Entity* self) {
    if (rand() % 100 < 2) {
        int dir = rand() % 4;
        if (dir == 0) self->x += 1;
        if (dir == 1) self->x -= 1;
        if (dir == 2) self->y += 1;
        if (dir == 3) self->y -= 1;
    }
}
```

### Chase Behavior

NPCs move toward the player:

```c
void chase_behavior(Entity* self) {
    Entity* player = get_player();
    if (!player) return;

    if (++chase_timer % 10 != 0) return; // Only moves every 10 ticks

    // Simple tile-by-tile movement towards player
    if (self->x < player->x) self->x++;
    else if (self->x > player->x) self->x--;

    if (self->y < player->y) self->y++;
    else if (self->y > player->y) self->y--;
}
```

### Player Behavior

The player uses keyboard input for movement:

```c
void player_behavior(Entity* self) {
    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    if (!keystates) return;

    if (keystates[SDL_SCANCODE_UP])     self->y -= 1;
    if (keystates[SDL_SCANCODE_DOWN])   self->y += 1;
    if (keystates[SDL_SCANCODE_LEFT])   self->x -= 1;
    if (keystates[SDL_SCANCODE_RIGHT])  self->x += 1;
}
```

## State Conditions

### Should Wander

NPCs occasionally decide to wander:

```c
int should_wander() { 
    return rand() % 200 == 0; 
}
```

### Sees Player

NPCs detect the player within a certain distance:

```c
int sees_player(Entity* self) {
    Entity* player = get_player();
    if (!player) return 0;
    
    int dx = abs(player->x - self->x);
    int dy = abs(player->y - self->y);
    return (dx + dy) <= 5; // Within distance of 5 tiles
}
```

### Lost Player

NPCs lose track of the player when they're too far:

```c
int lost_player(Entity* self) {
    Entity* player = get_player();
    if (!player) return 1;
    
    int dx = abs(player->x - self->x);
    int dy = abs(player->y - self->y);
    return (dx + dy) > 7; // Loses player when 7+ tiles away
}
```

## Entity Update Flow

Entities are updated in two phases:

1. **AI Brain Update** - NPCs evaluate state transitions
2. **Behavior Execution** - All entities (including player) execute their behavior function

```c
void update_entities() {
    for (int i = 0; i < entity_count; i++) {
        Entity* e = &entities[i];

        // Let the AI update its brain before acting
        if (!e->is_player) {
            npc_brain(e);
        }

        // Then run its behavior
        if (e->behavior) {
            e->behavior(e);
        }
    }
}
```

## Future Improvements

- More sophisticated pathfinding for chase behavior
- Line-of-sight calculations for player detection
- Alert states and investigation behaviors
- Group behaviors (flocking, formation)
- Scripted behavior system for custom AI patterns
