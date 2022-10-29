#include "catherine.h"

#include "assets/assets.h"
#include "assets/catherine_animation.h"

void catherine_init(Catherine* catherine, fw64Engine* engine, fw64Node* node, fw64Allocator* allocator) {
    catherine->engine = engine;
    catherine->node = node;
    catherine->allocator = allocator;

    fw64_node_set_mesh(node, NULL);

    catherine->mesh = fw64_mesh_load(engine->assets, FW64_ASSET_mesh_catherine, allocator);
    catherine->animation_data = fw64_animation_data_load(engine->assets, FW64_ASSET_animation_data_catherine, allocator);
    fw64_animation_controller_init(&catherine->animation_controller, catherine->animation_data, catherine_animation_Idle, allocator);
    fw64_animation_controller_play(&catherine->animation_controller);
    catherine->animation_controller.loop = 1;
    catherine->animation_controller.speed = 0.1f;
}

void catherine_update(Catherine* catherine) {
    fw64_animation_controller_update(&catherine->animation_controller, catherine->engine->time->time_delta);
}

void catherine_draw(Catherine* catherine) {
    fw64_renderer_draw_animated_mesh(catherine->engine->renderer, catherine->mesh, &catherine->animation_controller, &catherine->node->transform);
}

void catherine_uninit(Catherine* catherine) {
    fw64_animation_data_delete(catherine->animation_data, catherine->allocator);
}