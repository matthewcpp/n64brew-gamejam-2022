#pragma once

#include "framework64/engine.h"
#include "framework64/mesh.h"
#include "framework64/quat.h"
#include "framework64/scene.h"
#include "framework64/vec3.h"
#include "movement_controller.h"

#include "framework64/level.h"
#include "audio_controller.h"
#include "weapon_controller.h"
#include "ray.h"
#include "mapped_input.h"

typedef struct {
    fw64Engine* engine;
    fw64Allocator* allocator;
    fw64Level* level;
    fw64Node* node;
    MovementController movement;
    WeaponController weapon_controller;
    InputMapping input_map;
    Ray aim;    
} Player;

void player_init(Player* player, fw64Engine* engine, fw64Level* level, ProjectileController* projectile_controller, AudioController* audio_controller, fw64Allocator* allocator);
void player_uninit(Player* player);
void player_update(Player* player);
void player_draw(Player* player);
void player_draw_weapon(Player* player);

/** player attempts to pick up ammo.  will return false if their inventory is maxed out. */
int player_pickup_ammo(Player* player, WeaponType weapon_type, uint32_t amount);

void player_set_weapon(Player* player, WeaponType weapon_type);
void player_set_position(Player* player, Vec3* position);
