#pragma once

#include "framework64/level.h"

#define MAX_INTERACTABLE_SIZE 10
#define INTERACTION_DISTANCE_SQ 80.0f

typedef struct {
    fw64Level* level;
    fw64Transform* target;
    fw64Node* interesting_node;
    uint32_t node_uid;
    uint32_t layer_mask;
    fw64Scene* active_scene;
    fw64Node* interactables[MAX_INTERACTABLE_SIZE];
    uint32_t interactables_count;
} Interaction;

void interaction_init(Interaction* interaction, fw64Level* level, fw64Transform* target, uint32_t layer_mask);
void interaction_update(Interaction* interaction);