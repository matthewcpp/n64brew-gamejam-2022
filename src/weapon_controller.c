#include "weapon_controller.h"

#include "framework64/math.h"
#include "framework64/n64/controller_button.h"

#include "zombie.h"

#include <string.h>

#define WEAPON_CONTROLLER_TRANSITION_SPEED 0.50f
#define WEAPON_CONTROLLER_MUZZLE_FLASH_TIME 0.1f

#define WEAPON_CONTROLLER_MEMORY_POOL_SIZE (50 * 1024)

static void weapon_controller_fire(WeaponController* controller);
static int weapon_controller_is_idle(WeaponController* controller);
static void free_existing_weapon_data(WeaponController* controller);

void weapon_controller_init(WeaponController* controller, fw64Engine* engine, WeaponBob* weapon_bob, ProjectileController* projectile_controller, AudioController* audio_controller, fw64Allocator* player_allocator, InputMapping* input_map, int controller_index) {
    controller->engine = engine;
    controller->weapon_bob = weapon_bob;
    controller->projectile_controller = projectile_controller;
    controller->audio_controller = audio_controller;
    controller->controller_index = controller_index;
    controller->input_map = input_map;
    controller->state = WEAPON_CONTROLLER_HOLDING;
    controller->transition_time = 0.0f;
    controller->muzzle_flash_time_remaining = 0.0f;
    controller->recoil_time = 0.0f;
    controller->recoil_state = WEAPON_RECOIL_INACTIVE;
    controller->is_dry_firing = 0;

    fw64_bump_allocator_init_from_buffer(&controller->weapon_allocator, player_allocator->memalign(player_allocator, 8, WEAPON_CONTROLLER_MEMORY_POOL_SIZE), WEAPON_CONTROLLER_MEMORY_POOL_SIZE);

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

    memset(&controller->weapon_ammo[0], 0, sizeof(WeaponAmmo) * WEAPON_COUNT);

    weapon_init(&controller->weapon);
    weapon_init_none(&controller->weapon, controller->engine->assets, &controller->weapon_allocator.interface);
}

void weapon_controller_uninit(WeaponController* controller) {
    free_existing_weapon_data(controller);
    fw64_bump_allocator_uninit(&controller->weapon_allocator);
}

void free_existing_weapon_data(WeaponController* controller) {
    if (controller->weapon.mesh)
        fw64_mesh_delete(controller->engine->assets, controller->weapon.mesh, &controller->weapon_allocator.interface);
    if (controller->weapon.casing)
        fw64_mesh_delete(controller->engine->assets, controller->weapon.casing, &controller->weapon_allocator.interface);
    if (controller->weapon.muzzle_flash)
        fw64_mesh_delete(controller->engine->assets, controller->weapon.muzzle_flash, &controller->weapon_allocator.interface);
    if (controller->weapon.crosshair) {
        fw64_image_delete(controller->engine->assets, fw64_texture_get_image(controller->weapon.crosshair), &controller->weapon_allocator.interface);
        fw64_texture_delete(controller->weapon.crosshair, &controller->weapon_allocator.interface);
    }

    fw64_bump_allocator_reset(&controller->weapon_allocator);
}

/** shell casing ejection is modeled via a simple quadratic equation*/
static void weapon_controller_update_casing(WeaponController* controller) {
    float fly_time = controller->weapon.info->fire_rate - controller->time_to_next_fire;

    float x = fly_time;
    float y = (-((0.5f * x - 1.5f) * (0.5f * x - 1.5f)) + 2.0f) * 0.5f;

    float x_scale = 16.0f;
    float y_scale = 8.5f;

    vec3_add(&controller->casing_transform.position, &controller->weapon.info->ejection_port_pos, &controller->weapon_bob->translation);
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
    Weapon* weapon = &controller->weapon;

    controller->recoil_time += controller->engine->time->time_delta;

    if (controller->recoil_time >= weapon->info->recoil_time) {
        if (controller->recoil_state == WEAPON_RECOIL_RECOILING) {
            controller->recoil_time -= weapon->info->recoil_time;
            controller->recoil_state = WEAPON_RECOIL_RECOVERING;
        }
        else { // recoil is finished
            controller->recoil_state = WEAPON_RECOIL_INACTIVE;
            controller->recoil_time = 0.0f;
            controller->weapon_transform.position = weapon->info->default_position;
            controller->weapon_transform.rotation = weapon->info->default_rotation;
        }
    }

    float smoothed_time = fw64_smoothstep(0.0f, 1.0f, controller->recoil_time / weapon->info->recoil_time);

    if (controller->recoil_state == WEAPON_RECOIL_RECOILING) {
        vec3_lerp(&controller->weapon_transform.position, &weapon->info->default_position, &weapon->info->recoil_pos, smoothed_time);
    }
    else if (controller->recoil_state == WEAPON_RECOIL_RECOVERING) {
        vec3_lerp(&controller->weapon_transform.position, &weapon->info->recoil_pos, &weapon->info->default_position, smoothed_time);
    }

    vec3_add(&controller->weapon_transform.position, &controller->weapon_transform.position, &controller->weapon_bob->translation);
    fw64_transform_update_matrix(&controller->weapon_transform);
}

static void weapon_controller_update_moving(WeaponController* controller) {
    vec3_add(&controller->weapon_transform.position, &controller->weapon.info->default_position, &controller->weapon_bob->translation);
    fw64_transform_update_matrix(&controller->weapon_transform);
}

static void weapon_controller_update_holding(WeaponController* controller) {
    if (controller->weapon.info->type == WEAPON_TYPE_NONE)
        return;

    if (controller->recoil_state == WEAPON_RECOIL_INACTIVE)
        weapon_controller_update_moving(controller);
    else
        weapon_controller_update_recoil(controller);

    weapon_controller_update_muzzle_flash(controller);

    if (controller->time_to_next_fire > 0.0f) {
        controller->time_to_next_fire -= controller->engine->time->time_delta;
        weapon_controller_update_casing(controller);
    }

    if (controller->time_to_next_fire > 0.0f)
        return;

    if (mapped_input_controller_read(controller->input_map, controller->controller_index, INPUT_MAP_WEAPON_FIRE, NULL)) {
        weapon_controller_fire(controller);
    }

    if (mapped_input_controller_read(controller->input_map, controller->controller_index, INPUT_MAP_WEAPON_RELOAD, NULL)) {
        weapon_controller_reload_current_weapon(controller);
    }
}

static void weapon_controller_transition_complete(WeaponController* controller) {
        if (controller->transition_callback)
            controller->transition_callback(&controller->weapon, controller->state, controller->transition_arg);
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
    weapon_controller_update_transition(controller, &controller->weapon.info->lowered_position, &controller->weapon.info->default_position);

    if (controller->transition_time >= WEAPON_CONTROLLER_TRANSITION_SPEED) {
        weapon_controller_transition_complete(controller);
        controller->state = WEAPON_CONTROLLER_HOLDING;
    }
}

static void weapon_controller_update_lowering(WeaponController* controller) {
    weapon_controller_update_transition(controller, &controller->weapon.info->default_position, &controller->weapon.info->lowered_position);

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

void weapon_controller_draw(WeaponController* controller) {
    fw64Renderer* renderer = controller->engine->renderer;
    Weapon* weapon = &controller->weapon;

    fw64_renderer_draw_static_mesh(renderer, &controller->weapon_transform, weapon->mesh);
    
    if (controller->muzzle_flash_time_remaining > 0.0f) {
        fw64_renderer_draw_static_mesh(renderer, &controller->muzzle_flash_transform, weapon->muzzle_flash);
    }

    if (controller->time_to_next_fire > 0.0f && weapon->casing && !controller->is_dry_firing) {
        fw64_renderer_draw_static_mesh(renderer, &controller->casing_transform, controller->weapon.casing);
    }
}

void weapon_controller_set_weapon(WeaponController* controller, WeaponType weapon_type) {
    WeaponType previous_weapon_type = controller->weapon.info->type;

    if (weapon_type == previous_weapon_type)
        return;

    free_existing_weapon_data(controller);

    Weapon* weapon = &controller->weapon;
    switch(weapon_type) {
        case WEAPON_TYPE_HANDGUN:
            weapon_init_handgun(weapon, controller->engine->assets, &controller->weapon_allocator.interface);
        break;

        case WEAPON_TYPE_AR15:
            weapon_init_ar15(weapon, controller->engine->assets, &controller->weapon_allocator.interface);
        break;

        case WEAPON_TYPE_SHOTGUN:
            weapon_init_shotgun(weapon, controller->engine->assets, &controller->weapon_allocator.interface);
        break;

        case WEAPON_TYPE_UZI:
            weapon_init_uzi(weapon, controller->engine->assets, &controller->weapon_allocator.interface);
        break;

        case WEAPON_TYPE_NONE:
        case WEAPON_COUNT:
            weapon_init_none(weapon, controller->engine->assets, &controller->weapon_allocator.interface);
            break;
    }

    controller->time_to_next_fire = 0.0f; // i think this is OK?

    // TODO: does this need to be investigated more?
    if (controller->state == WEAPON_CONTROLLER_LOWERED || previous_weapon_type == WEAPON_TYPE_NONE) {
        controller->weapon_transform.position = weapon->info->lowered_position;
    }
    else if (controller->state == WEAPON_CONTROLLER_HOLDING) {
        controller->weapon_transform.position = weapon->info->default_position;
    }
    
    controller->weapon_transform.scale = weapon->info->default_scale;
    fw64_transform_update_matrix(&controller->weapon_transform);

    if (weapon->casing) {
        if (weapon->info->type == WEAPON_TYPE_AR15) // temp fix
            vec3_set_all(&controller->casing_transform.scale, 0.02f);
        else
            controller->casing_transform.scale = weapon->info->default_scale;

        fw64_transform_update_matrix(&controller->casing_transform);
    }

    // note other muzzle flash values will be set during update.
    controller->muzzle_flash_transform.scale = weapon->info->default_scale;
}

static void weapon_controller_fire(WeaponController* controller) {
    if (!weapon_controller_is_idle(controller))
        return;

    WeaponAmmo* weapon_ammo = weapon_controller_get_current_weapon_ammo(controller);
    if (weapon_ammo->current_mag_count <= 0) {
        audio_controller_play(controller->audio_controller, AUDIO_CONTROLLER_CHANNEL_PLAYER_WEAPON, controller->weapon.info->empty_mag_sound);
        controller->time_to_next_fire = controller->weapon.info->dry_fire_rate;
        controller->is_dry_firing = 1;
        return;
    }

    audio_controller_play(controller->audio_controller, AUDIO_CONTROLLER_CHANNEL_PLAYER_WEAPON, weapon_ammo->current_mag_count > 1 ? controller->weapon.info->gunshot_sound : controller->weapon.info->last_round_sound);
    controller->time_to_next_fire = controller->weapon.info->fire_rate;
    controller->is_dry_firing = 0;

    controller->casing_transform.position = controller->weapon.info->ejection_port_pos;
    fw64_transform_update_matrix(&controller->casing_transform);

    if(controller->weapon.info->type == WEAPON_TYPE_SHOTGUN)
        projectile_controller_fire_arc(controller->projectile_controller, controller->aim->position, &controller->aim->direction, 30.0f, 20.0f, controller->weapon.info->type);
    else
        projectile_controller_fire_ray(controller->projectile_controller, controller->aim->position, &controller->aim->direction, controller->weapon.info->type);

    controller->muzzle_flash_time_remaining = WEAPON_CONTROLLER_MUZZLE_FLASH_TIME;
    weapon_controller_update_muzzle_flash(controller);
    controller->recoil_state = WEAPON_RECOIL_RECOILING;

    weapon_ammo->current_mag_count -= 1;
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

static WeaponType get_next_weapon_with_ammo(WeaponController* controller) {
    WeaponType current_weapon_type = controller->weapon.info->type;
    WeaponType next_weapon_type = current_weapon_type + 1;

    while (next_weapon_type != current_weapon_type) {
        if (next_weapon_type == WEAPON_COUNT)
            next_weapon_type = WEAPON_TYPE_NONE;

        WeaponAmmo* weapon_ammo = &controller->weapon_ammo[next_weapon_type];
        if (weapon_ammo->additional_rounds_count > 0 || weapon_ammo->current_mag_count > 0) {
            return next_weapon_type;
        }

        next_weapon_type += 1;
    }

    return WEAPON_TYPE_NONE;
}

static void next_weapon_func(Weapon* current_weapon, WeaponControllerState complete_state, void* arg) {
    WeaponController* controller = (WeaponController*)arg;
    WeaponType next_weapon = get_next_weapon_with_ammo(controller);
    weapon_controller_set_weapon(controller, next_weapon);
    weapon_controller_raise_weapon(controller, NULL, NULL);
}

int weapon_controller_is_idle(WeaponController* controller) {
    return controller->state == WEAPON_CONTROLLER_HOLDING && controller->time_to_next_fire <= 0.0f;
}

void weapon_controller_switch_to_next_weapon(WeaponController* controller) {
    if (!weapon_controller_is_idle(controller))
        return;

    weapon_controller_lower_weapon(controller, next_weapon_func, controller);
}

static void check_weapon_ammo(WeaponController* controller, WeaponType weapon_type) {
    WeaponInfo* weapon_info = weapon_get_info(weapon_type);
    WeaponAmmo* weapon_ammo = &controller->weapon_ammo[weapon_type];

    if (weapon_ammo->current_mag_count > weapon_info->mag_size)
        weapon_ammo->current_mag_count = weapon_info->mag_size;

    if (weapon_ammo->additional_rounds_count > weapon_info->max_additional_rounds)
        weapon_ammo->additional_rounds_count = weapon_info->max_additional_rounds;
}

void weapon_controller_set_weapon_ammo(WeaponController* controller, WeaponType weapon_type, uint32_t current_mag_count, uint32_t additional_rounds_count) {
    WeaponAmmo* weapon_ammo = &controller->weapon_ammo[weapon_type];

    weapon_ammo->current_mag_count = current_mag_count;
    weapon_ammo->additional_rounds_count = additional_rounds_count;

    check_weapon_ammo(controller, weapon_type);
}

WeaponAmmo* weapon_controller_get_current_weapon_ammo(WeaponController* controller) {
    return &controller->weapon_ammo[controller->weapon.info->type];
}

void weapon_controller_refill_weapon_magazine(WeaponController* controller, WeaponType weapon_type) {
    WeaponAmmo* weapon_ammo = &controller->weapon_ammo[weapon_type];
    WeaponInfo* weapon_info = weapon_get_info(weapon_type);

    uint32_t ammo_needed = weapon_info->mag_size - weapon_ammo->current_mag_count;
    if (ammo_needed > weapon_ammo->additional_rounds_count) {
        ammo_needed = weapon_ammo->additional_rounds_count;
    }

    weapon_ammo->current_mag_count += ammo_needed;
    weapon_ammo->additional_rounds_count -= ammo_needed;
}

static void reload_weapon_func(Weapon* current_weapon, WeaponControllerState complete_state, void* arg) {
    WeaponController* controller = (WeaponController*)arg;
    weapon_controller_refill_weapon_magazine(controller, controller->weapon.info->type);
    
    audio_controller_play(controller->audio_controller, AUDIO_CONTROLLER_CHANNEL_PLAYER_WEAPON, controller->weapon.info->reload_sound);

    weapon_controller_raise_weapon(controller, NULL, NULL);
}

void weapon_controller_reload_current_weapon(WeaponController* controller) {
    if (!weapon_controller_is_idle(controller))
        return;

    // can we reload this weapon?
    WeaponAmmo* weapon_ammo = weapon_controller_get_current_weapon_ammo(controller);
    if (weapon_ammo->current_mag_count == controller->weapon.info->mag_size || weapon_ammo->additional_rounds_count == 0)
        return;

    weapon_controller_lower_weapon(controller, reload_weapon_func, controller);
}