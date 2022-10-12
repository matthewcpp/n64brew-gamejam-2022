#include "weapon_bob.h"

#include "framework64/types.h"

#define WEAPON_BOB_SCALE_X 1.0f
#define WEAPON_BOB_SCALE_Y 1.0f
#define WEAPON_BOB_CYCLE_TIME 1.0f

void weapon_bob_init(WeaponBob* weapon_bob, InputMapping* input_mapping) {
    vec3_zero(&weapon_bob->translation);
    weapon_bob->input_mapping = input_mapping;
    weapon_bob->current_time = 0.0f;
}

void weapon_bob_update(WeaponBob* weapon_bob, float time_delta) {
    Vec2 stick;
    int active =    mapped_input_controller_read(weapon_bob->input_mapping, 0, INPUT_MAP_MOVE_RIGHT, &stick) ||
                    mapped_input_controller_read(weapon_bob->input_mapping, 0, INPUT_MAP_MOVE_LEFT, &stick) ||
                    mapped_input_controller_read(weapon_bob->input_mapping, 0, INPUT_MAP_MOVE_FORWARD, &stick) ||
                    mapped_input_controller_read(weapon_bob->input_mapping, 0, INPUT_MAP_MOVE_BACKWARD, &stick);

    if (!active)
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
