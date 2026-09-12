#pragma once

#include "zombie_appearance.h"

#include <framework64/animation_controller.h>
#include <framework64/engine.h>

typedef enum {
    OPTION_ANIMATION,
    OPTION_FACE_IMAGE,
    OPTION_SHIRT_PALETTE,
    OPTION_PANTS_PALETTE,
    OPTION_COUNT
} Option;

#define ZOMBIE_FACE_IMAGE_COUNT 4

typedef struct {
    fw64Engine* engine;
    fw64Font* font;
    fw64SkinnedMeshInstance* zombie_instance;
    int selected_option;
    int current_animation;
    int current_face_image;
    int shirt_palette;
    int pants_palette;
    ZombieAppearance* zombie_appearance;
    fw64SpriteBatch* spritebatch;
    fw64Allocator* allocator;
} ZombieConfigUi;

void zombie_config_ui_init(ZombieConfigUi* ui, fw64Engine* engine, fw64SkinnedMeshInstance* zombie_insatance, ZombieAppearance* zombie_appearance, fw64Allocator* allocator);
void zombie_config_ui_uninit(ZombieConfigUi* ui);
void zombie_config_ui_update(ZombieConfigUi* ui);
void zombie_config_ui_draw(ZombieConfigUi* ui, fw64RenderPass* renderpass);