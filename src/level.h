#pragma once

/** \file level.h */

#include "framework64/engine.h"
#include "framework64/scene.h"
#include "framework64/util/bump_allocator.h"

#include <stdint.h>

#define LEVEL_DYNAMIC_NODE_SIZE 8

typedef struct {
    fw64Engine* engine;
    fw64Scene* scene;
    fw64Node* dynamic_nodes[LEVEL_DYNAMIC_NODE_SIZE];
    uint32_t dynamic_node_count;
} fw64Level;

void fw64_level_init(fw64Level* level, fw64Engine* engine);
fw64Scene* fw64_level_load_chunk(fw64Level* level, fw64AssetDatabase* assets, int handle, fw64Allocator* allocator);

int fw64_level_raycast(fw64Level* level, Vec3* origin, Vec3* direction, uint32_t mask, fw64RaycastHit* hit);
int fw64_level_moving_box_intersection(fw64Level* level, Box* box, Vec3* velocity, uint32_t mask, fw64IntersectMovingBoxQuery* result);

void fw64_level_draw_camera(fw64Level* level, fw64Camera* camera);

void fw64_level_add_dyanmic_node(fw64Level* level, fw64Node* node);
void fw64_level_remove_dynamic_node(fw64Level* level, fw64Node* node);
uint32_t fw64_level_get_dynamic_node_count(fw64Level* level);
fw64Node* fw64_level_get_dynamic_node(fw64Level* level, uint32_t index);