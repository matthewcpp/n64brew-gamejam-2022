#include "game.h"
#include "assets/assets.h"

#include "framework64/n64/controller_button.h"

#define BUMP_ALLOCATOR_SIZE 256 * 1024

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    game->font = fw64_font_load(engine->assets, FW64_ASSET_font_tools_default, fw64_default_allocator());
    fw64_scene_viewer_init(&game->scene_viewer, engine, game->font, BUMP_ALLOCATOR_SIZE);

    game->scene_viewer.fly_cam.camera.near = 1.0f;
    game->scene_viewer.fly_cam.camera.far = 200.0f;
    fw64_camera_update_projection_matrix(&game->scene_viewer.fly_cam.camera);

    fw64_scene_viewer_load(&game->scene_viewer, FW64_ASSET_scene_1_door);
}

void game_update(Game* game){
    fw64_scene_viewer_update(&game->scene_viewer);
}

void game_draw(Game* game) {
    fw64_scene_viewer_draw(&game->scene_viewer);
}
