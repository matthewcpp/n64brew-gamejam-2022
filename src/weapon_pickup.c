#include "weapon_pickup.h"

#include <string.h>

void weapon_pickups_init(WeaponPickups* weapon_pickups, Player* player) {
    weapon_pickups->count = 0;
    weapon_pickups->player = player;
}

void weapon_pickups_add(WeaponPickups* weapon_pickups, WeaponType weapon_type, uint32_t amount, fw64Node* node) {
    WeaponPickup* weapon_pickup = &weapon_pickups->items[weapon_pickups->count];

    weapon_pickup->weapon_type = weapon_type;
    weapon_pickup->amount = amount;
    weapon_pickup->node = node;

    weapon_pickups->count += 1;
}

void weapon_pickups_update(WeaponPickups* weapon_pickups) {
    for (uint32_t i = 0; i < weapon_pickups->count; i++) {
        WeaponPickup* pickup = &weapon_pickups->items[i];
        float dist_squared = vec3_distance_squared(&pickup->node->transform.position, &weapon_pickups->player->node->transform.position);
        if (dist_squared <= 36.0f) {
            int picked_up = player_pickup_ammo(weapon_pickups->player, pickup->weapon_type, pickup->amount);

            if (picked_up) {
                fw64_node_set_mesh(pickup->node, NULL);
                WeaponPickup* swap = &weapon_pickups->items[weapon_pickups->count - 1];
                memcpy(pickup, swap, sizeof(WeaponPickup));
                weapon_pickups->count -= 1;
            }
        }
    }
}

void weapon_pickups_draw(WeaponPickups* weapon_pickups, fw64Renderer* renderer) {
    for (uint32_t i = 0; i < weapon_pickups->count; i++) {
        fw64_node_billboard(weapon_pickups->items[i].node, fw64_renderer_get_camera(renderer));
    }
}