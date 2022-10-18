#include "interact.h"

#include <string.h>

void interaction_init(Interaction* interaction, fw64Level* level, fw64Transform* target, uint32_t layer_mask) {
    interaction->level = level;
    interaction->target = target;
    interaction->layer_mask = layer_mask;
    
    interaction->interesting_node = NULL;
    interaction->active_scene = NULL;
    memset(&interaction->interactables[0], 0, sizeof(fw64Node) * MAX_INTERACTABLE_SIZE);
    interaction->interactables_count = 0;
}

static fw64Scene* get_active_scene(Interaction* interaction) {
    if (interaction->active_scene && box_contains_point(fw64_scene_get_initial_bounds(interaction->active_scene), &interaction->target->position)) {
        return interaction->active_scene;
    }

    uint32_t chunk_count = fw64_level_get_chunk_count(interaction->level);

    for (uint32_t i = 0; i < chunk_count; i++) {
        fw64Scene* scene = fw64_level_get_chunk_by_index(interaction->level, i);

        if (box_contains_point(fw64_scene_get_initial_bounds(scene), &interaction->target->position)) {
            return scene;
        }
    }

    return NULL;
}

// note this is a very basic implementation but is probably fine for the purposes of this game jam
void interaction_update(Interaction* interaction) {
    fw64Scene* current_scene = get_active_scene(interaction);

    if (current_scene != interaction->active_scene) {
        interaction->active_scene = current_scene;

        if (interaction->active_scene == NULL)
            return;

        interaction->interactables_count = fw64_scene_find_nodes_with_layer_mask(interaction->active_scene, interaction->layer_mask, &interaction->interactables[0], MAX_INTERACTABLE_SIZE);
    }

    float closest_dist = FLT_MAX;
    interaction->interesting_node = NULL;

    for (uint32_t i = 0; i < interaction->interactables_count; i++) {
        fw64Node* node = interaction->interactables[i];
        float dist_sq = vec3_distance_squared(&interaction->target->position, &node->transform.position);

        if (dist_sq < INTERACTION_DISTANCE_SQ && dist_sq < closest_dist) {
            interaction->interesting_node = node;
            closest_dist = dist_sq;
        }
    }
}