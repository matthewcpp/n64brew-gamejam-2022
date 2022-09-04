#pragma once

#include "framework64/engine.h"

#include "weapon.h"
#include "level.h"

typedef enum {
    WEAPON_CONTROLLER_HOLDING,
    WEAPON_CONTROLLER_LOWERING,
    WEAPON_CONTROLLER_LOWERED,
    WEAPON_CONTROLLER_RAISING,
} WeaponControllerState;

typedef void(*WeaponTransitionFunc)(Weapon*, WeaponControllerState, void*);

typedef struct {
    fw64Engine* engine;
    fw64Level* level;
    int controller_index;
    int trigger_button;
    float time_to_next_fire;
    Weapon* weapon;
    fw64Transform weapon_transform;
    fw64Transform casing_transform;
    WeaponControllerState state;
    float transition_time;
    WeaponTransitionFunc transition_callback;
    void* transition_arg;
} WeaponController;

void weapon_controller_init(WeaponController* controller, fw64Engine* engine, fw64Level* level, int controller_index);
void weapon_controller_update(WeaponController* controller);
void weapon_controller_set_weapon(WeaponController* controller, Weapon* weapon);
int weapon_controller_raise_weapon(WeaponController* controller, WeaponTransitionFunc callback , void* arg);
int weapon_controller_lower_weapon(WeaponController* controller, WeaponTransitionFunc callback, void* arg);