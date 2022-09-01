#include "player.h"

#include "framework64/util/renderer_util.h"
#include "sound_bank_sounds.h"

void player_init(Player* player, fw64Engine* engine, fw64Scene* scene) {
    player->engine = engine;
    player->scene = scene;

    fw64_fps_camera_init(&player->camera, engine->input);
    player->camera.camera.transform.position.z = 45.0f;
    player->camera.camera.transform.position.y = 5.0f;
    player->camera.movement_speed = 40.0f;
    player->camera.camera.near = 2.0f;
    player->camera.camera.far = 225.0f;
    fw64_camera_update_projection_matrix(&player->camera.camera);
}

#include "framework64/n64/controller_button.h" 

static void player_fire_weapon(Player* player) {
    fw64_audio_play_sound(player->engine->audio, sound_bank_sounds_rifle_shot_2);
}

void player_update(Player* player) {
    fw64_fps_camera_update(&player->camera, player->engine->time->time_delta);

        // temp
    if (fw64_input_controller_button_pressed(player->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_Z)) {
        player_fire_weapon(player);
    }
}

void player_draw(Player* player) {
    fw64Renderer* renderer = player->engine->renderer;
    fw64Frustum frustum;
    fw64_camera_extract_frustum_planes(&player->camera.camera, &frustum);

    fw64_renderer_set_fog_enabled(renderer, 0);
    fw64_renderer_set_camera(renderer, &player->camera.camera);
    fw64_scene_draw_frustrum(player->scene, renderer, &frustum);
}

void player_draw_weapon(Player* player) {
    fw64Renderer* renderer = player->engine->renderer;
    
    fw64_renderer_set_fog_enabled(renderer, 0);
    fw64_renderer_set_camera(renderer, &player->weapon_camera);
    fw64_renderer_util_clear_viewport(renderer, &player->weapon_camera, FW64_RENDERER_FLAG_CLEAR_DEPTH);
    fw64_renderer_draw_static_mesh(renderer, &player->weapon_transform, player->weapon->mesh);
}

void player_set_weapon(Player* player, Weapon* weapon) {
    player->weapon = weapon;

    fw64_camera_init(&player->weapon_camera);
    vec3_zero(&player->weapon_camera.transform.position);
    fw64_camera_update_view_matrix(&player->weapon_camera);

    player->weapon_camera.near = 0.1f;
    player->weapon_camera.far = 125.0f;
    player->weapon_camera.fovy = 60.0f;
    fw64_camera_update_projection_matrix(&player->weapon_camera);

    player->weapon_transform.position = weapon->position;
    player->weapon_transform.rotation = weapon->rotation;
    player->weapon_transform.scale = weapon->scale;

    fw64_transform_update_matrix(&player->weapon_transform);
}