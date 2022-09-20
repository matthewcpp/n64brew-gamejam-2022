#include "weapon_controller.h"

#include "framework64/math.h"
#include "framework64/n64/controller_button.h"

#include "zombie.h"

#define WEAPON_CONTROLLER_TRANSITION_SPEED 0.50f
#define WEAPON_CONTROLLER_MUZZLE_FLASH_TIME 0.1f

static void weapon_controller_fire(WeaponController* controller);

void weapon_controller_init(WeaponController* controller, fw64Engine* engine, fw64Level* level, InputMapping* input_map, ProjectileController* projectile_controller, int controller_index) {
    controller->engine = engine;
    controller->level = level;
    controller->projectile_controller = projectile_controller;
    controller->controller_index = controller_index;
    controller->input_map = input_map;
    controller->weapon = NULL;
    controller->state = WEAPON_CONTROLLER_HOLDING;
    controller->transition_time = 0.0f;
    controller->muzzle_flash_time_remaining = 0.0f;
    controller->recoil_time = 0.0f;
    controller->recoil_state = WEAPON_RECOIL_INACTIVE;

    fw64_camera_init(&controller->weapon_camera);
    vec3_zero(&controller->weapon_camera.transform.position);
    fw64_camera_update_view_matrix(&controller->weapon_camera);

    controller->weapon_camera.near = 1.0f;
    controller->weapon_camera.far = 125.0f;
    controller->weapon_camera.fovy = 60.0f;
    fw64_camera_update_projection_matrix(&controller->weapon_camera);

    fw64_transform_init(&controller->weapon_transform);
    fw64_transform_init(&controller->casing_transform);
    fw64_transform_init(&controller->muzzle_flash_transform);
}

/** shell casing ejection is modeled via a simple quadratic equation*/
static void weapon_controller_update_casing(WeaponController* controller) {
    float fly_time = controller->weapon->fire_rate - controller->time_to_next_fire;

    float x = fly_time;
    float y = (-((0.5f * x - 1.5f) * (0.5f * x - 1.5f)) + 2.0f) * 0.5f;

    float x_scale = 16.0f;
    float y_scale = 8.5f;

    controller->casing_transform.position = controller->weapon->ejection_port_pos;
    controller->casing_transform.position.x += x * x_scale;
    controller->casing_transform.position.x += y * y_scale;

    fw64_transform_update_matrix(&controller->casing_transform);
}

static void weapon_controller_update_muzzle_flash(WeaponController* controller) {
    if (controller->muzzle_flash_time_remaining <= 0)
        return;

    controller->muzzle_flash_transform.position = controller->weapon_transform.position;
    controller->muzzle_flash_transform.rotation = controller->weapon_transform.rotation;

    fw64_transform_update_matrix(&controller->muzzle_flash_transform);

    controller->muzzle_flash_time_remaining -= controller->engine->time->time_delta;
}

static void weapon_controller_update_recoil(WeaponController* controller) {
    Weapon* weapon = controller->weapon;

    if (controller->recoil_state == WEAPON_RECOIL_INACTIVE)
        return;

    controller->recoil_time += controller->engine->time->time_delta;

    if (controller->recoil_time >= weapon->recoil_time) {
        if (controller->recoil_state == WEAPON_RECOIL_RECOILING) {
            controller->recoil_time -= weapon->recoil_time;
            controller->recoil_state = WEAPON_RECOIL_RECOVERING;
        }
        else { // recoil is finished
            controller->recoil_state = WEAPON_RECOIL_INACTIVE;
            controller->recoil_time = 0.0f;
            controller->weapon_transform.position = weapon->default_position;
            controller->weapon_transform.rotation = weapon->default_rotation;
        }
    }

    float smoothed_time = fw64_smoothstep(0.0f, 1.0f, controller->recoil_time / weapon->recoil_time);

    if (controller->recoil_state == WEAPON_RECOIL_RECOILING) {
        vec3_lerp(&controller->weapon_transform.position, &weapon->default_position, &weapon->recoil_pos, smoothed_time);
        quat_slerp(&controller->weapon_transform.rotation, &weapon->default_rotation, &weapon->recoil_rotation, smoothed_time);
    }
    else if (controller->recoil_state == WEAPON_RECOIL_RECOVERING) {
        vec3_lerp(&controller->weapon_transform.position, &weapon->recoil_pos, &weapon->default_position, smoothed_time);
        quat_slerp(&controller->weapon_transform.rotation, &weapon->recoil_rotation, &weapon->default_rotation, smoothed_time);
    }

    fw64_transform_update_matrix(&controller->weapon_transform);
}

static void weapon_controller_update_holding(WeaponController* controller) {
    if (controller->weapon == NULL)
        return;

    weapon_controller_update_recoil(controller);
    weapon_controller_update_muzzle_flash(controller);

    if (controller->time_to_next_fire > 0.0f) {
        controller->time_to_next_fire -= controller->engine->time->time_delta;
        weapon_controller_update_casing(controller);
    }

    if (controller->time_to_next_fire <= 0.0f && mapped_input_controller_read(controller->input_map, controller->controller_index, INPUT_MAP_WEAPON_FIRE, NULL)) {
        weapon_controller_fire(controller);
    }
}

static void weapon_controller_transition_complete(WeaponController* controller) {
        if (controller->transition_callback)
            controller->transition_callback(controller->weapon, controller->state, controller->transition_arg);
}

static void weapon_controller_update_transition(WeaponController* controller, Vec3* start, Vec3* end) {
    controller->transition_time += controller->engine->time->time_delta;

    if (controller->transition_time >= 1.0f) {
        vec3_copy(&controller->weapon_transform.position, end);
    }
    else {
        float t = controller->transition_time / WEAPON_CONTROLLER_TRANSITION_SPEED;
        vec3_smoothstep(&controller->weapon_transform.position, start, end, t);
    }
    
    fw64_transform_update_matrix(&controller->weapon_transform);
}

static void weapon_controller_update_raising(WeaponController* controller) {
    weapon_controller_update_transition(controller, &controller->weapon->lowered_position, &controller->weapon->default_position);

    if (controller->transition_time >= WEAPON_CONTROLLER_TRANSITION_SPEED) {
        weapon_controller_transition_complete(controller);
        controller->state = WEAPON_CONTROLLER_HOLDING;
    }
}

static void weapon_controller_update_lowering(WeaponController* controller) {
    weapon_controller_update_transition(controller, &controller->weapon->default_position, &controller->weapon->lowered_position);

    if (controller->transition_time >= WEAPON_CONTROLLER_TRANSITION_SPEED) {
        controller->state = WEAPON_CONTROLLER_LOWERED;
        weapon_controller_transition_complete(controller);
    }
}

void weapon_controller_update(WeaponController* controller) {
    switch (controller->state) {
        case WEAPON_CONTROLLER_HOLDING:
            weapon_controller_update_holding(controller);
            break;

        case WEAPON_CONTROLLER_LOWERING:
            weapon_controller_update_lowering(controller);
            break;

        case WEAPON_CONTROLLER_RAISING:
            weapon_controller_update_raising(controller);
            break;

        case WEAPON_CONTROLLER_LOWERED:
            break;
    }
}

void weapon_controller_set_weapon(WeaponController* controller, Weapon* weapon) {
    controller->weapon = weapon;
    controller->time_to_next_fire = 0.0f; // i think this is OK?

    // TODO: does this need to be investigated more?
    if (controller->state == WEAPON_CONTROLLER_HOLDING) {
        controller->weapon_transform.position = weapon->default_position;
    }
    else if (controller->state == WEAPON_CONTROLLER_LOWERED) {
        controller->weapon_transform.position = weapon->lowered_position;
    }
    
    controller->weapon_transform.scale = weapon->default_scale;
    fw64_transform_update_matrix(&controller->weapon_transform);

    if (weapon->casing) {
        if (weapon->type == WEAPON_TYPE_AR15) // temp fix
            vec3_set_all(&controller->casing_transform.scale, 0.02f);
        else
            controller->casing_transform.scale = weapon->default_scale;

        fw64_transform_update_matrix(&controller->casing_transform);
    }

    // note other muzzle flash values will be set during update.
    controller->muzzle_flash_transform.scale = weapon->default_scale;
}

static void weapon_controller_fire(WeaponController* controller) {
    fw64_audio_play_sound(controller->engine->audio, controller->weapon->gunshot_sound);
    controller->time_to_next_fire = controller->weapon->fire_rate;

    controller->casing_transform.position = controller->weapon->ejection_port_pos;
    fw64_transform_update_matrix(&controller->casing_transform);

    projectile_controller_fire_ray(controller->projectile_controller, controller->aim->position, &controller->aim->direction, controller->weapon->type);

    controller->muzzle_flash_time_remaining = WEAPON_CONTROLLER_MUZZLE_FLASH_TIME;
    weapon_controller_update_muzzle_flash(controller);
    controller->recoil_state = WEAPON_RECOIL_RECOILING;
}

static int weapon_controller_start_transition(WeaponController* controller, WeaponControllerState target_state, WeaponTransitionFunc callback, void* arg) {
    if (controller->state == WEAPON_CONTROLLER_LOWERING || controller->state == WEAPON_CONTROLLER_RAISING)
        return 0;

    controller->time_to_next_fire = 0.0f; // stop casing from rendering
    controller->state = target_state;
    controller->transition_callback = callback;
    controller->transition_arg = arg;
    controller->transition_time = 0.0f;

    return 1;
}

int weapon_controller_raise_weapon(WeaponController* controller, WeaponTransitionFunc callback, void* arg) {
    return weapon_controller_start_transition(controller,  WEAPON_CONTROLLER_RAISING, callback, arg);
}

int weapon_controller_lower_weapon(WeaponController* controller, WeaponTransitionFunc callback, void* arg) {
    return weapon_controller_start_transition(controller,  WEAPON_CONTROLLER_LOWERING, callback, arg);
}