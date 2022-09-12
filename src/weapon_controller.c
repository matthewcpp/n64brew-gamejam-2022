#include "weapon_controller.h"

#include "framework64/math.h"
#include "framework64/n64/controller_button.h"

#include "zombie.h"

#define WEAPON_CONTROLLER_TRANSITION_SPEED 0.75f

static void weapon_controller_fire(WeaponController* controller);

void weapon_controller_init(WeaponController* controller, fw64Engine* engine, fw64Level* level, InputMapping* input_map, int controller_index) {
    controller->engine = engine;
    controller->level = level;
    controller->controller_index = controller_index;
    controller->input_map = input_map;
    controller->weapon = NULL;
    controller->state = WEAPON_CONTROLLER_HOLDING;
    controller->transition_time = 0.0f;

    fw64_transform_init(&controller->weapon_transform);
    fw64_transform_init(&controller->casing_transform);
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

static void weapon_controller_update_holding(WeaponController* controller) {
    if (controller->weapon == NULL)
        return;

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
}

static void weapon_controller_fire(WeaponController* controller) {
    fw64_audio_play_sound(controller->engine->audio, controller->weapon->gunshot_sound);
    controller->time_to_next_fire = controller->weapon->fire_rate;

    controller->casing_transform.position = controller->weapon->ejection_port_pos;
    fw64_transform_update_matrix(&controller->casing_transform);

    // temp implementation for now need to raycast etc
    uint32_t dynamic_node_count = fw64_level_get_dynamic_node_count(controller->level);
    float closest_dist = FLT_MAX;
    Zombie* closest_nerd = NULL;
    for (uint32_t i = 0; i < dynamic_node_count; i++) {
        fw64Node* dynamic_node = fw64_level_get_dynamic_node(controller->level, i);

        if (dynamic_node->layer_mask & ZOMBIE_LAYER) {
            Zombie* zombie = (Zombie*)dynamic_node->data;
            if (zombie->health > 0 && zombie->state != ZOMBIE_FLYING_BACK) {
                Vec3 out_hitpoint;
                float out_distance;
                int bounding_hit = fw64_collision_test_ray_box( controller->aim->position,
                                                                &controller->aim->direction,
                                                                &zombie->node.collider->bounding,
                                                                &out_hitpoint,
                                                                &out_distance);
                if(bounding_hit && (out_distance < closest_dist)) {                    
                    closest_dist = out_distance;
                    closest_nerd = zombie;
                } 
            }          
        }
    }
    if(closest_nerd != NULL) {
        zombie_hit(closest_nerd, controller->weapon->type);
    }
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