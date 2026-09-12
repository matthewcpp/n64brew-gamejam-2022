#include "zombie_config.h"

#include "assets/assets.h"

// TODO: move this into a common file
#define ZOMBIE_SCALE 0.025f

void zombie_config_init(ZombieConfig* config, fw64Engine* engine, fw64Allocator* allocator) {
fw64Display* display = fw64_displays_get_primary(engine->displays);
    config->engine = engine;
    config->allocator = allocator;

    config->zombie_node = allocator->malloc(allocator, sizeof(fw64Node));
    fw64_node_init(config->zombie_node);
    vec3_set_all(&config->zombie_node->transform.scale, ZOMBIE_SCALE);
    fw64_node_update(config->zombie_node);

    fw64SkinnedMesh* zombie = fw64_assets_load_skinned_mesh(engine->assets, FW64_ASSET_skinnedmesh_zombie, allocator);
    config->zombie_mesh_instance = allocator->malloc(allocator, sizeof(fw64Node));
    fw64_skinned_mesh_instance_init(config->zombie_mesh_instance, config->zombie_node, zombie, 0, allocator);
    config->zombie_mesh_instance->controller.loop = 1;
    fw64_animation_controller_play(&config->zombie_mesh_instance->controller);

    config->camera_node = allocator->malloc(allocator, sizeof(fw64Node));
    fw64_node_init(config->camera_node);
    
    config->camera = allocator->malloc(allocator, sizeof(fw64Camera));
    fw64_camera_init(config->camera, config->camera_node, display);
    config->camera->near = 1.0f;
    config->camera->far = 100.0f;
    fw64_camera_update_projection_matrix(config->camera);

    fw64_arcball_init(&config->arcball, engine->input, config->camera);
    Box mesh_bounding = fw64_mesh_get_bounding_box(zombie->mesh);
    fw64_arcball_set_initial(&config->arcball, &config->zombie_mesh_instance->mesh_instance.render_bounds);

    zombie_appearance_init(&config->zombie_appearance, engine, config->zombie_mesh_instance, allocator);

    // note zombie config will be initialized in the ui init call
    zombie_config_ui_init(&config->ui, engine, config->zombie_mesh_instance, &config->zombie_appearance, allocator);

    config->renderpasses[ZOMBIE_CONFIG_RENDERPASS_WORLD] = fw64_renderpass_create(display, allocator);
    config->renderpasses[ZOMBIE_CONFIG_RENDERPASS_UI] = fw64_renderpass_create(display, allocator);
    fw64_renderpass_util_ortho2d(config->renderpasses[ZOMBIE_CONFIG_RENDERPASS_UI]);

    fw64_headlight_init(&config->headlight, config->renderpasses[ZOMBIE_CONFIG_RENDERPASS_WORLD], 0, &config->camera_node->transform);
}

void zombie_config_uninit(ZombieConfig* config){
    zombie_config_ui_uninit(&config->ui);

    zombie_appearance_uninit(&config->zombie_appearance);

    for (int i = 0; i < ZOMBIE_CONFIG_RENDERPASS_COUNT; i++) {
        fw64_renderpass_delete(config->renderpasses[i]);
    }

    fw64_skinned_mesh_delete(config->zombie_mesh_instance->skinned_mesh, config->engine->assets, config->allocator);
    fw64_allocator_free(config->allocator, config->zombie_mesh_instance);
    fw64_allocator_free(config->allocator, config->camera_node);
    fw64_allocator_free(config->allocator, config->zombie_node);
    fw64_allocator_free(config->allocator, config->camera);
}

void zombie_config_update(ZombieConfig* config){
    fw64_arcball_update(&config->arcball, config->engine->time->time_delta);
    fw64_headlight_update(&config->headlight);
    fw64_skinned_mesh_instance_update(config->zombie_mesh_instance, config->engine->time->time_delta);
    zombie_config_ui_update(&config->ui);
}

void zombie_config_draw(ZombieConfig* config){
    fw64Renderer* renderer = config->engine->renderer;

    fw64RenderPass* renderpass = config->renderpasses[ZOMBIE_CONFIG_RENDERPASS_WORLD];
    fw64_renderpass_begin(renderpass);
    fw64_renderpass_set_camera(renderpass, config->camera);
    fw64_renderpass_draw_skinned_mesh(renderpass, config->zombie_mesh_instance);
    fw64_renderpass_end(renderpass);
    fw64_renderer_submit_renderpass(config->engine->renderer, renderpass);

    renderpass = config->renderpasses[ZOMBIE_CONFIG_RENDERPASS_UI];
    fw64_renderpass_begin(renderpass);
    zombie_config_ui_draw(&config->ui, renderpass);
    fw64_renderpass_end(renderpass);
    fw64_renderer_submit_renderpass(config->engine->renderer, renderpass);
}