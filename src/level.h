#pragma once

/** \file level.h */

#include "framework64/scene.h"

#include <stdint.h>

#define LEVEL_DYNAMIC_NODE_SIZE 8

typedef struct {
    fw64Scene* scene;
    fw64Node* dynamic_nodes[LEVEL_DYNAMIC_NODE_SIZE];
    uint32_t dynamic_node_count;
} fw64Level;

void fw64_level_init(fw64Level* level);
void fw64_level_add_dyanmic_node(fw64Level* level, fw64Node* node);
void fw64_level_remove_dynamic_node(fw64Level* level, fw64Node* node);
uint32_t fw64_level_get_dynamic_node_count(fw64Level* level);
fw64Node* fw64_level_get_dynamic_node(fw64Level* level, uint32_t index);