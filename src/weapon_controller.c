#include "weapon_controller.h"

#include "framework64/math.h"
#include "framework64/n64/controller_button.h"

#define WEAPON_CONTROLLER_PICKUP_SPEED 1.0f

static void weapon_controller_fire(WeaponController* controller);

void weapon_controller_init(WeaponController* controller, fw64Engine* engine, int controller_index) {
    controller->engine = engine;
    controller->controller_index = controller_index;
    controller->trigger_button = FW64_N64_CONTROLLER_BUTTON_Z;
    controller->weapon = NULL;

    fw64_transform_init(&controller->weapon_transform);
}

void weapon_controller_update(WeaponController* controller) {
    if (controller->weapon == NULL)
        return;

    if (controller->time_to_next_fire > 0.0f) {
        controller->time_to_next_fire -= controller->engine->time->time_delta;
    }

    if (controller->time_to_next_fire <= 0.0f && fw64_input_controller_button_down(controller->engine->input, controller->controller_index, controller->trigger_button)) {
        weapon_controller_fire(controller);
    }
}

void weapon_controller_set_weapon(WeaponController* controller, Weapon* weapon) {
    controller->weapon = weapon;

    controller->weapon_transform.position = weapon->default_position;
    controller->weapon_transform.scale = weapon->default_scale;

    fw64_transform_update_matrix(&controller->weapon_transform);
}

static void weapon_controller_fire(WeaponController* controller) {
    fw64_audio_play_sound(controller->engine->audio, controller->weapon->gunshot_sound);

    controller->time_to_next_fire = controller->weapon->fire_rate;
}