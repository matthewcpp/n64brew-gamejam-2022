#include "weapon_bob.h"

#include "framework64/types.h"
#include "framework64/math.h"

#include <string.h>

#define WEAPON_BOB_SCALE_X 0.20f
#define WEAPON_BOB_SCALE_Y 0.15f
#define WEAPON_BOB_CYCLE_TIME 0.75f
#define MAX_MOVE_SPEED 40.0f // hack. should equal default move speed of movement controller ( x 5 for some reason)

void weapon_bob_init(WeaponBob* weapon_bob) {
    memset(weapon_bob, 0, sizeof(WeaponBob));
}

void weapon_bob_update(WeaponBob* weapon_bob, float time_delta, float move_speed) {   
    weapon_bob->step_speed = move_speed;
    if (!weapon_bob->is_active) {
        if(fw64_fabsf(weapon_bob->current_time) > EPSILON) {
            weapon_bob->current_time *= 0.5f;
            weapon_bob->translation.x *= 0.5f;
            weapon_bob->translation.y *= 0.5f;
        }
        return;
    }
    
    weapon_bob->current_time += time_delta * (move_speed / MAX_MOVE_SPEED);
    if (weapon_bob->current_time >= WEAPON_BOB_CYCLE_TIME) {
        weapon_bob->current_time -= WEAPON_BOB_CYCLE_TIME;
    }

    float t = (M_PI * 0.5f) + (weapon_bob->current_time / WEAPON_BOB_CYCLE_TIME) * (M_PI * 2.0f);
    weapon_bob->translation.x = ((fw64_cosf(t)) * WEAPON_BOB_SCALE_X);
    weapon_bob->translation.y = -fw64_fabsf(fw64_cosf(t) * WEAPON_BOB_SCALE_Y);
}
