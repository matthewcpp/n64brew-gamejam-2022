#pragma once

#include "framework64/Vec3.h"
#include "mapped_input.h"

typedef struct {
    Vec3 translation;
    InputMapping* input_mapping;
    float current_time;
} WeaponBob;

void weapon_bob_init(WeaponBob* weapon_bob, InputMapping* input_mapping);
void weapon_bob_update(WeaponBob* weapon_bob, float time_delta);