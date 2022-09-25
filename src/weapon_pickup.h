#pragma once

#include "player.h"
#include "weapon.h"
#include "framework64/node.h"

typedef struct {
    WeaponType weapon_type;
    uint32_t amount;
    fw64Node* node;
} WeaponPickup;

void weapon_pickup_init(WeaponPickup* pickup, WeaponType weapon_type, uint32_t amount, fw64Node* node);

#define WEAPON_PICKUP_COUNT 4

typedef struct {
    WeaponPickup items[WEAPON_PICKUP_COUNT];
    uint32_t count;
    Player* player;
} WeaponPickups;

void weapon_pickups_init(WeaponPickups* weapon_pickups, Player* player);
void weapon_pickups_add(WeaponPickups* weapon_pickups, WeaponType weapon_type, uint32_t amount, fw64Node* node);
void weapon_pickups_update(WeaponPickups* weapon_pickups);