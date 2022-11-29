#include "game.h"
#include "assets/assets.h"

#include "framework64/n64/controller_button.h"
#include "assets/zombie_animation.h"

// TODO: move this into a common file
#define ZOMBIE_SCALE 0.025f

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    game->engine = engine;

    game->node = allocator->malloc(allocator, sizeof(fw64Node));
    fw64_node_init(game->node);
    fw64_node_set_mesh(game->node, fw64_mesh_load(engine->assets, FW64_ASSET_mesh_zombie, allocator));
    fw64_node_set_box_collider(game->node, allocator->malloc(allocator, sizeof(fw64Collider)));
    vec3_set_all(&game->node->transform.scale, ZOMBIE_SCALE);
    fw64_node_update(game->node);

    game->animation_data = fw64_animation_data_load(engine->assets, FW64_ASSET_animation_data_zombie, allocator);
    game->animation_controller = allocator->malloc(allocator, sizeof(fw64AnimationController));
    fw64_animation_controller_init(game->animation_controller, game->animation_data, zombie_animation_Idle, allocator);
    game->animation_controller->loop = 1;
    fw64_animation_controller_play(game->animation_controller);


    fw64_arcball_init(&game->arcball, engine->input);
    game->arcball.camera.near = 1.0f;
    game->arcball.camera.far = 100.0f;
    fw64_camera_update_projection_matrix(&game->arcball.camera);
    fw64_arcball_set_initial(&game->arcball, &game->node->collider->bounding);

    // note zombie config will be initialized in the ui init call
    zombie_config_ui_init(&game->ui, engine, game->node->mesh, &game->zombie_config, game->animation_controller);
}

void game_update(Game* game){
    fw64_arcball_update(&game->arcball, game->engine->time->time_delta);
    fw64_animation_controller_update(game->animation_controller, game->engine->time->time_delta);
    zombie_config_ui_update(&game->ui);
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;

    fw64_renderer_begin(game->engine->renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_set_camera(renderer, &game->arcball.camera);
    zombie_config_apply(&game->zombie_config, game->node->mesh);
    fw64_renderer_draw_animated_mesh(renderer, game->node->mesh, game->animation_controller, &game->node->transform);
    zombie_config_ui_draw(&game->ui);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}
