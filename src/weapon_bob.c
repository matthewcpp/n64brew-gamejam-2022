#include "weapon_bob.h"

#include "framework64/types.h"

#include <string.h>

#define WEAPON_BOB_SCALE_X 1.0f
#define WEAPON_BOB_SCALE_Y 1.0f
#define WEAPON_BOB_CYCLE_TIME 1.0f

void weapon_bob_init(WeaponBob* weapon_bob) {
    memset(weapon_bob, 0, sizeof(WeaponBob));
}

void weapon_bob_update(WeaponBob* weapon_bob, float time_delta) {
    if (!weapon_bob->is_active)
        return;

    weapon_bob->current_time += time_delta;
    if (weapon_bob->current_time >= WEAPON_BOB_CYCLE_TIME)
        weapon_bob->current_time -= WEAPON_BOB_CYCLE_TIME;

    float t = (weapon_bob->current_time / WEAPON_BOB_CYCLE_TIME) * (M_PI * 2.0f);
    weapon_bob->translation.x = fw64_cosf(t) * WEAPON_BOB_SCALE_X;
    weapon_bob->translation.y = fw64_cosf(t) * WEAPON_BOB_SCALE_Y;

    if (weapon_bob->translation.y > 0)
        weapon_bob->translation.y *= -1;
}
