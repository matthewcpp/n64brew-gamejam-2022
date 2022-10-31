#include "zombie_control.h"

#include "framework64/random.h"

#include "assets/layers.h"

#include <string.h>

void zombie_control_init(ZombieControl* control, Player* player, ZombieSpawner* spawner){
    memset(control, 0, sizeof(ZombieControl));
    control->player = player;
    control->spawner = spawner;
}

ZombieSpawnInfo* zombie_control_get_info(ZombieControl* control, Vec3* point) {
    for (uint32_t i = 0; i < control->count; i++) {
        ZombieSpawnInfo* info = &control->infos[i];

        if (box_contains_point(fw64_scene_get_initial_bounds(info->ref->scene), point)) {
            return info;
        }
    }

    return NULL;
}

void zombie_control_add_ref(ZombieControl* control, fw64LevelChunckRef* ref) {
    if (control->count >= INFOS_CAPACITY)
        return;

    ZombieSpawnInfo* info = &control->infos[control->count++];
    memset(info, 0, sizeof(ZombieSpawnInfo));

    info->ref = ref;
    info->count = fw64_scene_find_nodes_with_layer_mask(ref->scene, FW64_layer_pickups, &info->nodes[0], MAX_SPAWN_NODE_COUNT);

}
void zombie_control_remove_ref(ZombieControl* control, fw64LevelChunckRef* ref){
    for (uint32_t i = 0; i < control->count; i++) {
        ZombieSpawnInfo* info = &control->infos[i];

        if (info->ref = ref) {
            ZombieSpawnInfo* swap = &control->infos[control->count - 1];
            memcpy(info, swap, sizeof(ZombieSpawnInfo));
            control->count -= 1;
            return;
        }
    }
}

#define SPAWN_DIST_SQ (30.0f * 30.0f)

void zombie_control_update(ZombieControl* control) {
    float closest_distance = FLT_MAX;
    int node_index;
    ZombieSpawnInfo* closest_info = NULL;

    for (uint32_t i = 0; i < control->count; i++) {
        ZombieSpawnInfo* current_info = &control->infos[i];

        for (uint32_t j = 0; j < current_info->count; j++) {
            if (current_info->spawned[j])
                continue;

            fw64Node* current_node = current_info->nodes[j];

            float sq_dist = vec3_distance_squared(&control->player->node->transform.position, &current_node->transform.position);

            if (sq_dist <= SPAWN_DIST_SQ && sq_dist < closest_distance) {
                closest_distance = sq_dist;
                closest_info = current_info;
                node_index = j;
            }
        }
    }

    if (closest_info) {
        fw64Node* target_node = closest_info->nodes[node_index];
        closest_info->spawned[node_index] = 1;

        zombie_spawner_spawn_at_pos(control->spawner, fw64_random_int_in_range(3,5), &target_node->transform.position, closest_info->ref->handle);
    }
}