#pragma once

#include <framework64/animation_controller.h>
#include <framework64/engine.h>

#include "zombie_config.h"

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
    fw64Mesh* zombie_mesh;
    fw64AnimationController* animation_controller;
    int selected_option;
    int current_animation;
    int current_face_image;
    ZombieConfig* config;
    fw64Mesh* mesh;
    fw64Image* face_images[ZOMBIE_FACE_IMAGE_COUNT];
} ZombieConfigUi;

void zombie_config_ui_init(ZombieConfigUi* ui, fw64Engine* engine, fw64Mesh* mesh, ZombieConfig* config, fw64AnimationController* zombie_controller);
void zombie_config_ui_update(ZombieConfigUi* ui);
void zombie_config_ui_draw(ZombieConfigUi* ui);