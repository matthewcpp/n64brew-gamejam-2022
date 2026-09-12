#include "game.h"
#include "assets/assets.h"

#include "framework64/controller_mapping/n64.h"
#include "framework64/util/renderpass_util.h"

#include "assets/zombie_animation.h"

// TODO: move this into a common file
#define ZOMBIE_SCALE 0.025f

void game_init(Game* game, fw64Engine* engine) {
    fw64Display* display = fw64_displays_get_primary(engine->displays);
    fw64Allocator* allocator = fw64_default_allocator();
    game->engine = engine;

    game->zombie_node = allocator->malloc(allocator, sizeof(fw64Node));
    fw64_node_init(game->zombie_node);
    vec3_set_all(&game->zombie_node->transform.scale, ZOMBIE_SCALE);
    fw64_node_update(game->zombie_node);

    fw64SkinnedMesh* zombie = fw64_assets_load_skinned_mesh(engine->assets, FW64_ASSET_skinnedmesh_zombie, allocator);
    game->zombie_mesh_instance = allocator->malloc(allocator, sizeof(fw64Node));
    fw64_skinned_mesh_instance_init(game->zombie_mesh_instance, game->zombie_node, zombie, 0, allocator);
    game->zombie_mesh_instance->controller.loop = 1;
    fw64_animation_controller_play(&game->zombie_mesh_instance->controller);

    game->camera_node = allocator->malloc(allocator, sizeof(fw64Node));
    fw64_node_init(game->camera_node);
    
    game->camera = allocator->malloc(allocator, sizeof(fw64Camera));
    fw64_camera_init(game->camera, game->camera_node, display);
    game->camera->near = 1.0f;
    game->camera->far = 100.0f;
    fw64_camera_update_projection_matrix(game->camera);

    fw64_arcball_init(&game->arcball, engine->input, game->camera);
    Box mesh_bounding = fw64_mesh_get_bounding_box(zombie->mesh);
    fw64_arcball_set_initial(&game->arcball, &game->zombie_mesh_instance->mesh_instance.render_bounds);

    // note zombie config will be initialized in the ui init call
    zombie_config_ui_init(&game->ui, engine, game->zombie_mesh_instance, &game->zombie_config, allocator);

    game->renderpasses[ZOMBIE_CONFIG_RENDERPASS_WORLD] = fw64_renderpass_create(display, allocator);
    game->renderpasses[ZOMBIE_CONFIG_RENDERPASS_UI] = fw64_renderpass_create(display, allocator);
    fw64_renderpass_util_ortho2d(game->renderpasses[ZOMBIE_CONFIG_RENDERPASS_UI]);

    fw64_headlight_init(&game->headlight, game->renderpasses[ZOMBIE_CONFIG_RENDERPASS_WORLD], 0, &game->camera_node->transform);
}

void game_update(Game* game){
    fw64_arcball_update(&game->arcball, game->engine->time->time_delta);
    fw64_headlight_update(&game->headlight);
    fw64_skinned_mesh_instance_update(game->zombie_mesh_instance, game->engine->time->time_delta);
    zombie_config_ui_update(&game->ui);
}

void game_fixed_update(Game* game) {
    (void)game;
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;

    fw64RenderPass* renderpass = game->renderpasses[ZOMBIE_CONFIG_RENDERPASS_WORLD];
    fw64_renderpass_begin(renderpass);
    fw64_renderpass_set_camera(renderpass, game->camera);
    fw64_renderpass_draw_skinned_mesh(renderpass, game->zombie_mesh_instance);
    fw64_renderpass_end(renderpass);
    fw64_renderer_submit_renderpass(game->engine->renderer, renderpass);

    renderpass = game->renderpasses[ZOMBIE_CONFIG_RENDERPASS_UI];
    fw64_renderpass_begin(renderpass);
    zombie_config_ui_draw(&game->ui, renderpass);
    fw64_renderpass_end(renderpass);
    fw64_renderer_submit_renderpass(game->engine->renderer, renderpass);
}
