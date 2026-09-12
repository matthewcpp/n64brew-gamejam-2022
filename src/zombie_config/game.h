#pragma once

#include "framework64/animation_controller.h"
#include "framework64/engine.h"
#include "level.h"

#include "arcball_camera.h"
#include "zombie_appearance.h"
#include "zombie_config_ui.h"

#include "components/fw64_headlight.h"

typedef enum {
    ZOMBIE_CONFIG_RENDERPASS_WORLD,
    ZOMBIE_CONFIG_RENDERPASS_UI,
    ZOMBIE_CONFIG_RENDERPASS_COUNT
} ZombieConfigRenderPass;

typedef struct {
    fw64Engine* engine;
    fw64ArcballCamera arcball;
    fw64Node* zombie_node;
    fw64Node* camera_node;
    fw64SkinnedMeshInstance* zombie_mesh_instance;
    fw64Camera* camera;
    ZombieConfigUi ui;
    fw64RenderPass* renderpasses[ZOMBIE_CONFIG_RENDERPASS_COUNT];
    fw64Headlight headlight;
} Game;

#ifdef __cplusplus
extern "C" {
#endif

void game_init(Game* game, fw64Engine* engine);
void game_update(Game* game);
void game_fixed_update(Game* game);
void game_draw(Game* game);

#ifdef __cplusplus
}
#endif
