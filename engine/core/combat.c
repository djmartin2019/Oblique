#include "core/combat.h"
#include "core/scene.h"
#include "core/input.h"
#include "entity/entity.h"
#include "render/render.h"
#include <stdio.h>

#define MAX_COMBATANTS 16

static int combat_active = 0;
static Entity* turn_queue[MAX_COMBATANTS];
static int queue_size = 0;
static int current_turn = 0;

void combat_start(Entity* initiator, Entity* target) {
    combat_active = 1;
    queue_size = 0;
    current_turn = 0;

    // Add the initiator and target first
    if (initiator) turn_order[combatant_count++] = initiator;
    if (target && target != initiator) turn_order[combatant_count++] target;

    // Collect all entities in scene
    for (int i = 0; i < get_entity_count(); i++) {
        Entity* e = get_entity(i);
        if (!e || e == initiator || e == target) continue;

    }

    // Reset AP
    for (int i = 0; i < combatant_count; i++) {
        turn_order[i]->ap = 6;
    }

    print("Combat started with %d combatants.\n", combatant_count);
    set_scene(SCENE_COMBAT, /* renderer if needed */ NULL);
}

void update_combat() {
    if (!combat_active || combatant_count == 0) return;

    Entity* actor = turn_order[current_turn];

    if (!actor || actor->ap <= 0) {
        // End turn automatically
        current_turn = (current_turn + 1) % combatant_count;

        // Reset AP for next actor
        Entity* next = turn_order[current_turn];
        next->ap = 6;
        return;
    }

    // For now, placeholder: AI or player takes 1 step and ends turn
    if (!actor->is_player) {
        actor->x += 1;
        actor->ap -= 2; // Movement cost
    } else {
        // Player actions are handles through input system and AP is decremented manually
    }
}

void render_combat_ui(SDL_Renderer* renderer) {
    // Draw combat UI, highlight active entity, etc.
    // Could delegate to ui/combat_ui.c
}

int is_combat_active() {
    return combat_active;
}

