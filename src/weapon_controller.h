#pragma once

#include "framework64/engine.h"
#include "framework64/util/bump_allocator.h"

#include "weapon.h"
#include "framework64/level.h"
#include "ray.h"
#include "mapped_input.h"
#include "projectile_controller.h"
#include "audio_controller.h"
#include "weapon_bob.h"

typedef enum {
    WEAPON_CONTROLLER_HOLDING,
    WEAPON_CONTROLLER_LOWERING,
    WEAPON_CONTROLLER_LOWERED,
    WEAPON_CONTROLLER_RAISING,
} WeaponControllerState;

typedef enum {
    WEAPON_RECOIL_INACTIVE,
    WEAPON_RECOIL_RECOILING,
    WEAPON_RECOIL_RECOVERING
} RecoilState;

typedef struct {
    uint32_t current_mag_count;
    uint32_t additional_rounds_count;
} WeaponAmmo;

typedef void(*WeaponTransitionFunc)(Weapon*, WeaponControllerState, void*);

typedef struct {
    fw64Engine* engine;
    WeaponBob* weapon_bob;
    ProjectileController* projectile_controller;
    AudioController* audio_controller;
    fw64BumpAllocator weapon_allocator;
    fw64Camera weapon_camera;
    Ray* aim;
    
    int controller_index;
    InputMapping* input_map;

    WeaponControllerState state;
    WeaponAmmo weapon_ammo[WEAPON_COUNT];

    float time_to_next_fire;
    float muzzle_flash_time_remaining;
    float recoil_time;
    RecoilState recoil_state;
    int is_dry_firing;

    Weapon weapon;
    fw64Transform weapon_transform;
    fw64Transform casing_transform;
    fw64Transform muzzle_flash_transform;
    
    float transition_time;
    WeaponTransitionFunc transition_callback;
    void* transition_arg;
} WeaponController;

/**
 * Note: the weapon controller will allocate a chunk of memory from the passed in player allocator
 * this memory is used to manage the weapon assets: Gun + shell casing + muzzle flash
 * Every time the player switches a weapon the bump allocator will be reset and new gun's assets DMA'd from ROM
 */
void weapon_controller_init(WeaponController* controller, fw64Engine* engine, WeaponBob* weapon_bob, ProjectileController* projectile_controller, AudioController* audio_controller, fw64Allocator* player_allocator, InputMapping* input_map, int controller_index);
void weapon_controller_uninit(WeaponController* controller);
void weapon_controller_update(WeaponController* controller);
void weapon_controller_draw(WeaponController* controller);
void weapon_controller_set_weapon(WeaponController* controller, WeaponType weapon_type);
int weapon_controller_raise_weapon(WeaponController* controller, WeaponTransitionFunc callback , void* arg);
int weapon_controller_lower_weapon(WeaponController* controller, WeaponTransitionFunc callback, void* arg);
WeaponAmmo* weapon_controller_get_current_weapon_ammo(WeaponController* controller);

void weapon_controller_switch_to_next_weapon(WeaponController* controller);
void weapon_controller_reload_current_weapon(WeaponController* controller);

/** 
 * immediately sets the ammo counts for the specified weapon.
 * use \ref weapon_controller_reload to perform a weapon reload action
 */
void weapon_controller_set_weapon_ammo(WeaponController* controller, WeaponType weapon_type, uint32_t current_mag_count, uint32_t additional_rounds_count);

/**
 * immediately refills the current weapon's magazine from extra carried ammo.
 * use \ref weapon_controller_reload to perform a weapon reload action
 */
void weapon_controller_refill_weapon_magazine(WeaponController* controller, WeaponType weapon_type);