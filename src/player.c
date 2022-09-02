#include "player.h"

#include "framework64/n64/controller_button.h"
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

    fw64_camera_init(&player->weapon_camera);
    vec3_zero(&player->weapon_camera.transform.position);
    fw64_camera_update_view_matrix(&player->weapon_camera);

    player->weapon_camera.near = 0.1f;
    player->weapon_camera.far = 125.0f;
    player->weapon_camera.fovy = 60.0f;
    fw64_camera_update_projection_matrix(&player->weapon_camera);

    weapon_init(&player->weapon);
    weapon_controller_init(&player->weapon_controller, engine, 0);
}

static void player_next_weapon_func(Weapon* current_weapon, WeaponControllerState complete_state, void* arg) {
    Player* player = (Player*)arg;

    if (player->weapon.type == WEAPON_TYPE_AR15)
        player_set_weapon(player, WEAPON_TYPE_SHOTGUN);
    else
        player_set_weapon(player, WEAPON_TYPE_AR15);

    weapon_controller_raise_weapon(&player->weapon_controller, NULL, NULL);
}

void player_update(Player* player) {
    fw64_fps_camera_update(&player->camera, player->engine->time->time_delta);
    weapon_controller_update(&player->weapon_controller);

    if (fw64_input_controller_button_pressed(player->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_R))
        weapon_controller_lower_weapon(&player->weapon_controller, player_next_weapon_func, player);
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

    if (player->weapon.type == WEAPON_TYPE_NONE)
        return;
    
    fw64_renderer_set_fog_enabled(renderer, 0);
    fw64_renderer_set_camera(renderer, &player->weapon_camera);
    fw64_renderer_util_clear_viewport(renderer, &player->weapon_camera, FW64_RENDERER_FLAG_CLEAR_DEPTH);
    fw64_renderer_draw_static_mesh(renderer, &player->weapon_controller.weapon_transform, player->weapon.mesh);
}

void player_set_weapon(Player* player, WeaponType weapon_type) {
    switch(weapon_type) {
        case WEAPON_TYPE_AR15:
            weapon_init_ar15(&player->weapon, player->engine->assets, NULL);
        break;

        case WEAPON_TYPE_SHOTGUN:
            weapon_init_shotgun(&player->weapon, player->engine->assets, NULL);
        break;
    }

    weapon_controller_set_weapon(&player->weapon_controller, &player->weapon);
}