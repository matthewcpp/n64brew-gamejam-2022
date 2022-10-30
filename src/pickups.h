#pragma once

#include "player.h"
#include "framework64/engine.h"

#define MAX_PICKUP_COUNT 8
#define PICKUP_DISTANCE_SQ 36.0f

typedef enum {
    PICKUP_TYPE_NONE,
    PICKUP_TYPE_SHOTGUN_AMMO,
    PICKUP_TYPE_MAX_SHOTGUN_AMMO,
    PICKUP_TYPE_UZI_AMMO,
    PICKUP_TYPE_MAX_UZI_AMMO,
    PICKUP_TYPE_AR15_AMMO,
    PICKUP_TYPE_MAX_AR15_AMMO,
    PICKUP_TYPE_COUNT
} PickupType;

typedef struct {
    PickupType type;
    uint32_t amount;
    fw64Node* node;
} Pickup;

typedef void(*PickupCallback)(Pickup* pickup, void* arg);

typedef struct {
    fw64Engine* engine;
    Player* player;
    fw64Allocator* allocator;
    fw64Mesh* meshes[PICKUP_TYPE_COUNT];
    Pickup items[MAX_PICKUP_COUNT];
    uint32_t item_count;
    PickupCallback callback;
    void* callback_arg;
} Pickups;

void pickups_init(Pickups* pickups, fw64Engine* engine, Player* player, fw64Allocator* allocator);
void pickups_uninit(Pickups* pickups);

void pickups_set_callback(Pickups* pickups, PickupCallback callback, void* arg);

int pickups_add(Pickups* pickups, PickupType type, uint32_t amount, fw64Node* node);
void pickups_add_from_scene(Pickups* pickups, fw64Scene* scene);
void pickups_remove_from_scene(Pickups* pickups, fw64Scene* scene);

void pickups_update(Pickups* pickups);
void pickups_draw(Pickups* pickups);
