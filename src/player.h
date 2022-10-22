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
#include "weapon_bob.h"

#define PLAYER_MAX_HEALTH 100

typedef struct {
    fw64Engine* engine;
    fw64Allocator* allocator;
    fw64Level* level;
    fw64Node* node;
    WeaponBob weapon_bob;
    MovementController movement;
    WeaponController weapon_controller;
    InputMapping input_map;
    Ray aim;
    int current_health;
    float damage_overlay_time, damage_overlay_initial_time;
} Player;

/**
 * Player will make some small allocations from the level allocator to dynamically create it's node and collider.
 * The allocator will be passed to the weapon controller so that it can manage memory for the FPS assets.
 */
void player_init(Player* player, fw64Engine* engine, fw64Level* level, ProjectileController* projectile_controller, AudioController* audio_controller, fw64Allocator* allocator);
void player_uninit(Player* player);
void player_update(Player* player);
void player_draw(Player* player);
void player_draw_weapon(Player* player);
void player_draw_damage(Player* player);

/** 
 * player attempts to pick up ammo.  will return false if their inventory is maxed out.
 * note this will also play a weapon animation and pickup sound.
 * so simply add ammo to the player use \ref player_add_ammo
 * */
int player_pickup_ammo(Player* player, WeaponType weapon_type, uint32_t amount);
int player_add_ammo(Player* player, WeaponType weapon_type, uint32_t amount);

void player_set_weapon(Player* player, WeaponType weapon_type);
void player_set_position(Player* player, Vec3* position);
void player_take_damage(Player* player, int amount);
int player_is_interacting(Player* player);