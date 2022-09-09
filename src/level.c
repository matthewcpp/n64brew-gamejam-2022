#include "level.h"

#include <string.h>

void fw64_level_init(fw64Level* level, fw64Engine* engine) {
    memset(level, 0, sizeof(fw64Level));
    level->engine = engine;
}

void fw64_level_delete(fw64Level* level) {
    fw64_scene_delete(level->engine->assets, level->scene, fw64_scene_get_allocator(level->scene));
}

fw64Scene* fw64_level_load_chunk(fw64Level* level, fw64AssetDatabase* assets, int handle, fw64Allocator* allocator) {
    level->scene = fw64_scene_load(assets, handle, allocator);

    return level->scene;
}

int fw64_level_raycast(fw64Level* level, Vec3* origin, Vec3* direction, uint32_t mask, fw64RaycastHit* hit) {
    return fw64_scene_raycast(level->scene, origin, direction, mask, hit);
}

int fw64_level_moving_box_intersection(fw64Level* level, Box* box, Vec3* velocity, uint32_t mask, fw64IntersectMovingBoxQuery* result) {
    return fw64_scene_moving_box_intersection(level->scene, box, velocity, mask, result);
}

void fw64_level_draw_camera(fw64Level* level, fw64Camera* camera) {
    fw64Renderer* renderer = level->engine->renderer;
    fw64Frustum frustum;
    fw64_camera_extract_frustum_planes(camera, &frustum);

    fw64_scene_draw_frustrum(level->scene, renderer, &frustum);
}

void fw64_level_add_dyanmic_node(fw64Level* level, fw64Node* node) {
    if (level->dynamic_node_count == LEVEL_DYNAMIC_NODE_SIZE)
        return;

    level->dynamic_nodes[level->dynamic_node_count++] = node;
}

void fw64_level_remove_dynamic_node(fw64Level* level, fw64Node* node) {
    for (int i = 0; i < level->dynamic_node_count; i++) {
        if (level->dynamic_nodes[i] == node) {
            level->dynamic_node_count -= 1;
            level->dynamic_nodes[i] = level->dynamic_nodes[level->dynamic_node_count];
            
            return;
        }
    }
}

uint32_t fw64_level_get_dynamic_node_count(fw64Level* level) {
    return level->dynamic_node_count;
}

fw64Node* fw64_level_get_dynamic_node(fw64Level* level, uint32_t index) {
    return level->dynamic_nodes[index];
}


