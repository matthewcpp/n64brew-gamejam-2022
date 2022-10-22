#include "pickups.h"

#include "weapon.h"
#include "assets/assets.h"
#include "assets/layers.h"

#include <string.h>

static int process_pickup(Pickups* pickups, Pickup* pickup);
static int get_pickup_amount(PickupType pickup_type);

void pickups_init(Pickups* pickups, fw64Engine* engine, Player* player, fw64Allocator* allocator) {
    pickups->player = player;
    pickups->engine = engine;
    pickups->allocator = allocator;
    pickups->item_count = 0;

    pickups_set_callback(pickups, NULL, NULL);

    memset(&pickups->items[0], 0, sizeof(Pickup) * MAX_PICKUP_COUNT);

    pickups->meshes[PICKUP_TYPE_NONE] = NULL;

    pickups->meshes[PICKUP_TYPE_SHOTGUN_AMMO] = fw64_mesh_load(engine->assets, FW64_ASSET_mesh_shotgun_pickup, allocator);
    pickups->meshes[PICKUP_TYPE_MAX_SHOTGUN_AMMO] = pickups->meshes[PICKUP_TYPE_SHOTGUN_AMMO];

    pickups->meshes[PICKUP_TYPE_UZI_AMMO] = fw64_mesh_load(engine->assets, FW64_ASSET_mesh_uzi_pickup, allocator);
    pickups->meshes[PICKUP_TYPE_MAX_UZI_AMMO] = pickups->meshes[PICKUP_TYPE_UZI_AMMO];

    pickups->meshes[PICKUP_TYPE_AR15_AMMO] = fw64_mesh_load(engine->assets, FW64_ASSET_mesh_ar15_pickup, allocator);
    pickups->meshes[PICKUP_TYPE_MAX_AR15_AMMO] = pickups->meshes[PICKUP_TYPE_AR15_AMMO];
}

void pickups_uninit(Pickups* pickups) {
    fw64_mesh_delete(pickups->engine->assets, pickups->meshes[PICKUP_TYPE_SHOTGUN_AMMO], pickups->allocator);
    fw64_mesh_delete(pickups->engine->assets, pickups->meshes[PICKUP_TYPE_UZI_AMMO], pickups->allocator);
    fw64_mesh_delete(pickups->engine->assets, pickups->meshes[PICKUP_TYPE_AR15_AMMO], pickups->allocator);
}

void pickups_set_callback(Pickups* pickups, PickupCallback callback, void* arg) {
    pickups->callback = callback;
    pickups->callback_arg = arg;
}

int pickups_add(Pickups* pickups, PickupType weapon_type, uint32_t amount, fw64Node* node) {
    if (pickups->item_count >= MAX_PICKUP_COUNT)
        return 0;

    Pickup* pickup = &pickups->items[pickups->item_count];

    pickup->type = weapon_type;
    pickup->amount = amount;
    pickup->node = node;

    pickups->item_count += 1;

    return 1;
}

void pickups_add_from_scene(Pickups* pickups, fw64Scene* scene) {
    fw64Node* pickup_nodes[MAX_PICKUP_COUNT];
    uint32_t pickup_nodes_count = fw64_scene_find_nodes_with_layer_mask(scene, FW64_layer_pickups, &pickup_nodes[0], MAX_PICKUP_COUNT);

    for (uint32_t i = 0; i < pickup_nodes_count; i++) {
        fw64Node* node = pickup_nodes[i];
        PickupType pickup_type = (PickupType)node->data;

        if (!pickups_add(pickups, pickup_type, get_pickup_amount(pickup_type), node))
            return;

        fw64_node_set_mesh(node, pickups->meshes[pickup_type]);
    }
}

static int pickup_ammo(Player* player, WeaponType weapon_type) {
    WeaponInfo* weapon_info = weapon_get_info(weapon_type);
    return player_pickup_ammo(player, WEAPON_TYPE_SHOTGUN, weapon_info->mag_size);
}

static int pickup_max_ammo(Player* player, WeaponType weapon_type) {
    WeaponInfo* weapon_info = weapon_get_info(weapon_type);
    return player_pickup_ammo(player, weapon_type, weapon_info->mag_size + weapon_info->max_additional_rounds);
}

void pickups_update(Pickups* pickups) {
    for (uint32_t i = 0; i < pickups->item_count; i++) {
        Pickup* pickup = &pickups->items[i];
        float dist_squared = vec3_distance_squared(&pickup->node->transform.position, &pickups->player->node->transform.position);
        
        if (dist_squared <= PICKUP_DISTANCE_SQ) {
            int picked_up = process_pickup(pickups, pickup);

            if (!picked_up) 
                continue;

            if (pickups->callback)
                pickups->callback(pickup, pickups->callback_arg);

            fw64_node_set_mesh(pickup->node, NULL);
            Pickup* swap = &pickups->items[pickups->item_count - 1];
            memcpy(pickup, swap, sizeof(Pickup));
            pickups->item_count -= 1;
        }
    }
}

void pickups_draw(Pickups* weapon_pickups) {
    for (uint32_t i = 0; i < weapon_pickups->item_count; i++) {
        fw64_node_billboard(weapon_pickups->items[i].node, &weapon_pickups->player->movement.camera);
    }
}

int get_pickup_amount(PickupType pickup_type) {
    WeaponInfo* weapon_info;
    switch (pickup_type) {
        case PICKUP_TYPE_SHOTGUN_AMMO:
            weapon_info = weapon_get_info(WEAPON_TYPE_SHOTGUN);
            return weapon_info->mag_size;

        case PICKUP_TYPE_MAX_SHOTGUN_AMMO:
            weapon_info = weapon_get_info(WEAPON_TYPE_SHOTGUN);
            return weapon_info->mag_size + weapon_info->max_additional_rounds;

        case PICKUP_TYPE_UZI_AMMO:
            weapon_info = weapon_get_info(WEAPON_TYPE_UZI);
            return weapon_info->mag_size;

        case PICKUP_TYPE_MAX_UZI_AMMO:
            weapon_info = weapon_get_info(WEAPON_TYPE_UZI);
            return weapon_info->mag_size + weapon_info->max_additional_rounds;

        case PICKUP_TYPE_AR15_AMMO:
            weapon_info = weapon_get_info(WEAPON_TYPE_AR15);
            return weapon_info->mag_size;

        case PICKUP_TYPE_MAX_AR15_AMMO:
            weapon_info = weapon_get_info(WEAPON_TYPE_AR15);
            return weapon_info->mag_size + weapon_info->max_additional_rounds;

        case PICKUP_TYPE_NONE:
        case PICKUP_TYPE_COUNT:
            break;
    }

    return 0;
}

int process_pickup(Pickups* pickups, Pickup* pickup) {
    int picked_up = 0;
    int pickup_amount = get_pickup_amount(pickup->type);

    if (pickup_amount == 0)
        return 0;


    switch (pickup->type) {
        case PICKUP_TYPE_SHOTGUN_AMMO:
            picked_up = pickup_ammo(pickups->player, WEAPON_TYPE_SHOTGUN);
            break;

        case PICKUP_TYPE_MAX_SHOTGUN_AMMO:
            picked_up = pickup_max_ammo(pickups->player, WEAPON_TYPE_SHOTGUN);
            break;

        case PICKUP_TYPE_UZI_AMMO:
            picked_up = pickup_ammo(pickups->player, WEAPON_TYPE_UZI);
            break;

        case PICKUP_TYPE_MAX_UZI_AMMO:
            picked_up = pickup_max_ammo(pickups->player, WEAPON_TYPE_UZI);
            break;

        case PICKUP_TYPE_AR15_AMMO:
            picked_up = pickup_ammo(pickups->player, WEAPON_TYPE_AR15);
            break;

        case PICKUP_TYPE_MAX_AR15_AMMO:
            picked_up = pickup_max_ammo(pickups->player, WEAPON_TYPE_AR15);
            break;

        case PICKUP_TYPE_NONE:
        case PICKUP_TYPE_COUNT:
            break;
    }

    return picked_up;
}
