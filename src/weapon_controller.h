#pragma once

#include "framework64/engine.h"

#include "weapon.h"

typedef struct {
    fw64Engine* engine;
    int controller_index;
    int trigger_button;
    float time_to_next_fire;
    Weapon* weapon;
    fw64Transform weapon_transform;
} WeaponController;

void weapon_controller_init(WeaponController* controller, fw64Engine* engine, int controller_index);
void weapon_controller_update(WeaponController* controller);
void weapon_controller_set_weapon(WeaponController* controller, Weapon* weapon);