#pragma once

#include "arcball_camera.h"
#include "zombie_appearance.h"
#include "zombie_config_ui.h"

#include <framework64/engine.h>

#include "components/fw64_headlight.h"

typedef enum {
    ZOMBIE_CONFIG_RENDERPASS_WORLD,
    ZOMBIE_CONFIG_RENDERPASS_UI,
    ZOMBIE_CONFIG_RENDERPASS_COUNT
} ZombieConfigRenderPass;

/** Game state which allows for viewing all the different looks of the zombies */
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
    fw64Allocator* allocator;
    ZombieAppearance zombie_appearance;
} ZombieConfig;

void zombie_config_init(ZombieConfig* config, fw64Engine* engine, fw64Allocator* allocator);
void zombie_config_uninit(ZombieConfig* config);

void zombie_config_update(ZombieConfig* config);
void zombie_config_draw(ZombieConfig* config);
