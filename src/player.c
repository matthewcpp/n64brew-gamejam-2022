#include "player.h"
#include "framework64/n64/controller_button.h"
#include "framework64/util/renderer_util.h"
#include "assets/sound_bank_sounds.h"
#include "assets/layers.h"

#define DAMAGE_OVERLAY_DURATION 0.025f

static Vec3 default_player_dimensions = {0.75, 5.6f, 1.1f};

static void setup_player_node(Player* player);

void player_init(Player* player, fw64Engine* engine, fw64Level* level, ProjectileController* projectile_controller, AudioController* audio_controller, fw64Allocator* level_allocator) {
    player->engine = engine;
    player->level = level;
    player->allocator = level_allocator;

    setup_player_node(player);
    mapped_input_init(&player->input_map, engine->input);
    weapon_bob_init(&player->weapon_bob);

    movement_controller_init(&player->movement, &player->input_map, &player->weapon_bob, level, player->node->collider);
    player->movement.height = 5.0f;
    player->movement.collision_mask = FW64_layer_obstacles | FW64_layer_wall | FW64_layer_buildings;
    player->movement.camera.near = 1.0f;
    player->movement.camera.far = 225.0f;
    fw64_camera_update_projection_matrix(&player->movement.camera);

    player->aim.position = &player->movement.camera.transform.position;
    vec3_zero(&player->aim.direction);
    player->aim.direction.x = player->movement.rotation.x;
    player->aim.direction.y = player->movement.rotation.y;
    player->aim.infinite = 1; //boolean true

    // todo: investigate weapon allocator usage
    weapon_controller_init(&player->weapon_controller, engine, &player->weapon_bob, projectile_controller, audio_controller, level_allocator, &player->input_map, 0);
    player->weapon_controller.aim = &player->aim;
    weapon_controller_set_weapon(&player->weapon_controller, WEAPON_TYPE_NONE);
    player->current_health = 100;

    player->damage_overlay_time = 0.0f;
    player->damage_overlay_initial_time = 1.0;
}

void player_uninit(Player* player) {
    fw64Allocator* allocator = player->allocator;

    weapon_controller_uninit(&player->weapon_controller);
    allocator->free(allocator, player->node->collider);
    allocator->free(allocator, player->node);
}

void setup_player_node(Player* player) {
    fw64Allocator* allocator = player->allocator;

    player->node = allocator->malloc(allocator, sizeof(fw64Node));
    fw64_node_init(player->node);
    fw64Collider* collider = allocator->malloc(allocator, sizeof(fw64Node));
    fw64_node_set_collider(player->node, collider);

    player->node->layer_mask = FW64_layer_player;
    player->node->data = player;

    Box player_box;
    vec3_set(&player_box.min, -default_player_dimensions.x / 2.0f, 0.0f, -default_player_dimensions.z /2.0f);
    vec3_set(&player_box.max, default_player_dimensions.x / 2.0f, default_player_dimensions.y, default_player_dimensions.z / 2.0f);

    fw64_collider_set_type_box(collider, &player_box);
    fw64_level_add_dynamic_node(player->level, player->node);
}

void player_aim_update(Player* player) {

    Quat q;
    quat_from_euler(&q, player->movement.rotation.x, player->movement.rotation.y, 0.0f);

    Vec3 forward = { 0.0f, 0.0f, -1.0f };
    quat_transform_vec3(&player->aim.direction, &q, &forward);
}

void player_update(Player* player) {
    player->weapon_bob.is_active = 0;
    movement_controller_update(&player->movement, player->engine->time->time_delta);
    weapon_bob_update(&player->weapon_bob, player->engine->time->time_delta);
    player_aim_update(player); // should be updated after fps camera
    weapon_controller_update(&player->weapon_controller);

    vec3_copy(&player->node->transform.position, &player->movement.camera.transform.position);
    fw64_node_update(player->node); // todo manual update xform / collider
    if(mapped_input_controller_read(&player->input_map, 0, INPUT_MAP_WEAPON_SWAP, NULL)) {
        weapon_controller_switch_to_next_weapon(&player->weapon_controller);
    }

    player->damage_overlay_time -= player->engine->time->time_delta;
    if (player->damage_overlay_time < 0.0f)
        player->damage_overlay_time = 0.0f;
}

void player_draw(Player* player) {
    fw64_renderer_set_camera(player->engine->renderer, &player->movement.camera);
    fw64_level_draw_camera_all(player->level, &player->movement.camera);
}

void player_draw_weapon(Player* player) {
    fw64Renderer* renderer = player->engine->renderer;

    if (player->weapon_controller.weapon.info->type == WEAPON_TYPE_NONE)
        return;
    
    fw64_renderer_set_camera(renderer, &player->weapon_controller.weapon_camera);
    fw64_renderer_util_clear_viewport(renderer, &player->weapon_controller.weapon_camera, FW64_RENDERER_FLAG_CLEAR_DEPTH);
    weapon_controller_draw(&player->weapon_controller);
}

void player_set_weapon(Player* player, WeaponType weapon_type) {
    weapon_controller_set_weapon(&player->weapon_controller, weapon_type);
}

void player_set_position(Player* player, Vec3* position) {
    vec3_copy(&player->movement.camera.transform.position, position);
    vec3_copy(&player->node->transform.position, position);

    fw64_camera_update_view_matrix(&player->movement.camera);
    fw64_node_update(player->node);
}

int player_add_ammo(Player* player, WeaponType weapon_type, uint32_t amount) {
    WeaponInfo* weapon_info = weapon_get_info(weapon_type);
    WeaponAmmo* weapon_ammo = &player->weapon_controller.weapon_ammo[weapon_type];

    uint32_t available_count = weapon_info->max_additional_rounds - weapon_ammo->additional_rounds_count;
    if (available_count == 0)
        return 0;

    if (amount > available_count)
        amount = available_count;

    weapon_ammo->additional_rounds_count += amount;

    if (weapon_ammo->current_mag_count == 0)
        weapon_controller_refill_weapon_magazine(&player->weapon_controller, weapon_type);

    return 1;
}

int player_pickup_ammo(Player* player, WeaponType weapon_type, uint32_t amount) {
    if (player_add_ammo(player, weapon_type, amount))
        return 0;

    if (player->weapon_controller.weapon.info->type == WEAPON_TYPE_NONE) {
        weapon_controller_set_weapon(&player->weapon_controller, weapon_type);
        weapon_controller_raise_weapon(&player->weapon_controller, NULL, NULL);
    }
    
    WeaponInfo* weapon_info = weapon_get_info(weapon_type);
    fw64_audio_play_sound(player->engine->audio, weapon_info->reload_sound);

    return 1;
}

void player_take_damage(Player* player, int amount) {
    player->current_health -= amount;

    int speed_mod = player->current_health / 10;
    switch (speed_mod) {
        case 0:
        player->movement.injury_speed_mod = 0.25f;
            break;
        case 1: /* fall through */
        case 2:
            player->movement.injury_speed_mod = 0.5f;
            break;
        case 3: /* fall through */
        case 4:
            player->movement.injury_speed_mod = 0.7f;
            break;        
        case 5: /* fall through */
        case 6:
            player->movement.injury_speed_mod = 0.9f;
            break;        
        case 7: /* fall through */
        case 8: /* fall through */
        case 9: /* fall through */
        default:
            player->movement.injury_speed_mod = 1.0f;
            break;
    }
    

    if((float)amount * DAMAGE_OVERLAY_DURATION > player->movement.staggered_timer) {
        player->movement.staggered_timer = (float)amount * DAMAGE_OVERLAY_DURATION;
    }

    if((float)amount * DAMAGE_OVERLAY_DURATION > player->damage_overlay_time) {
        player->damage_overlay_time = (float)amount * DAMAGE_OVERLAY_DURATION;
        player->damage_overlay_initial_time = player->damage_overlay_time;
    }
}

void player_draw_damage(Player* player) {
    fw64Renderer* renderer = player->engine->renderer;
    if (player->damage_overlay_time > 0.0f) {
        uint8_t alpha = (uint8_t)(100.0f * (1.0f - ((player->damage_overlay_initial_time - player->damage_overlay_time)/player->damage_overlay_initial_time)));
        fw64_renderer_util_fullscreen_overlay(renderer, 165, 0, 0, alpha);
    }
    
}

int player_is_interacting(Player* player) {
    static int button_released = 0;
    if(mapped_input_controller_read(&player->input_map, player->movement.player_index, INPUT_MAP_INTERACT, NULL)) {
        if(button_released) {
            button_released = 0;
            return 1;
        }
    } else {
        button_released = 1;
    }
    return 0;
}