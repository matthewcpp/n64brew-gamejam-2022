#pragma once

#include "framework64/Vec3.h"
#include "mapped_input.h"

typedef struct {
    Vec3 translation;
    int is_active;
    float current_time;
    float step_speed;
} WeaponBob;

void weapon_bob_init(WeaponBob* weapon_bob);
void weapon_bob_update(WeaponBob* weapon_bob, float time_delta, float move_speed);