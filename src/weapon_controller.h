#pragma once

#include "framework64/engine.h"

#include "weapon.h"
#include "framework64/level.h"
#include "ray.h"
#include "mapped_input.h"
#include "projectile_controller.h"

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

typedef void(*WeaponTransitionFunc)(Weapon*, WeaponControllerState, void*);

typedef struct {
    fw64Engine* engine;
    fw64Level* level;
    ProjectileController* projectile_controller;
    fw64Camera weapon_camera;
    Ray* aim;
    int controller_index;
    InputMapping* input_map;
    float time_to_next_fire;
    float muzzle_flash_time_remaining;
    float recoil_time;
    RecoilState recoil_state;
    Weapon* weapon;
    fw64Transform weapon_transform;
    fw64Transform casing_transform;
    fw64Transform muzzle_flash_transform;
    WeaponControllerState state;
    float transition_time;
    WeaponTransitionFunc transition_callback;
    void* transition_arg;
} WeaponController;

void weapon_controller_init(WeaponController* controller, fw64Engine* engine, fw64Level* level, InputMapping* input_map, ProjectileController* projectile_controller, int controller_index);
void weapon_controller_update(WeaponController* controller);
void weapon_controller_set_weapon(WeaponController* controller, Weapon* weapon);
int weapon_controller_raise_weapon(WeaponController* controller, WeaponTransitionFunc callback , void* arg);
int weapon_controller_lower_weapon(WeaponController* controller, WeaponTransitionFunc callback, void* arg);